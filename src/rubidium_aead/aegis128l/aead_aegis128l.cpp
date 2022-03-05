
#include <errno.h>
#include <stdlib.h>

#include "rubidium_aead_aegis128l.h"
#include "private/common.h"
#include "randombytes.h"
namespace rubidium {
    size_t
    rubidium_aead_aegis128l_keybytes(void) {
        return rubidium_aead_aegis128l_KEYBYTES;
    }

    size_t
    rubidium_aead_aegis128l_nsecbytes(void) {
        return rubidium_aead_aegis128l_NSECBYTES;
    }

    size_t
    rubidium_aead_aegis128l_npubbytes(void) {
        return rubidium_aead_aegis128l_NPUBBYTES;
    }

    size_t
    rubidium_aead_aegis128l_abytes(void) {
        return rubidium_aead_aegis128l_ABYTES;
    }

    size_t
    rubidium_aead_aegis128l_messagebytes_max(void) {
        return rubidium_aead_aegis128l_MESSAGEBYTES_MAX;
    }

    void
    rubidium_aead_aegis128l_keygen(unsigned char k[rubidium_aead_aegis128l_KEYBYTES]) {
        randombytes_buf(k, rubidium_aead_aegis128l_KEYBYTES);
    }

#if !((defined(HAVE_TMMINTRIN_H) && defined(HAVE_WMMINTRIN_H)) || \
      defined(RUBIDIUM_HAVE_ARMNEON))

#ifndef ENOSYS
# define ENOSYS ENXIO
#endif

    int
    rubidium_aead_aegis128l_encrypt_detached(unsigned char *c, unsigned char *mac,
                                           std::size_t *maclen_p, const unsigned char *m,
                                           std::size_t mlen, const unsigned char *ad,
                                           std::size_t adlen, const unsigned char *nsec,
                                           const unsigned char *npub, const unsigned char *k) {
        errno = ENOSYS;
        return -1;
    }

    int
    rubidium_aead_aegis128l_encrypt(unsigned char *c, std::size_t *clen_p, const unsigned char *m,
                                  std::size_t mlen, const unsigned char *ad,
                                  std::size_t adlen, const unsigned char *nsec,
                                  const unsigned char *npub, const unsigned char *k) {
        errno = ENOSYS;
        return -1;
    }

    int
    rubidium_aead_aegis128l_decrypt_detached(unsigned char *m, unsigned char *nsec, const unsigned char *c,
                                           std::size_t clen, const unsigned char *mac,
                                           const unsigned char *ad, std::size_t adlen,
                                           const unsigned char *npub, const unsigned char *k) {
        errno = ENOSYS;
        return -1;
    }

    int
    rubidium_aead_aegis128l_decrypt(unsigned char *m, std::size_t *mlen_p, unsigned char *nsec,
                                  const unsigned char *c, std::size_t clen,
                                  const unsigned char *ad, std::size_t adlen,
                                  const unsigned char *npub, const unsigned char *k) {
        errno = ENOSYS;
        return -1;
    }

    int
    rubidium_aead_aegis128l_is_available(void) {
        return 0;
    }
}
#endif
