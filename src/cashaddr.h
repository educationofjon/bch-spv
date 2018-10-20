#ifndef _BCH_CASHADDR_H
#define _BCH_CASHADDR_H

#include <stdint.h>

bool
bch_cashaddr_encode(
  char *output,
  const char *prefix,
  int type,
  const uint8_t *hash,
  size_t hash_len
);

bool
bch_cashaddr_decode(
  int *type,
  uint8_t *hash,
  size_t *hash_len,
  char *prefix,
  const char *default_prefix,
  const char *addr
);
#endif
