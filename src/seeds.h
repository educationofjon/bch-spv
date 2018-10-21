#include "constants.h"

#if BCH_NETWORK == BCH_MAIN

/*
 * Main
 */

static const char *bch_seeds[] = {
  "seed.bitcoinabc.org",
  "seed-abc.bitcoinforks.org",
  "btccash-seeder.bitcoinunlimited.info",
  "seed.bitprim.org",
  "seed.deadalnix.me",
  "seeder.criptolayer.net",
  NULL
};

#elif BCH_NETWORK == BCH_TESTNET

/*
 * Testnet
 */

static const char *bch_seeds[] = {
  "testnet-seed.bitcoinabc.org",
  "testnet-seed-abc.bitcoinforks.org",
  "testnet-seed.bitprim.org",
  "testnet-seed.deadalnix.me",
  "testnet-seeder.criptolayer.net",
  NULL
};

#elif BCH_NETWORK == BCH_REGTEST

/*
 * Regtest
 */

static const char *bch_seeds[] = {
  NULL
};

#else

/*
 * Bad Network
 */

#error "Invalid network."

#endif
