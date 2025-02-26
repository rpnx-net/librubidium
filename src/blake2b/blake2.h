/*
   BLAKE2 reference source code package - reference C implementations

   Written in 2012 by Samuel Neves <sneves@dei.uc.pt>

   To the extent possible under law, the author(s) have dedicated all copyright
   and related and neighboring rights to this software to the public domain
   worldwide. This software is distributed without any warranty.

   All code is triple-licensed under the
   [CC0](http://creativecommons.org/publicdomain/zero/1.0), the
   [OpenSSL Licence](https://www.openssl.org/source/license.html), or
   the [Apache Public License 2.0](http://www.apache.org/licenses/LICENSE-2.0),
   at your choosing.
 */
 /* Copyright 2022 Ryan P. Nicholl <rnicholl@protonmail.com>
  * MIT LICENSE
  */

#ifndef RUBIDIUM_BLAKE2_H
#define RUBIDIUM_BLAKE2_H

#include <cstddef>
#include <cstdint>

#include "rubidium_generichash_blake2b.h"
#include "export.h"

enum blake2b_constant {
    BLAKE2B_BLOCKBYTES    = 128,
    BLAKE2B_OUTBYTES      = 64,
    BLAKE2B_KEYBYTES      = 64,
    BLAKE2B_SALTBYTES     = 16,
    BLAKE2B_PERSONALBYTES = 16
};


typedef struct blake2b_param_ {
    uint8_t digest_length;                   /*  1 */
    uint8_t key_length;                      /*  2 */
    uint8_t fanout;                          /*  3 */
    uint8_t depth;                           /*  4 */
    uint8_t leaf_length[4];                  /*  8 */
    uint8_t node_offset[8];                  /* 16 */
    uint8_t node_depth;                      /* 17 */
    uint8_t inner_length;                    /* 18 */
    uint8_t reserved[14];                    /* 32 */
    uint8_t salt[BLAKE2B_SALTBYTES];         /* 48 */
    uint8_t personal[BLAKE2B_PERSONALBYTES]; /* 64 */
} blake2b_param;

typedef struct blake2b_state {
    uint64_t h[8];
    uint64_t t[2];
    uint64_t f[2];
    uint8_t  buf[2 * 128];
    size_t   buflen;
    uint8_t  last_node;
} blake2b_state;



/* Streaming API */
int _rubidium_blake2b_init(blake2b_state *S, const uint8_t outlen);
int _rubidium_blake2b_init_salt_personal(blake2b_state *S, const uint8_t outlen,
                               const void *salt, const void *personal);
int _rubidium_blake2b_init_key(blake2b_state *S, const uint8_t outlen, const void *key,
                     const uint8_t keylen);
int _rubidium_blake2b_init_key_salt_personal(blake2b_state *S, const uint8_t outlen,
                                   const void *key, const uint8_t keylen,
                                   const void *salt, const void *personal);
int _rubidium_blake2b_init_param(blake2b_state *S, const blake2b_param *P);
int _rubidium_blake2b_update(blake2b_state *S, const uint8_t *in, uint64_t inlen);
int _rubidium_blake2b_final(blake2b_state *S, uint8_t *out, uint8_t outlen);

/* Simple API */
int _rubidium_blake2b(uint8_t *out, const void *in, const void *key, const uint8_t outlen,
            const uint64_t inlen, uint8_t keylen);
int _rubidium_blake2b_salt_personal(uint8_t *out, const void *in, const void *key,
                          const uint8_t outlen, const uint64_t inlen,
                          uint8_t keylen, const void *salt,
                          const void *personal);

typedef int (*blake2b_compress_fn)(blake2b_state *S,
                                   const uint8_t  block[BLAKE2B_BLOCKBYTES]);
int _rubidium_blake2b_pick_best_implementation(void);
int _rubidium_blake2b_compress_ref(blake2b_state *S,
                         const uint8_t  block[BLAKE2B_BLOCKBYTES]);
int _rubidium_blake2b_compress_ssse3(blake2b_state *S,
                           const uint8_t  block[BLAKE2B_BLOCKBYTES]);
int _rubidium_blake2b_compress_sse41(blake2b_state *S,
                           const uint8_t  block[BLAKE2B_BLOCKBYTES]);
int _rubidium_blake2b_compress_avx2(blake2b_state *S,
                          const uint8_t  block[BLAKE2B_BLOCKBYTES]);

#endif
