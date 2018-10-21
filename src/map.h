/*
 * Parts of this software are based on khash.h:
 *
 *  The MIT License
 *
 *  Copyright (c) 2008, 2009, 2011 by Attractive Chaos <attractor@live.co.uk>
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software and associated documentation files (the
 *  "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so, subject to
 *  the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 *  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 *  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#ifndef _BCH_MAP_H
#define _BCH_MAP_H

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <stdbool.h>

typedef uint32_t (*bch_map_hash_func)(const void *key);
typedef bool (*bch_map_equal_func)(const void *a, const void *b);
typedef void (*bch_map_free_func)(void *ptr);

typedef struct bch_map_s {
  uint32_t n_buckets;
  uint32_t size;
  uint32_t n_occupied;
  uint32_t upper_bound;
  uint32_t *flags;
  void **keys;
  void **vals;
  bool is_map;
  bch_map_hash_func hash_func;
  bch_map_equal_func equal_func;
  bch_map_free_func free_func;
} bch_map_t;

typedef uint32_t bch_map_iter_t;

#define __bch_isempty(f, i) ((f[i >> 4] >> ((i & 0xfu) << 1)) & 2)
#define __bch_isdel(f, i) ((f[i >> 4] >> ((i & 0xfu) << 1)) & 1)
#define __bch_iseither(f, i) ((f[i >> 4] >> ((i & 0xfu) << 1)) & 3)
#define __bch_set_isdel_false(f, i) (f[i >> 4] &= ~(1ul << ((i & 0xfu) << 1)))
#define __bch_set_isempty_false(f, i) (f[i >> 4] &= ~(2ul << ((i & 0xfu) << 1)))
#define __bch_set_isboth_false(f, i) (f[i >> 4] &= ~(3ul << ((i & 0xfu) << 1)))
#define __bch_set_isdel_true(f, i) (f[i >> 4] |= 1ul << ((i & 0xfu) << 1))
#define __bch_fsize(m) ((m) < 16 ? 1 : (m) >> 4)
#define __bch_roundup32(x) \
  do {                     \
    --(x);                 \
    (x) |= (x) >> 1;       \
    (x) |= (x) >> 2;       \
    (x) |= (x) >> 4;       \
    (x) |= (x) >> 8;       \
    (x) |= (x) >> 16;      \
    ++(x);                 \
  } while (0)

static const double __bch_hash_upper = 0.77;

#define bch_map_begin(map) ((bch_map_iter_t)0)
#define bch_map_end(map) ((map)->n_buckets)
#define bch_map_exists(map, i) (!__bch_iseither((map)->flags, (i)))
#define bch_map_key(map, i) ((map)->keys[i])
#define bch_map_value(map, i) ((map)->vals[i])

#define bch_map_each(map, kvar, vvar, code)                         \
  do {                                                              \
    bch_map_iter_t __i;                                             \
    for (__i = bch_map_begin(map); __i < bch_map_end(map); __i++) { \
      if (!bch_map_exists(map, i))                                  \
        continue;                                                   \
                                                                    \
      (kvar) = bch_map_key(map, __i);                               \
      (vvar) = bch_map_value(map, __i);                             \
                                                                    \
      code;                                                         \
    }                                                               \
  } while (0)

#define bch_map_each_value(map, vvar, code)                         \
  do {                                                              \
    bch_map_iter_t __i;                                             \
    for (__i = bch_map_begin(map); __i < bch_map_end(map); __i++) { \
      if (!bch_map_exists(map, i))                                  \
        continue;                                                   \
                                                                    \
      (vvar) = bch_map_value(map, __i);                             \
                                                                    \
      code;                                                         \
    }                                                               \
  } while (0)

void
bch_map_init(
  bch_map_t *map,
  bool is_map,
  bch_map_hash_func hash_func,
  bch_map_equal_func equal_func,
  bch_map_free_func free_func
);

void
bch_map_init_map(
  bch_map_t *map,
  bch_map_hash_func hash_func,
  bch_map_equal_func equal_func,
  bch_map_free_func free_func
);

void
bch_map_init_set(
  bch_map_t *map,
  bch_map_hash_func hash_func,
  bch_map_equal_func equal_func
);

void
bch_map_init_hash_map(bch_map_t *map, bch_map_free_func free_func);

void
bch_map_init_hash_set(bch_map_t *map);

void
bch_map_init_hash160_map(bch_map_t *map, bch_map_free_func free_func);

void
bch_map_init_hash160_set(bch_map_t *map);

void
bch_map_init_str_map(bch_map_t *map, bch_map_free_func free_func);

void
bch_map_init_str_set(bch_map_t *map);

void
bch_map_init_int_map(bch_map_t *map, bch_map_free_func free_func);

void
bch_map_init_int_set(bch_map_t *map);

void
bch_map_uninit(bch_map_t *map);

bch_map_t *
bch_map_alloc(
  bool is_map,
  bch_map_hash_func hash_func,
  bch_map_equal_func equal_func,
  bch_map_free_func free_func
);

bch_map_t *
bch_map_alloc_map(
  bch_map_hash_func hash_func,
  bch_map_equal_func equal_func,
  bch_map_free_func free_func
);

bch_map_t *
bch_map_alloc_set(
  bch_map_hash_func hash_func,
  bch_map_equal_func equal_func
);

bch_map_t *
bch_map_alloc_hash_map(bch_map_free_func free_func);

bch_map_t *
bch_map_alloc_hash_set(void);

bch_map_t *
bch_map_alloc_hash160_map(bch_map_free_func free_func);

bch_map_t *
bch_map_alloc_hash160_set(void);

bch_map_t *
bch_map_alloc_str_map(bch_map_free_func free_func);

bch_map_t *
bch_map_alloc_str_set(void);

bch_map_t *
bch_map_alloc_int_map(bch_map_free_func free_func);

bch_map_t *
bch_map_alloc_int_set(void);

void
bch_map_free(bch_map_t *map);

void
bch_map_reset(bch_map_t *map);

uint32_t
bch_map_lookup(const bch_map_t *map, const void *key);

int
bch_map_resize(bch_map_t *map, uint32_t new_n_buckets);

uint32_t
bch_map_put(bch_map_t *map, const void *key, int *ret);

void
bch_map_delete(bch_map_t *map, uint32_t x);

void
bch_map_clear(bch_map_t *map);

bool
bch_map_set(bch_map_t *map, const void *key, void *value);

void *
bch_map_get(const bch_map_t *map, const void *key);

bool
bch_map_has(const bch_map_t *map, const void *key);

bool
bch_map_del(bch_map_t *map, const void *key);

uint32_t
bch_map_hash_str(const void *key);

bool
bch_map_equal_str(const void *a, const void *b);

uint32_t
bch_map_hash_int(const void *key);

bool
bch_map_equal_int(const void *a, const void *b);

uint32_t
bch_map_hash_hash(const void *key);

bool
bch_map_equal_hash(const void *a, const void *b);

uint32_t
bch_map_hash_hash160(const void *key);

bool
bch_map_equal_hash160(const void *a, const void *b);

uint32_t
bch_map_murmur3(const uint8_t *data, size_t data_len, uint32_t seed);

uint32_t
bch_map_tweak3(
  const uint8_t *data,
  size_t data_len,
  uint32_t n,
  uint32_t tweak
);
#endif
