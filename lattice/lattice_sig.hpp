#ifndef LATTICE_SIG
#define LATTICE_SIG

#include <array>

#include "../isdsr/signature_scheme.hpp"

#ifdef __cplusplus
extern "C"{
#endif

#include <fips202.h>
#include <packing.h>
#include <reduce.h>
#ifdef __cplusplus
}
#endif
#include "matrix_cpp.hpp"
#include "packing_agg_cpp.hpp"
#include "sign_agg_cpp.hpp"

namespace oit::ist::nws::adhoc_routing{

class lattice_sig:public signature_scheme{


    protected:
    uint8_t mpk[CRYPTO_PUBLICKEYBYTES];
    uint8_t msk[CRYPTO_SECRETKEYBYTES];
    uint8_t pk_agg[PUBLIC_KEY_AGG_BYTES];
    uint8_t skid[SECRET_KEY_AGG_BYTES];
    sign_agg signagg;
    public:
    lattice_sig(){}
    ~lattice_sig(){}
    virtual void setup();
    virtual void key_derivation();
    virtual void sign(isdsr_packet &pkt);
    virtual bool verify(isdsr_packet &pkt);
    virtual string signature_scheme_name(){return " lattice";}
};
};
#endif