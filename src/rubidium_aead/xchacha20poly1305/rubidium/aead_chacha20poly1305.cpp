
#include <cstdint>
#include <stdlib.h>
#include <limits.h>
#include <string.h>


#include "rubidium_aead_chacha20poly1305.h"
#include "rubidium_onetimeauth_poly1305.h"
#include "rubidium_stream_chacha20.h"
#include "rubidium_verify_16.h"
#include "randombytes.h"
#include "utils.h"

#include "private/chacha20_ietf_ext.h"
#include "private/common.h"
#include <stdexcept>
static const unsigned char _pad0[16] = { 0 };

int
rubidium_aead_chacha20poly1305_encrypt_detached(unsigned char *c,
                                              unsigned char *mac,
                                              std::size_t *maclen_p,
                                              const unsigned char *m,
                                              std::size_t mlen,
                                              const unsigned char *ad,
                                              std::size_t adlen,
                                              const unsigned char *nsec,
                                              const unsigned char *npub,
                                              const unsigned char *k)
{
    rubidium_onetimeauth_poly1305_state state;
    unsigned char                     block0[64U];
    unsigned char                     slen[8U];

    (void) nsec;
    rubidium_stream_chacha20(block0, sizeof block0, npub, k);
    rubidium_onetimeauth_poly1305_init(&state, block0);
    rubidium_memzero(block0, sizeof block0);

    rubidium_onetimeauth_poly1305_update(&state, ad, adlen);
    store64_le((slen), ((uint64_t) adlen));
    rubidium_onetimeauth_poly1305_update(&state, slen, sizeof slen);

    rubidium_stream_chacha20_xor_ic(c, m, mlen, npub, 1U, k);

    rubidium_onetimeauth_poly1305_update(&state, c, mlen);
    store64_le((slen), ((uint64_t) mlen));
    rubidium_onetimeauth_poly1305_update(&state, slen, sizeof slen);

    rubidium_onetimeauth_poly1305_final(&state, mac);
    rubidium_memzero(&state, sizeof state);

    if (maclen_p != NULL) {
        *maclen_p = rubidium_aead_chacha20poly1305_ABYTES;
    }
    return 0;
}

int
rubidium_aead_chacha20poly1305_encrypt(unsigned char *c,
                                     std::size_t *clen_p,
                                     const unsigned char *m,
                                     std::size_t mlen,
                                     const unsigned char *ad,
                                     std::size_t adlen,
                                     const unsigned char *nsec,
                                     const unsigned char *npub,
                                     const unsigned char *k)
{
    std::size_t clen = 0ULL;
    int                ret;

    if (mlen > rubidium_aead_chacha20poly1305_MESSAGEBYTES_MAX) {
        throw std::invalid_argument("");
    }
    ret = rubidium_aead_chacha20poly1305_encrypt_detached(c,
                                                        c + mlen, NULL,
                                                        m, mlen,
                                                        ad, adlen,
                                                        nsec, npub, k);
    if (clen_p != NULL) {
        if (ret == 0) {
            clen = mlen + rubidium_aead_chacha20poly1305_ABYTES;
        }
        *clen_p = clen;
    }
    return ret;
}

int
rubidium_aead_chacha20poly1305_ietf_encrypt_detached(unsigned char *c,
                                                   unsigned char *mac,
                                                   std::size_t *maclen_p,
                                                   const unsigned char *m,
                                                   std::size_t mlen,
                                                   const unsigned char *ad,
                                                   std::size_t adlen,
                                                   const unsigned char *nsec,
                                                   const unsigned char *npub,
                                                   const unsigned char *k)
{
    rubidium_onetimeauth_poly1305_state state;
    unsigned char                     block0[64U];
    unsigned char                     slen[8U];

    (void) nsec;
    rubidium_stream_chacha20_ietf(block0, sizeof block0, npub, k);
    rubidium_onetimeauth_poly1305_init(&state, block0);
    rubidium_memzero(block0, sizeof block0);

    rubidium_onetimeauth_poly1305_update(&state, ad, adlen);
    rubidium_onetimeauth_poly1305_update(&state, _pad0, (0x10 - adlen) & 0xf);

    rubidium_stream_chacha20_ietf_xor_ic(c, m, mlen, npub, 1U, k);

    rubidium_onetimeauth_poly1305_update(&state, c, mlen);
    rubidium_onetimeauth_poly1305_update(&state, _pad0, (0x10 - mlen) & 0xf);

    store64_le((slen), ((uint64_t) adlen));
    rubidium_onetimeauth_poly1305_update(&state, slen, sizeof slen);

    store64_le((slen), ((uint64_t) mlen));
    rubidium_onetimeauth_poly1305_update(&state, slen, sizeof slen);

    rubidium_onetimeauth_poly1305_final(&state, mac);
    rubidium_memzero(&state, sizeof state);

    if (maclen_p != NULL) {
        *maclen_p = rubidium_aead_chacha20poly1305_ietf_ABYTES;
    }
    return 0;
}

int
rubidium_aead_chacha20poly1305_ietf_encrypt(unsigned char *c,
                                          std::size_t *clen_p,
                                          const unsigned char *m,
                                          std::size_t mlen,
                                          const unsigned char *ad,
                                          std::size_t adlen,
                                          const unsigned char *nsec,
                                          const unsigned char *npub,
                                          const unsigned char *k)
{
    std::size_t clen = 0ULL;
    int                ret;

    if (mlen > rubidium_aead_chacha20poly1305_ietf_MESSAGEBYTES_MAX) {
        throw std::invalid_argument("");
    }
    ret = rubidium_aead_chacha20poly1305_ietf_encrypt_detached(c,
                                                             c + mlen, NULL,
                                                             m, mlen,
                                                             ad, adlen,
                                                             nsec, npub, k);
    if (clen_p != NULL) {
        if (ret == 0) {
            clen = mlen + rubidium_aead_chacha20poly1305_ietf_ABYTES;
        }
        *clen_p = clen;
    }
    return ret;
}

int
rubidium_aead_chacha20poly1305_decrypt_detached(unsigned char *m,
                                              unsigned char *nsec,
                                              const unsigned char *c,
                                              std::size_t clen,
                                              const unsigned char *mac,
                                              const unsigned char *ad,
                                              std::size_t adlen,
                                              const unsigned char *npub,
                                              const unsigned char *k)
{
    rubidium_onetimeauth_poly1305_state state;
    unsigned char                     block0[64U];
    unsigned char                     slen[8U];
    unsigned char                     computed_mac[rubidium_aead_chacha20poly1305_ABYTES];
    std::size_t                mlen;
    int                               ret;

    (void) nsec;
    rubidium_stream_chacha20(block0, sizeof block0, npub, k);
    rubidium_onetimeauth_poly1305_init(&state, block0);
    rubidium_memzero(block0, sizeof block0);

    rubidium_onetimeauth_poly1305_update(&state, ad, adlen);
    store64_le((slen), ((uint64_t) adlen));
    rubidium_onetimeauth_poly1305_update(&state, slen, sizeof slen);

    mlen = clen;
    rubidium_onetimeauth_poly1305_update(&state, c, mlen);
    store64_le((slen), ((uint64_t) mlen));
    rubidium_onetimeauth_poly1305_update(&state, slen, sizeof slen);

    rubidium_onetimeauth_poly1305_final(&state, computed_mac);
    rubidium_memzero(&state, sizeof state);

    static_assert(sizeof computed_mac == 16U);
    ret = rubidium_verify_16(computed_mac, mac);
    rubidium_memzero(computed_mac, sizeof computed_mac);
    if (m == NULL) {
        return ret;
    }
    if (ret != 0) {
        memset(m, 0, mlen);
        return -1;
    }
    rubidium_stream_chacha20_xor_ic(m, c, mlen, npub, 1U, k);

    return 0;
}

int
rubidium_aead_chacha20poly1305_decrypt(unsigned char *m,
                                     std::size_t *mlen_p,
                                     unsigned char *nsec,
                                     const unsigned char *c,
                                     std::size_t clen,
                                     const unsigned char *ad,
                                     std::size_t adlen,
                                     const unsigned char *npub,
                                     const unsigned char *k)
{
    std::size_t mlen = 0ULL;
    int                ret = -1;

    if (clen >= rubidium_aead_chacha20poly1305_ABYTES) {
        ret = rubidium_aead_chacha20poly1305_decrypt_detached
            (m, nsec,
             c, clen - rubidium_aead_chacha20poly1305_ABYTES,
             c + clen - rubidium_aead_chacha20poly1305_ABYTES,
             ad, adlen, npub, k);
    }
    if (mlen_p != NULL) {
        if (ret == 0) {
            mlen = clen - rubidium_aead_chacha20poly1305_ABYTES;
        }
        *mlen_p = mlen;
    }
    return ret;
}

int
rubidium_aead_chacha20poly1305_ietf_decrypt_detached(unsigned char *m,
                                                   unsigned char *nsec,
                                                   const unsigned char *c,
                                                   std::size_t clen,
                                                   const unsigned char *mac,
                                                   const unsigned char *ad,
                                                   std::size_t adlen,
                                                   const unsigned char *npub,
                                                   const unsigned char *k)
{
    rubidium_onetimeauth_poly1305_state state;
    unsigned char                     block0[64U];
    unsigned char                     slen[8U];
    unsigned char                     computed_mac[rubidium_aead_chacha20poly1305_ietf_ABYTES];
    std::size_t                mlen;
    int                               ret;

    (void) nsec;
    rubidium_stream_chacha20_ietf(block0, sizeof block0, npub, k);
    rubidium_onetimeauth_poly1305_init(&state, block0);
    rubidium_memzero(block0, sizeof block0);

    rubidium_onetimeauth_poly1305_update(&state, ad, adlen);
    rubidium_onetimeauth_poly1305_update(&state, _pad0, (0x10 - adlen) & 0xf);

    mlen = clen;
    rubidium_onetimeauth_poly1305_update(&state, c, mlen);
    rubidium_onetimeauth_poly1305_update(&state, _pad0, (0x10 - mlen) & 0xf);

    store64_le((slen), ((uint64_t) adlen));
    rubidium_onetimeauth_poly1305_update(&state, slen, sizeof slen);

    store64_le((slen), ((uint64_t) mlen));
    rubidium_onetimeauth_poly1305_update(&state, slen, sizeof slen);

    rubidium_onetimeauth_poly1305_final(&state, computed_mac);
    rubidium_memzero(&state, sizeof state);

    static_assert(sizeof computed_mac == 16U);
    ret = rubidium_verify_16(computed_mac, mac);
    rubidium_memzero(computed_mac, sizeof computed_mac);
    if (m == NULL) {
        return ret;
    }
    if (ret != 0) {
        memset(m, 0, mlen);
        return -1;
    }
    rubidium_stream_chacha20_ietf_xor_ic(m, c, mlen, npub, 1U, k);

    return 0;
}

int
rubidium_aead_chacha20poly1305_ietf_decrypt(unsigned char *m,
                                          std::size_t *mlen_p,
                                          unsigned char *nsec,
                                          const unsigned char *c,
                                          std::size_t clen,
                                          const unsigned char *ad,
                                          std::size_t adlen,
                                          const unsigned char *npub,
                                          const unsigned char *k)
{
    std::size_t mlen = 0ULL;
    int                ret = -1;

    if (clen >= rubidium_aead_chacha20poly1305_ietf_ABYTES) {
        ret = rubidium_aead_chacha20poly1305_ietf_decrypt_detached
            (m, nsec,
             c, clen - rubidium_aead_chacha20poly1305_ietf_ABYTES,
             c + clen - rubidium_aead_chacha20poly1305_ietf_ABYTES,
             ad, adlen, npub, k);
    }
    if (mlen_p != NULL) {
        if (ret == 0) {
            mlen = clen - rubidium_aead_chacha20poly1305_ietf_ABYTES;
        }
        *mlen_p = mlen;
    }
    return ret;
}

size_t
rubidium_aead_chacha20poly1305_ietf_keybytes(void)
{
    return rubidium_aead_chacha20poly1305_ietf_KEYBYTES;
}

size_t
rubidium_aead_chacha20poly1305_ietf_npubbytes(void)
{
    return rubidium_aead_chacha20poly1305_ietf_NPUBBYTES;
}

size_t
rubidium_aead_chacha20poly1305_ietf_nsecbytes(void)
{
    return rubidium_aead_chacha20poly1305_ietf_NSECBYTES;
}

size_t
rubidium_aead_chacha20poly1305_ietf_abytes(void)
{
    return rubidium_aead_chacha20poly1305_ietf_ABYTES;
}

size_t
rubidium_aead_chacha20poly1305_ietf_messagebytes_max(void)
{
    return rubidium_aead_chacha20poly1305_ietf_MESSAGEBYTES_MAX;
}

void
rubidium_aead_chacha20poly1305_ietf_keygen(unsigned char k[rubidium_aead_chacha20poly1305_ietf_KEYBYTES])
{
    rubidium::randombytes_fill(k, rubidium_aead_chacha20poly1305_ietf_KEYBYTES);
}

size_t
rubidium_aead_chacha20poly1305_keybytes(void)
{
    return rubidium_aead_chacha20poly1305_KEYBYTES;
}

size_t
rubidium_aead_chacha20poly1305_npubbytes(void)
{
    return rubidium_aead_chacha20poly1305_NPUBBYTES;
}

size_t
rubidium_aead_chacha20poly1305_nsecbytes(void)
{
    return rubidium_aead_chacha20poly1305_NSECBYTES;
}

size_t
rubidium_aead_chacha20poly1305_abytes(void)
{
    return rubidium_aead_chacha20poly1305_ABYTES;
}

size_t
rubidium_aead_chacha20poly1305_messagebytes_max(void)
{
    return rubidium_aead_chacha20poly1305_MESSAGEBYTES_MAX;
}

void
rubidium_aead_chacha20poly1305_keygen(unsigned char k[rubidium_aead_chacha20poly1305_KEYBYTES])
{
    rubidium::randombytes_fill(k, rubidium_aead_chacha20poly1305_KEYBYTES);
}
