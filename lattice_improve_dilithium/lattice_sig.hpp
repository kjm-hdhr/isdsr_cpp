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
#include "matrix.hpp"
#include "polyvec_op.hpp"

namespace oit::ist::nws::adhoc_routing{

struct master_secret_key{
    uint8_t rho[SEEDBYTES];
    uint8_t tr[SEEDBYTES];
    uint8_t key[SEEDBYTES];
    polyvecl s1;
    polyveck s2;
    polyveck t0;
};
struct master_public_key{
    uint8_t rho[SEEDBYTES];
    polyveck t1;
};

struct public_key_id{
    uint8_t rho[SEEDBYTES];
    polyveck tid;
    polyveck t;
};
struct secret_key_id{
	uint8_t rho[SEEDBYTES];
	uint8_t trid[SEEDBYTES];
	uint8_t key[SEEDBYTES];
    polyvecl s1id;
    polyveck s2id;
};

struct lattice_signature{
    uint8_t c[SEEDBYTES];
	polyvecl z;
	polyveck w;
};

#define POLY_MODQ_LENGTH 256*3
#define INDEX_SIG_C 0
#define INDEX_SIG_Z SEEDBYTES
#define INDEX_SIG_W SEEDBYTES+POLY_MODQ_LENGTH*K
#define SIG_LENGTH SEEDBYTES+POLY_MODQ_LENGTH*K+POLY_MODQ_LENGTH*L
#define PUBLIC_KEY_ID_BYTES SEEDBYTES+POLY_MODQ_LENGTH*K

class lattice_sig:public signature_scheme{

    protected:
    master_public_key mpk;
    master_secret_key msk;
    public_key_id pkid;
    secret_key_id skid;
    polyvec_op pop;
    //uint8_t mpk[CRYPTO_PUBLICKEYBYTES];
    //uint8_t msk[CRYPTO_SECRETKEYBYTES];
    //uint8_t pk_agg[PUBLIC_KEY_AGG_BYTES];
    //uint8_t skid[SECRET_KEY_AGG_BYTES];
    //uint8_t pkid[PUBLIC_KEY_AGG_BYTES];
    //sign_agg signagg;
    //int32_t hash_id();
    int32_t hash_id(std::array<std::uint8_t,ADDR_SIZE> &idarray);
    void generate_tid(polyveck &tid,int32_t idhash,polyveck &t);
    void generate_pkid();
    void generate_skid();
    uint64_t invmod (uint64_t a, uint64_t p);
    void initialize_signature(lattice_signature &sig);
    void serialize_pkid(uint8_t* buf, public_key_id &pk_id);
    void serialize_poly_modQ(int index, std::vector<std::uint8_t> &buf, const poly &p);
    void deserialize_poly_modQ(int index, const std::vector<std::uint8_t> &buf, poly &p);
    void serialize_polyveck(int index, std::vector<std::uint8_t> &buf, const polyveck &p);
    void deserialize_polyveck(int index, const std::vector<std::uint8_t> &buf, polyveck &p);
    void serialize_polyvecl(int index, std::vector<std::uint8_t> &buf, const polyvecl &p);
    void deserialize_polyvecl(int index, const std::vector<std::uint8_t> &buf, polyvecl &p);
    void serialize_signature(std::vector<std::uint8_t> &buf, const lattice_signature &sig);
    void deserialize_signature(const std::vector<std::uint8_t> &buf, lattice_signature &sig);
    public:
    lattice_sig(){}
    lattice_sig(array<uint8_t, ADDR_SIZE> &id):signature_scheme(id){};
    ~lattice_sig(){}
    virtual void setup();
    virtual void key_derivation();
    virtual void sign(isdsr_packet &pkt);
    virtual bool verify(isdsr_packet &pkt);
    virtual string signature_scheme_name(){return " lattice";}
};
};
#endif