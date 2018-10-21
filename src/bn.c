/**
 * Parts of this software are based on tiny-bignum-c:
 * https://github.com/kokke/tiny-bignum-c
 *
 * tiny-bignum-c resides in the public domain.
 */

#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "bn.h"

static void _lshift_one_bit(bch_bn_t *a);
static void _rshift_one_bit(bch_bn_t *a);
static void _lshift_word(bch_bn_t *a, int nwords);
static void _rshift_word(bch_bn_t *a, int nwords);

void
bch_bn_init(bch_bn_t *n) {
  assert(n && "n is null");

  int i;
  for (i = 0; i < BCH_BN_SIZE; i++)
    n->array[i] = 0;
}

void
bch_bn_from_int(bch_bn_t *n, uint64_t i) {
  assert(n && "n is null");

  bch_bn_init(n);

  n->array[0] = (uint32_t)i;
  n->array[1] = (uint32_t)(i >> 32);
}

uint64_t
bch_bn_to_int(const bch_bn_t *n) {
  assert(n && "n is null");

  uint64_t ret = 0;

  ret |= (uint64_t)n->array[0];
  ret |= ((uint64_t)n->array[1]) << 32;

  return ret;
}

void
bch_bn_from_string(bch_bn_t *n, const char *str, int nbytes) {
  assert(n && "n is null");
  assert(str && "str is null");
  assert(nbytes > 0 && "nbytes must be positive");
  assert((nbytes & 1) == 0 && "odd input size");

  bch_bn_init(n);

  uint32_t tmp;

  // index into string
  int i = nbytes - (2 * 4);

  // index into array
  int j = 0;

  // reading last hex-byte "MSB" from string first -> big endian
  // MSB ~= most significant byte / block ? :)
  while (i >= 0) {
    tmp = 0;

    sscanf(&str[i], "%8x", &tmp);

    n->array[j] = tmp;

    // step 4 hex-byte(s) back in the string.
    i -= (2 * 4);

    // step one element forward in the array.
    j += 1;
  }
}

void
bch_bn_to_string(const bch_bn_t *n, char *str, int nbytes) {
  assert(n && "n is null");
  assert(str && "str is null");
  assert(nbytes > 0 && "nbytes must be positive");
  assert((nbytes & 1) == 0 && "odd output size");

  // index into array - reading
  // "MSB" first -> big-endian
  int j = BCH_BN_SIZE - 1;

  // index into string representation.
  int i = 0;

  // reading last array-element
  // "MSB" first -> big endian
  while ((j >= 0) && (nbytes > (i + 1))) {
    sprintf(&str[i], "%.08x", n->array[j]);

    // step 4 hex-byte(s)
    // forward in the string.
    i += (2 * 4);

    // step one element back
    // in the array.
    j -= 1;
  }

  // Count leading zeros:
  j = 0;
  while (str[j] == '0')
    j += 1;

  // Move string j places ahead,
  // effectively skipping leading zeros
  for (i = 0; i < (nbytes - j); i++)
    str[i] = str[i + j];

  // Zero-terminate string
  str[i] = 0;
}

void
bch_bn_from_array(bch_bn_t *n, const uint8_t *array, size_t size) {
  assert(n && "n is null");
  assert(array && "array is null");

  bch_bn_init(n);

  int j = (size / 4) - 1;
  int i = 0;

  for (; j >= 0; j--) {
    n->array[j] = ((uint32_t)array[i++]) << 24;
    n->array[j] |= ((uint32_t)array[i++]) << 16;
    n->array[j] |= ((uint32_t)array[i++]) << 8;
    n->array[j] |= ((uint32_t)array[i++]);
  }
}

void
bch_bn_to_array(const bch_bn_t *n, uint8_t *array, size_t size) {
  assert(n && "n is null");
  assert(array && "array is null");

  int j = (size / 4) - 1;
  int i = 0;

  for (; j >= 0; j--) {
    array[i++] = (uint8_t)(n->array[j] >> 24);
    array[i++] = (uint8_t)(n->array[j] >> 16);
    array[i++] = (uint8_t)(n->array[j] >> 8);
    array[i++] = (uint8_t)n->array[j];
  }
}

void
bch_bn_neg(bch_bn_t *n) {
  assert(n && "n is null");

  uint32_t res;
  uint64_t tmp; // copy of n

  int i;
  for (i = 0; i < BCH_BN_SIZE; i++) {
    tmp = n->array[i];
    res = ~tmp;
    n->array[i] = res;
  }
}

void
bch_bn_dec(bch_bn_t *n) {
  assert(n && "n is null");

  uint32_t tmp; // copy of n
  uint32_t res;

  int i;
  for (i = 0; i < BCH_BN_SIZE; i++) {
    tmp = n->array[i];
    res = tmp - 1;
    n->array[i] = res;

    if (!(res > tmp))
      break;
  }
}

void
bch_bn_inc(bch_bn_t *n) {
  assert(n && "n is null");

  uint32_t res;
  uint64_t tmp; // copy of n

  int i;
  for (i = 0; i < BCH_BN_SIZE; i++) {
    tmp = n->array[i];
    res = tmp + 1;
    n->array[i] = res;

    if (res > tmp)
      break;
  }
}

void
bch_bn_add(const bch_bn_t *a, const bch_bn_t *b, bch_bn_t *c) {
  assert(a && "a is null");
  assert(b && "b is null");
  assert(c && "c is null");

  uint64_t tmp;
  int carry = 0;

  int i;
  for (i = 0; i < BCH_BN_SIZE; i++) {
    tmp = (uint64_t)a->array[i] + (uint64_t)b->array[i] + carry;
    carry = (tmp > BCH_BN_MAX);
    c->array[i] = (tmp & BCH_BN_MAX);
  }
}

void
bch_bn_sub(const bch_bn_t *a, const bch_bn_t *b, bch_bn_t *c) {
  assert(a && "a is null");
  assert(b && "b is null");
  assert(c && "c is null");

  uint64_t res;
  uint64_t tmp1;
  uint64_t tmp2;
  int borrow = 0;

  int i;
  for (i = 0; i < BCH_BN_SIZE; i++) {
    // + number_base
    tmp1 = (uint64_t)a->array[i] + (BCH_BN_MAX + 1);
    tmp2 = (uint64_t)b->array[i] + borrow;;
    res = (tmp1 - tmp2);

    // "modulo number_base" == "% (number_base - 1)"
    // if number_base is 2^N
    c->array[i] = (uint32_t)(res & BCH_BN_MAX);
    borrow = (res <= BCH_BN_MAX);
  }
}

void
bch_bn_mul(const bch_bn_t *a, const bch_bn_t *b, bch_bn_t *c) {
  assert(a && "a is null");
  assert(b && "b is null");
  assert(c && "c is null");

  bch_bn_t row;
  bch_bn_t tmp;
  bch_bn_t cc;
  int i, j;

  bch_bn_init(&cc);

  for (i = 0; i < BCH_BN_SIZE; i++) {
    bch_bn_init(&row);

    for (j = 0; j < BCH_BN_SIZE; j++) {
      if (i + j < BCH_BN_SIZE) {
        bch_bn_init(&tmp);

        uint64_t intermediate =
          ((uint64_t)a->array[i] * (uint64_t)b->array[j]);

        bch_bn_from_int(&tmp, intermediate);
        _lshift_word(&tmp, i + j);
        bch_bn_add(&tmp, &row, &row);
      }
    }

    bch_bn_add(&cc, &row, &cc);
  }

  bch_bn_assign(c, &cc);
}

void
bch_bn_div(const bch_bn_t *a, const bch_bn_t *b, bch_bn_t *c) {
  assert(a && "a is null");
  assert(b && "b is null");
  assert(c && "c is null");

  bch_bn_t current;
  bch_bn_t denom;
  bch_bn_t tmp;

  // int current = 1;
  bch_bn_from_int(&current, 1);
  // denom = b
  bch_bn_assign(&denom, b);
  // tmp = a
  bch_bn_assign(&tmp, a);

  const uint64_t half_max = 1 + (uint64_t)(BCH_BN_MAX / 2);
  bool overflow = false;

  // while (denom <= a) {
  while (bch_bn_cmp(&denom, a) != 1) {
    if (denom.array[BCH_BN_SIZE - 1] >= half_max) {
      overflow = true;
      break;
    }

    // current <<= 1;
    _lshift_one_bit(&current);

    // denom <<= 1;
    _lshift_one_bit(&denom);
  }

  if (!overflow) {
    // denom >>= 1;
    _rshift_one_bit(&denom);
    // current >>= 1;
    _rshift_one_bit(&current);
  }

  // int answer = 0;
  bch_bn_init(c);

  // while (current != 0)
  while (!bch_bn_is_zero(&current)) {
    // if (dividend >= denom)
    if (bch_bn_cmp(&tmp, &denom) != -1)  {
      // dividend -= denom;
      bch_bn_sub(&tmp, &denom, &tmp);
      // answer |= current;
      bch_bn_or(c, &current, c);
    }

    // current >>= 1;
    _rshift_one_bit(&current);

    // denom >>= 1;
    _rshift_one_bit(&denom);
  }

  // return answer;
}

void
bch_bn_lshift(bch_bn_t *a, bch_bn_t *b, int nbits) {
  assert(a && "a is null");
  assert(b && "b is null");
  assert(nbits >= 0 && "no negative shifts");

  // Handle shift in multiples of word-size
  const int nbits_pr_word = 4 * 8;
  int nwords = nbits / nbits_pr_word;

  if (nwords != 0) {
    _lshift_word(a, nwords);
    nbits -= (nwords * nbits_pr_word);
  }

  if (nbits != 0) {
    int i;
    for (i = (BCH_BN_SIZE - 1); i > 0; i--) {
      a->array[i] = (a->array[i] << nbits)
        | (a->array[i - 1] >> ((8 * 4) - nbits));
    }

    a->array[i] <<= nbits;
  }

  bch_bn_assign(b, a);
}

void
bch_bn_rshift(bch_bn_t *a, bch_bn_t *b, int nbits) {
  assert(a && "a is null");
  assert(b && "b is null");
  assert(nbits >= 0 && "no negative shifts");

  // Handle shift in multiples of word-size
  const int nbits_pr_word = (4 * 8);
  int nwords = nbits / nbits_pr_word;

  if (nwords != 0) {
    _rshift_word(a, nwords);
    nbits -= (nwords * nbits_pr_word);
  }

  if (nbits != 0) {
    int i;
    for (i = 0; i < (BCH_BN_SIZE - 1); i++) {
      a->array[i] = (a->array[i] >> nbits)
        | (a->array[i + 1] << ((8 * 4) - nbits));
    }

    a->array[i] >>= nbits;
  }

  bch_bn_assign(b, a);
}

void
bch_bn_mod(const bch_bn_t *a, const bch_bn_t *b, bch_bn_t *c) {
  // mod(a, b) = a - ((a / b) * b)
  // example:
  //   mod(8, 3) = 8 - ((8 / 3) * 3) = 2
  assert(a && "a is null");
  assert(b && "b is null");
  assert(c && "c is null");

  bch_bn_t tmp;

  // c = (a / b)
  bch_bn_div(a, b, c);

  // tmp = (c * b)
  bch_bn_mul(c, b, &tmp);

  // c = a - tmp
  bch_bn_sub(a, &tmp, c);
}

void
bch_bn_and(const bch_bn_t *a, const bch_bn_t *b, bch_bn_t *c) {
  assert(a && "a is null");
  assert(b && "b is null");
  assert(c && "c is null");

  int i;
  for (i = 0; i < BCH_BN_SIZE; i++)
    c->array[i] = (a->array[i] & b->array[i]);
}

void
bch_bn_or(const bch_bn_t *a, const bch_bn_t *b, bch_bn_t *c) {
  assert(a && "a is null");
  assert(b && "b is null");
  assert(c && "c is null");

  int i;
  for (i = 0; i < BCH_BN_SIZE; i++)
    c->array[i] = (a->array[i] | b->array[i]);
}

void
bch_bn_xor(const bch_bn_t *a, const bch_bn_t *b, bch_bn_t *c) {
  assert(a && "a is null");
  assert(b && "b is null");
  assert(c && "c is null");

  int i;
  for (i = 0; i < BCH_BN_SIZE; i++)
    c->array[i] = (a->array[i] ^ b->array[i]);
}

int
bch_bn_cmp(const bch_bn_t *a, const bch_bn_t *b) {
  assert(a && "a is null");
  assert(b && "b is null");

  int i = BCH_BN_SIZE;

  do {
    // Decrement first, to start
    // with last array element
    i -= 1;

    if (a->array[i] > b->array[i])
      return 1;
    else if (a->array[i] < b->array[i])
      return -1;
  } while (i != 0);

  return 0;
}

int
bch_bn_is_zero(const bch_bn_t *n) {
  assert(n && "n is null");

  int i;
  for (i = 0; i < BCH_BN_SIZE; i++) {
    if (n->array[i])
      return 0;
  }

  return 1;
}

void
bch_bn_pow(const bch_bn_t *a, bch_bn_t *b, bch_bn_t *c) {
  assert(a && "a is null");
  assert(b && "b is null");
  assert(c && "c is null");

  bch_bn_t tmp;
  bch_bn_t cc;

  bch_bn_init(&cc);

  if (bch_bn_cmp(b, &cc) == 0) {
    // Return 1 when exponent
    // is 0 -- n^0 = 1
    bch_bn_inc(&cc);
  } else {
    // Copy a -> tmp
    bch_bn_assign(&tmp, a);

    bch_bn_dec(b);

    // Begin summing products:
    while (!bch_bn_is_zero(b)) {
      // c = tmp * tmp
      bch_bn_mul(&tmp, a, &cc);

      // Decrement b by one
      bch_bn_dec(b);

      bch_bn_assign(&tmp, &cc);
    }

    // c = tmp
    bch_bn_assign(&cc, &tmp);
  }

  bch_bn_assign(c, &cc);
}

void
bch_bn_assign(bch_bn_t *dst, const bch_bn_t *src) {
  assert(dst && "dst is null");
  assert(src && "src is null");

  int i;
  for (i = 0; i < BCH_BN_SIZE; i++)
    dst->array[i] = src->array[i];
}

static void
_rshift_word(bch_bn_t *a, int nwords) {
  // Naive method:
  assert(a && "a is null");
  assert(nwords >= 0 && "no negative shifts");

  int i;
  for (i = 0; i < nwords; i++)
    a->array[i] = a->array[i + 1];

  for (; i < BCH_BN_SIZE; i++)
    a->array[i] = 0;
}

static void
_lshift_word(bch_bn_t *a, int nwords) {
  assert(a && "a is null");
  assert(nwords >= 0 && "no negative shifts");

  int i;

  // Shift whole words
  for (i = (BCH_BN_SIZE - 1); i >= nwords; i--)
    a->array[i] = a->array[i - nwords];

  // Zero pad shifted words.
  for (; i >= 0; i--)
    a->array[i] = 0;
}

static void
_lshift_one_bit(bch_bn_t *a) {
  assert(a && "a is null");

  int i;
  for (i = (BCH_BN_SIZE - 1); i > 0; i--) {
    a->array[i] = (a->array[i] << 1)
      | (a->array[i - 1] >> ((8 * 4) - 1));
  }

  a->array[0] <<= 1;
}

static void
_rshift_one_bit(bch_bn_t *a) {
  assert(a && "a is null");

  int i;
  for (i = 0; i < (BCH_BN_SIZE - 1); i++) {
    a->array[i] = (a->array[i] >> 1)
      | (a->array[i + 1] << ((8 * 4) - 1));
  }

  a->array[BCH_BN_SIZE - 1] >>= 1;
}
