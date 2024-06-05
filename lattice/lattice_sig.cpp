#include "lattice_sig.hpp"

#include <fstream>

#include "packing_agg.h"
using namespace oit::ist::nws::adhoc_routing;

void lattice_sig::setup(){
    std::ifstream ifs_mpk("lattice_mpk.key", std::ios::binary);
    std::ifstream ifs_msk("lattice_msk.key", std::ios::binary);
    
    uint8_t mpk_buf[CRYPTO_PUBLICKEYBYTES];
    uint8_t msk_buf[CRYPTO_SECRETKEYBYTES];
    ifs_mpk.read((char*)(this->mpk),CRYPTO_PUBLICKEYBYTES);
    ifs_msk.read((char*)(this->msk),CRYPTO_SECRETKEYBYTES);

}
void lattice_sig::key_derivation(){
    std::memset(pk_agg,0,PUBLIC_KEY_AGG_BYTES);
    std::memset(skid,0,SECRET_KEY_AGG_BYTES);
    uint8_t own_id[ADDR_SIZE];
    std::copy(this->id.begin(),this->id.end(),own_id);
    crypto_sign_keypair_skid(pk_agg, skid,own_id,msk);
}
void lattice_sig::sign(isdsr_packet &p){
    uint8_t sig[EACH_SIG_LENGTH];
	size_t siglen;
    uint8_t ri_bytes[p.get_ri_length()*ADDR_SIZE];
    for(int i=0;i<p.get_ri_length();i++){
        std::copy(p.get_ri()->at(i).begin(),p.get_ri()->at(i).end(),ri_bytes+(ADDR_SIZE*i));
    }
    crypto_sign_signatre_skid(
    sig, &siglen,ri_bytes,p.get_ri_length()*ADDR_SIZE,skid);

	//uint8_t *prev_aggsig=p->sig;
    uint8_t prev_aggsig[p.get_sig()->size()];
    std::copy(p.get_sig()->begin(),p.get_sig()->end(),prev_aggsig);
    int agg_sig_length=SEEDBYTES+POLY_MODQ_LENGTH*L+POLY_MODQ_LENGTH*K*p.get_ri_length()+1;
    uint8_t agg_sig[agg_sig_length];
    crypto_sign_signature_agg(agg_sig,prev_aggsig,sig,p.get_ri_length()-1);
    
    p.get_sig()->resize(agg_sig_length,0);
    std::copy(agg_sig,agg_sig+agg_sig_length,p.get_sig()->begin());
}
bool lattice_sig::verify(isdsr_packet &p){
    uint8_t sig[p.get_sig_length()];
    std::copy(p.get_sig()->begin(),p.get_sig()->end(),sig);
    uint8_t ri_bytes[p.get_ri_length()*ADDR_SIZE];
    for(int i=0;i<p.get_ri_length();i++){
        std::copy(p.get_ri()->at(i).begin(),p.get_ri()->at(i).end(),ri_bytes+(ADDR_SIZE*i));
    }
    int v=crypto_sign_verify_agg(pk_agg,sig,ri_bytes,p.get_ri_length());
	return v;
}