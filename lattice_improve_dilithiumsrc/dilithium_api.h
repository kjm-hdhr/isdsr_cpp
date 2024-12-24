#ifndef DILITHIUM_API_H
#define DILITHIUM_API_H

#include "fips202.h"
#include "polyvec.h"

void pqcrystals_dilithium2_ref_unpack_sk(uint8_t rho[SEEDBYTES],
               uint8_t tr[SEEDBYTES],
               uint8_t key[SEEDBYTES],
               polyveck *t0,
               polyvecl *s1,
               polyveck *s2,
               const uint8_t sk[CRYPTO_SECRETKEYBYTES]);

void pqcrystals_dilithium2_ref_polyvec_matrix_expand(polyvecl mat[K], const uint8_t rho[SEEDBYTES]);
void pqcrystals_dilithium2_ref_polyveck_caddq(polyveck *v);
//void pqcrystals_dilithium2_ref_polyveck_uniform_eta(polyvecl *v, const uint8_t* seed, uint16_t nonce);
void pqcrystals_dilithium_fips202_ref_shake256_init(keccak_state *state);
void pqcrystals_dilithium_fips202_ref_shake256_absorb(keccak_state *state, const uint8_t *in, size_t inlen);
void pqcrystals_dilithium_fips202_ref_shake256_finalize(keccak_state *state);
void pqcrystals_dilithium_fips202_ref_shake256_squeeze(uint8_t *out, size_t outlen, keccak_state *state);

#endif