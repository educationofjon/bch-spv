#ifndef _BCH_HEADER_H
#define _BCH_HEADER_H

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct bch_header_s {
  uint32_t version;
  uint8_t prev_block[32];
  uint8_t merkle_root[32];
  uint64_t time;
  uint32_t bits;
  uint8_t nonce[4];

  bool cache;
  uint8_t hash[32];
  uint32_t height;
  uint8_t work[32];

  struct bch_header_s *next;
} bch_header_t;

void
bch_header_init(bch_header_t *hdr);

bch_header_t *
bch_header_alloc(void);

bch_header_t *
bch_header_clone(const bch_header_t *hdr);

bool
bch_pow_to_target(uint32_t bits, uint8_t *target);

bool
bch_pow_to_bits(const uint8_t *target, uint32_t *bits);

bool
bch_header_get_proof(const bch_header_t *hdr, uint8_t *proof);

bool
bch_header_calc_work(bch_header_t *hdr, const bch_header_t *prev);

bool
bch_header_read(uint8_t *data, size_t data_len, bch_header_t *hdr);

bool
bch_header_decode(const uint8_t *data, size_t data_len, bch_header_t *hdr);

int
bch_header_write(const bch_header_t *hdr, uint8_t **data);

int
bch_header_size(const bch_header_t *hdr);

int
bch_header_encode(const bch_header_t *hdr, uint8_t *data);

void
bch_header_print(bch_header_t *hdr, const char *prefix);
#endif
