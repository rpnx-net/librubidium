
#include <assert.h>
#include <limits.h>
#include <cstdint>

#include "blake2.h"
#include "rubidium_generichash_blake2b.h"
#include "private/common.h"
#include "private/implementations.h"

int
rubidium_generichash_blake2b(unsigned char *out, size_t outlen,
                           const unsigned char *in, std::size_t inlen,
                           const unsigned char *key, size_t keylen)
{
    if (outlen <= 0U || outlen > BLAKE2B_OUTBYTES ||
        keylen > BLAKE2B_KEYBYTES || inlen > UINT64_MAX) {
        return -1;
    }
    assert(outlen <= UINT8_MAX);
    assert(keylen <= UINT8_MAX);

    return _rubidium_blake2b((uint8_t *) out, in, key, (uint8_t) outlen, (uint64_t) inlen,
                   (uint8_t) keylen);
}

int
rubidium_generichash_blake2b_salt_personal(
    unsigned char *out, size_t outlen, const unsigned char *in,
    std::size_t inlen, const unsigned char *key, size_t keylen,
    const unsigned char *salt, const unsigned char *personal)
{
    if (outlen <= 0U || outlen > BLAKE2B_OUTBYTES ||
        keylen > BLAKE2B_KEYBYTES || inlen > UINT64_MAX) {
        return -1;
    }
    assert(outlen <= UINT8_MAX);
    assert(keylen <= UINT8_MAX);

    return _rubidium_blake2b_salt_personal((uint8_t *) out, in, key, (uint8_t) outlen,
                                 (uint64_t) inlen, (uint8_t) keylen, salt,
                                 personal);
}

int
rubidium_generichash_blake2b_init(rubidium_generichash_blake2b_state *state,
                                const unsigned char *key, const size_t keylen,
                                const size_t outlen)
{
    if (outlen <= 0U || outlen > BLAKE2B_OUTBYTES ||
        keylen > BLAKE2B_KEYBYTES) {
        return -1;
    }
    assert(outlen <= UINT8_MAX);
    assert(keylen <= UINT8_MAX);
    static_assert(sizeof(blake2b_state) <= sizeof *state);
    if (key == NULL || keylen <= 0U) {
        if (_rubidium_blake2b_init((blake2b_state *) (void *) state, (uint8_t) outlen) != 0) {
            return -1; /* LCOV_EXCL_LINE */
        }
    } else if (_rubidium_blake2b_init_key((blake2b_state *) (void *) state, (uint8_t) outlen, key,
                                (uint8_t) keylen) != 0) {
        return -1; /* LCOV_EXCL_LINE */
    }
    return 0;
}

int
rubidium_generichash_blake2b_init_salt_personal(
    rubidium_generichash_blake2b_state *state, const unsigned char *key,
    const size_t keylen, const size_t outlen, const unsigned char *salt,
    const unsigned char *personal)
{
    if (outlen <= 0U || outlen > BLAKE2B_OUTBYTES ||
        keylen > BLAKE2B_KEYBYTES) {
        return -1;
    }
    assert(outlen <= UINT8_MAX);
    assert(keylen <= UINT8_MAX);
    if (key == NULL || keylen <= 0U) {
        if (_rubidium_blake2b_init_salt_personal((blake2b_state *) (void *) state,
                                       (uint8_t) outlen, salt, personal) != 0) {
            return -1; /* LCOV_EXCL_LINE */
        }
    } else if (_rubidium_blake2b_init_key_salt_personal((blake2b_state *) (void *) state,
                                              (uint8_t) outlen, key,
                                              (uint8_t) keylen, salt,
                                              personal) != 0) {
        return -1; /* LCOV_EXCL_LINE */
    }
    return 0;
}

int
rubidium_generichash_blake2b_update(rubidium_generichash_blake2b_state *state,
                                  const unsigned char *in,
                                  std::size_t inlen)
{
    return _rubidium_blake2b_update((blake2b_state *) (void *) state,
                          (const uint8_t *) in, (uint64_t) inlen);
}

int
rubidium_generichash_blake2b_final(rubidium_generichash_blake2b_state *state,
                                 unsigned char *out, const size_t outlen)
{
    assert(outlen <= UINT8_MAX);
    return _rubidium_blake2b_final((blake2b_state *) (void *) state,
                         (uint8_t *) out, (uint8_t) outlen);
}

int
_rubidium_generichash_blake2b_pick_best_implementation(void)
{
    return _rubidium_blake2b_pick_best_implementation();
}
