#ifndef _BCH_CONSTANTS_H
#define _BCH_CONSTANTS_H

#include "genesis.h"

#define BCH_MAIN 0
#define BCH_TESTNET 1
#define BCH_REGTEST 2

#ifndef BCH_NETWORK
#define BCH_NETWORK BCH_REGTEST

#define BCH_MAX_MESSAGE (32 * 1000 * 1000)
#define BCH_USER_AGENT "/bch:0.0.1/"
#define BCH_PROTO_VERSION 1
#define BCH_SERVICES 0


static const uint8_t BCH_ZERO_HASH[32] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

#if BCH_NETWORK == BCH_MAIN

/*
 * Main
 */

#define BCH_NETWORK_NAME "main"
#define BCH_MAGIC 0xe8f3e1e3
#define BCH_PORT 8333
#define BCH_BITS 0x1d00ffff

static const uint8_t BCH_CHAINWORK[32] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xb8, 0x70, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

#define BCH_TARGET_SPACING (10 * 60)
#define BCH_TARGET_TIMESPAN (14 * 24 * 60 * 60)
#define BCH_TARGET_RESET false
#define BCH_NO_RETARGETTING false
#define BCH_GENESIS BCH_GENESIS_MAIN

#define BCH_USE_CHECKPOINTS false
#define BCH_LAST_CHECKPOINT 530359
#define BCH_MAX_TIP_AGE (24 * 60 * 60)

#else

/*
 * Bad Network
 */

#error "Invalid network."

#endif
