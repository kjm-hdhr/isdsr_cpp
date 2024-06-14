#ifndef LATTICE_SIG_HPP
#define LATTICE_SIG_HPP

#include <array>

#include "../isdsr/signature_scheme.hpp"
#include "polyvec.hpp"


namespace oit::ist::nws::adhoc_routing{

#define INDEX_AGG_SIG_C 0
#define INDEX_AGG_SIG_Z INDEX_AGG_SIG_C+SEEDBYTES
#define INDEX_AGG_SIG_WLENGTH INDEX_AGG_SIG_Z+POLYVECL_MODQ_SIZE
#define INDEX_AGG_SIG_W INDEX_AGG_SIG_WLENGTH+1

#define RHO_SIZE SEEDBYTES
#define TR_SIZE SEEDBYTES
#define KEY_SIZE SEEDBYTES

#define ID_PUB_KEY_SIZE RHO_SIZE+POLYVECK_MODQ_SIZE



typedef struct {
    std::uint8_t c[SEEDBYTES];
	polyvecl z;
	polyveck w;
} single_sig;

typedef struct {
    std::uint8_t c[SEEDBYTES];
	polyvecl z;
    vector<polyveck> w;
    std::uint8_t wlength;
} aggregate_sig;

typedef struct {
    std::uint8_t rhoid[RHO_SIZE];
	std::uint8_t trid[TR_SIZE];
	std::uint8_t keyid[KEY_SIZE];
    polyveck tid;
    polyvecl s1id;
    polyveck s2id;
} id_sec_key;

typedef struct public_key_id{
    std::uint8_t rho[RHO_SIZE];
    polyveck t;
} id_pub_key;

typedef struct {
    std::uint8_t rho[RHO_SIZE];
    polyveck t1;
} master_pub_key;

typedef struct {
    std::uint8_t rho[RHO_SIZE];
    std::uint8_t tr[TR_SIZE];
    std::uint8_t key[KEY_SIZE];
    polyvecl s1;
    polyveck s2;
    polyveck t0;
} master_sec_key;

class lattice_sig:public signature_scheme{


    protected:
    poly_operation po;
    polyvec_operation pvo;
    id_pub_key idpk;
    id_sec_key idsk;
    master_pub_key mpk;
    master_sec_key msk;
    single_sig ssig;
    aggregate_sig aggsig;

    uint64_t invmod (uint64_t a, uint64_t p);
    int32_t multiply(int32_t a, int32_t b, int p);
    void generate_s1id(polyvecl &s1id, polyvecl mat[K], polyveck &t);
    void sign_single_signature(isdsr_packet &pkt);
    void sign_agg_signature(isdsr_packet &pkt);
    void serialize_aggregate_sig(vector<std::uint8_t> &buf);
    void deserialize_aggregate_sig(const vector<std::uint8_t> &buf);
    std::uint32_t aggregate_sig_length();
    void generate_idpk_array(std::array<std::uint8_t,ADDR_SIZE> &target_id, polyveck &tid, std::vector<std::uint8_t> &buf);
    void idpk_byte_array(std::array<std::uint8_t,ADDR_SIZE> &target_id, polyveck &tid,std::uint8_t *byte_array);
    public:
    lattice_sig(){};
    ~lattice_sig(){};
    virtual void setup();
    virtual void key_derivation();
    virtual void sign(isdsr_packet &pkt);
    virtual bool verify(isdsr_packet &pkt);
    int32_t hash_id(array<std::uint8_t,ADDR_SIZE> &id_array);
    void generate_keypair_from_own_id();
    virtual string signature_scheme_name(){return " lattice";}
};
};
#endif