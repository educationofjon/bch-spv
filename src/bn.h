/**
 * Parts of this software are based on tiny-bignum-c:
 * https://github.com/kokke/tiny-bignum-c
 *
 * tiny-bignum-c resides in the public domain.
 */

#ifndef _BCH_BN_H
#define _BCH_BN_H

#include <assert.h>
#include <stdint.h>

#define BCH_BN_SIZE (64 / 4)
#define BCH_BN_MSB ((uint64_t)0x80000000)
#define BCH_BN_MAX ((uint64_t)0xffffffff)

typedef struct bch_bn_s {
  uint32_t array[BCH_BN_SIZE];
} bch_bn_t;

/*
 * Initialization functions
 */

void
bch_bn_init(bch_bn_t *n);

void
bch_bn_from_int(bch_bn_t *n, uint64_t i);

uint64_t
bch_bn_to_int(const bch_bn_t *n);

void
bch_bn_from_string(bch_bn_t *n, const char *str, int nbytes);

void
bch_bn_to_string(const bch_bn_t *n, char *str, int maxsize);

void
bch_bn_from_array(bch_bn_t *n, const uint8_t *array, size_t size);

void
bch_bn_to_array(const bch_bn_t *n, uint8_t *array, size_t size);

/*
 * Basic Arithmetic Operations
 */

void
bch_bn_add(const bch_bn_t *a, const bch_bn_t *b, bch_bn_t *c); // c = a + b

void
bch_bn_sub(const bch_bn_t *a, const bch_bn_t *b, bch_bn_t *c); // c = a - b

void
bch_bn_mul(const bch_bn_t *a, const bch_bn_t *b, bch_bn_t *c); // c = a * b

void
bch_bn_div(const bch_bn_t *a, const bch_bn_t *b, bch_bn_t *c); // c = a / b

void
bch_bn_mod(const bch_bn_t *a, const bch_bn_t *b, bch_bn_t *c); // c = a % b

/*
 * Bitwise Operations
 */

void
bch_bn_and(const bch_bn_t *a, const bch_bn_t *b, bch_bn_t *c); // c = a & b

void
bch_bn_or(const bch_bn_t *a, const bch_bn_t *b, bch_bn_t *c); // c = a | b

void
bch_bn_xor(const bch_bn_t *a, const bch_bn_t *b, bch_bn_t *c); // c = a ^ b

void
bch_bn_lshift(bch_bn_t *a, bch_bn_t *b, int nbits); // a = b << nbits)

void
bch_bn_rshift(bch_bn_t *a, bch_bn_t *b, int nbits); // b = a >> nbits)

/*
 * Special operations and comparison
 */

int
bch_bn_cmp(const bch_bn_t *a, const bch_bn_t *b);

int
bch_bn_is_zero(const bch_bn_t *n);

void
bch_bn_neg(bch_bn_t *n);

void
bch_bn_inc(bch_bn_t *n);

void
bch_bn_dec(bch_bn_t *n);

void
bch_bn_pow(const bch_bn_t *a, bch_bn_t *b, bch_bn_t *c);

void
bch_bn_assign(bch_bn_t *dst, const bch_bn_t *src);

#endif
