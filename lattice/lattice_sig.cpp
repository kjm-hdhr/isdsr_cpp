#include "lattice_sig.hpp"
#include "polyvec.hpp"
#include "matrix_calc.hpp"
#include <fstream>
#include <cstring>
using namespace oit::ist::nws::adhoc_routing;

uint32_t lattice_sig::aggregate_sig_length(const aggregate_sig &agg_sig){
    return SEEDBYTES+POLYVECL_MODQ_SIZE+1+POLYVECK_MODQ_SIZE*agg_sig.w.size();
}
void lattice_sig::generate_idpk_array(std::array<uint8_t,ADDR_SIZE> &target_id, polyveck &tid,std::vector<uint8_t> &buf){
    int32_t idhash_value=0;
    idhash_value=this->hash_id(target_id);
    this->pvo.polyveck_multiply(tid,idhash_value,this->idpk.t,Q);
    buf.resize(ID_PUB_KEY_SIZE);
    std::copy(this->idpk.rho,this->idpk.rho+RHO_SIZE,buf.begin());
    this->pvo.serialize_polyveck_modQ(tid,buf,RHO_SIZE);
}
uint64_t lattice_sig::invmod (uint64_t a, uint64_t p){
    uint64_t j = 1, i = 0, b = p, c = a, x, y;
    while (c != 0)
    {
        x = b / c;
        y = b - x * c;
        b = c; 
        c = y;
        y = j;
        j = i - j * x;
        i = y;
    }
    if ((int64_t)i < 0)
        i += p;
    return i;
}
int32_t lattice_sig::multiply(int32_t a, int32_t b, int p){
    int64_t ret=a;
	ret=(ret*b)%p;
	return ret;
}
void lattice_sig::setup(){
    std::ifstream ifs_mpk("lattice_mpk.key", std::ios::binary);
    std::ifstream ifs_msk("lattice_msk.key", std::ios::binary);
    
    uint8_t mpk_buf[CRYPTO_PUBLICKEYBYTES];
    uint8_t msk_buf[CRYPTO_SECRETKEYBYTES];
    ifs_mpk.read(((char*)mpk_buf),CRYPTO_PUBLICKEYBYTES);
    ifs_msk.read(((char*)msk_buf),CRYPTO_SECRETKEYBYTES);

    unpack_sk(this->mpk.rho,msk.tr,msk.key,&(msk.t0),&(msk.s1),&(msk.s2),msk_buf);
    unpack_pk(this->mpk.rho,&(mpk.t1),mpk_buf);
}
void lattice_sig::key_derivation(){
    polyvecl msk_mat[K];
    polyvec_matrix_expand(msk_mat, this->mpk.rho);//msk_mat = A
    int32_t idhash_value=hash_id(this->id);
    uint8_t id_array[this->id.size()];
    std::copy(this->id.begin(),this->id.end(),id_array);
    uint8_t idhash[CRHBYTES];
    shake256(idhash,CRHBYTES,id_array,SINGLE_ID_LENGTH);

    polyveck t;
    this->pvo.polyvec_pointwise(t,msk_mat,this->msk.s1,Q);
    this->pvo.polyveck_add_mod(t, t, this->msk.s2,Q);
    polyveck_caddq(&t);
    //t=As1+s2
    //id pub key t and rho
    this->pvo.copy_polyveck(this->idpk.t,t);
    std::copy(this->mpk.rho,this->mpk.rho+RHO_SIZE,this->idpk.rho);

    //id sec key tid
    vector<uint8_t> idpk_array_vector(ID_PUB_KEY_SIZE);
    this->generate_idpk_array(this->id,this->idsk.tid,idpk_array_vector);
    uint8_t idpk_array[ID_PUB_KEY_SIZE];
    //id sec key trid
	shake256(this->idsk.trid, SEEDBYTES, idpk_array, ID_PUB_KEY_SIZE);
    //id sec key rhoid
    std::copy(this->msk.rho, this->msk.rho+RHO_SIZE,this->idsk.rhoid);
    //id sec key keyid
    std::copy(this->msk.key, this->msk.key+KEY_SIZE,this->idsk.keyid);


    //pkid_pack(pkid,rho,&pkt);
    polyveck idh2;
    polyveck_uniform_eta(&idh2, idhash, L);
    this->pvo.polyveck_pointwise(idh2,idh2,msk.s2,Q);
    this->pvo.polyveck_sub_mod(t,t,this->idsk.s2id,Q);
    //id sec key s1id
    this->generate_s1id(this->idsk.s1id, msk_mat, t);
	
	//pkid_pack(pkid,msk_rho,&pkt);
	//skid_pack(skid, rho, trid, key, &tid, &s1id, &s2id);
}
void lattice_sig::sign(isdsr_packet &p){
    single_sig s_sig;
    this->sign_single_signature(p,s_sig);
    this->sign_agg_signature(p,s_sig);
}
bool lattice_sig::verify(isdsr_packet &p){
    
    aggregate_sig agg_sig;
    this->deserialize_aggregate_sig(agg_sig,*(p.get_sig()));

    polyvecl mat[K];
    polyveck Az;
    polyvec_matrix_expand(mat, this->idpk.rho);//rho=A
    this->pvo.polyvec_pointwise(Az,mat,agg_sig.z,Q);
    polyveck_caddq(&Az);

    int32_t idhash_value=0;
    vector<uint8_t> id_array_vector(ID_PUB_KEY_SIZE);
    uint8_t ri_array[p.get_ri_length()*ADDR_SIZE];
    for(int i=0;i<p.get_ri_length();i++){
        std::copy(p.get_ri()->at(i).begin(),p.get_ri()->at(i).end(),ri_array+(i+ADDR_SIZE));
    }
    uint8_t id_array[ID_PUB_KEY_SIZE];
    uint8_t trid[TR_SIZE];
    uint8_t mu[CRHBYTES];
    polyveck w1,w0,wid,wsum;
    uint8_t w1_pack[K*POLYW1_PACKEDBYTES];
    uint8_t cid[SEEDBYTES]={0},c_tmp[SEEDBYTES];
    polyveck ctid,ct_tmp,tid;
    poly pc;
    keccak_state state;
    this->pvo.polyveck_initialize(wsum);
    this->pvo.polyveck_initialize(ctid);

    for(int i=0;i<p.get_ri_length();i++){
        idhash_value=this->hash_id(p.get_ri()->at(i));
        this->generate_idpk_array(p.get_ri()->at(i),tid,id_array_vector);
        std::copy(id_array_vector.begin(),id_array_vector.end(),id_array);

		// tr = CRH(rho || t)
		shake256(trid, SEEDBYTES, id_array, ID_PUB_KEY_SIZE);

		shake256_init(&state);
        shake256_absorb(&state, trid, TR_SIZE);
        shake256_absorb(&state, ri_array, (i+1)*ADDR_SIZE);
        shake256_finalize(&state);
        //mu = CRH(tr||M)
        shake256_squeeze(mu, CRHBYTES, &state);
        polyveck_decompose(&w1, &w0, &(agg_sig.w[i]));
        polyveck_pack_w1(w1_pack, &w1);

        shake256_init(&state);
        shake256_absorb(&state, mu, CRHBYTES);
        shake256_absorb(&state, w1_pack, K*POLYW1_PACKEDBYTES);
        shake256_finalize(&state);
        //c = CRH(mu||w1)
        shake256_squeeze(c_tmp, SEEDBYTES, &state);
        for(int j=0;j<SEEDBYTES;j++){
			cid[j]=(cid[j]+c_tmp[j])%Q;
		}
		//ct
		poly_challenge(&pc, c_tmp);
        this->pvo.polyveck_add_mod(wsum,wsum,agg_sig.w[i],Q);
        this->pvo.polyveck_pointwise_poly(ct_tmp,pc,tid,Q);
        this->pvo.polyveck_add_mod(ctid,ctid,ct_tmp,Q);
    }
    //Az = tc+w
    this->pvo.polyveck_sub_mod(wid,Az,ctid,Q);
	//polyveck_sub_mod(&wid,&Az,&ctid,Q);
	polyveck_caddq(&wid);
	
	polyveck wid1, wid0;
	polyveck wsum1, wsum0;
	polyveck_decompose(&wid1, &wid0, &wid);
	polyveck_decompose(&wsum1, &wsum0, &wsum);
	return this->pvo.compare_polyveck(wsum1,wid1);

}
void lattice_sig::serialize_aggregate_sig(const aggregate_sig &agg_sig, vector<uint8_t> &buf){
    uint8_t len=(uint8_t)agg_sig.w.size();
    if(buf.size()<this->aggregate_sig_length(agg_sig)){
        buf.resize(this->aggregate_sig_length(agg_sig));
    }
    std::copy(agg_sig.c,agg_sig.c+SEEDBYTES,buf.begin());
    this->pvo.serialize_polyvecl_modQ(agg_sig.z,buf,INDEX_AGG_SIG_Z);
    buf[INDEX_AGG_SIG_WLENGTH]=len;
    for(int i=0;i<agg_sig.w.size();i++){
        this->pvo.serialize_polyveck(agg_sig.w.at(i),buf,INDEX_AGG_SIG_W+(i*POLYVECK_MODQ_SIZE));
    }
}
void lattice_sig::deserialize_aggregate_sig(aggregate_sig &agg_sig, const vector<uint8_t> &buf){
    std::copy(buf.begin(),buf.begin()+SEEDBYTES,agg_sig.c);
    this->pvo.deserialize_polyvecl(agg_sig.z,buf,INDEX_AGG_SIG_Z);
    agg_sig.wlength=buf[INDEX_AGG_SIG_WLENGTH];
    agg_sig.w.resize(agg_sig.wlength);
    for(int i=0;i<agg_sig.wlength;i++){
        this->pvo.deserialize_polyveck(agg_sig.w.at(i),buf,INDEX_AGG_SIG_W+(i*POLYVECK_MODQ_SIZE));
    }
}
int32_t lattice_sig::hash_id(array<uint8_t,ADDR_SIZE> &id_array){
	uint8_t idh[4];
    uint8_t tmp_id[id_array.size()];
    std::copy(id_array.begin(),id_array.end(),tmp_id);
	shake256(idh,4,tmp_id,SINGLE_ID_LENGTH);
	int32_t* idhash_value=(int32_t*)idh;
	return caddq((*idhash_value)%Q);
}
void lattice_sig::generate_s1id(polyvecl &s1id, polyvecl mat[K], polyveck &t){

    int32_t b[4];
    int32_t inv;
	int i,j;
    mat44_calc mat44c;
    array<array<int32_t,4>,4> adj_mat;
    int32_t tmp=0;
	for(i=0;i<N;i++){
        for(int r=0;r<4;r++){
            for(int c=0;c<4;c++){
                mat44c.set(r,c,mat[r].vec[c].coeffs[i]);
            }
        }
        for(j=0;j<K;j++){
            b[j]=caddq(t.vec[j].coeffs[i]);
        }
        inv=mat44c.get_det(Q);
        //inv=mat44_det(mat44,Q);
        inv=caddq(inv);
        inv=this->invmod(inv,Q);
        mat44c.make_adjugate_mat44(Q,adj_mat);
        //make_adjugate_mat44(mat44_t,mat44,Q);
        for(int r=0;r<4;r++){
            for(int c=0;c<4;c++){
                adj_mat[r][c]=caddq(adj_mat[r][c]);
            }
        }
        for(int r=0;r<4;r++){
            for(int c=0;c<4;c++){
                tmp=(tmp+this->multiply(adj_mat[r][c],b[c],Q))%Q;
            }
            s1id.vec[j].coeffs[i]=multiply(tmp,inv,Q);
            tmp=0;
        }
    }
}
void lattice_sig::sign_single_signature(isdsr_packet &p, single_sig &s_sig){
    uint8_t c[SEEDBYTES];
    uint16_t nonce = 0;
    polyvecl mat[K], y;
    polyveck t, w1, w0, w;
    poly cp;
    uint32_t rilength=p.get_ri_length()*ADDR_SIZE;
    uint8_t ri[rilength];
    for(int i=0;i<p.get_ri_length();i++){
        std::copy(p.get_ri()->at(i).begin(),p.get_ri()->at(i).end(),ri+(ADDR_SIZE*i));
    }

    keccak_state state;
    uint8_t mu[CRHBYTES];
    /* Compute CRH(tr, msg) */
    shake256_init(&state);
    shake256_absorb(&state, this->idsk.trid, SEEDBYTES);
    //shake256_absorb(&state, tr, SEEDBYTES);
    shake256_absorb(&state, ri, rilength);
    shake256_finalize(&state);
    shake256_squeeze(mu, CRHBYTES, &state);

    uint8_t keymu[SEEDBYTES+CRHBYTES];
    std::copy(this->idsk.keyid,this->idsk.keyid+SEEDBYTES,keymu);
    std::copy(mu,mu+CRHBYTES,keymu+SEEDBYTES);
    uint8_t rhoprime[CRHBYTES];

#ifdef DILITHIUM_RANDOMIZED_SIGNING
    randombytes(rhoprime, CRHBYTES);
#else
    shake256(rhoprime, CRHBYTES, keymu, SEEDBYTES + CRHBYTES);
#endif
    
    polyvec_matrix_expand(mat, this->idsk.rhoid);
    rej:
    /* Sample intermediate vector y */
    polyvecl_uniform_gamma1(&y, rhoprime, nonce++);
	//polyvecl_uniform_eta(&y, rhoprime, nonce++);
    /* Matrix-vector multiplication */
    this->pvo.copy_polyvecl(s_sig.z,y);
    //z = y;
    //polyvecl_ntt(&z);
    this->pvo.polyvec_pointwise(w,mat,y,Q);//Ay
	//polyvec_pointwise(&w,mat,&y);//Ay
    //polyvec_matrix_pointwise_montgomery(&w1, mat, &z);
    //polyveck_reduce(&w1);
    //polyveck_invntt_tomont(&w1);

    /* Decompose w and call the random oracle */
    //polyveck_caddq(&w1);
    polyveck_caddq(&w);
    this->pvo.copy_polyveck(s_sig.w,w);
    //polyveck_decompose(&w1, &w0, &w1);
    //polyveck_pack_w1(sig, &w1);

    polyveck_decompose(&w1, &w0, &w);
    uint8_t packw1[K*POLYW1_PACKEDBYTES];
    polyveck_pack_w1(packw1, &w1);

    shake256_init(&state);
    shake256_absorb(&state, mu, CRHBYTES);
    shake256_absorb(&state, packw1, K*POLYW1_PACKEDBYTES);
    shake256_finalize(&state);
    shake256_squeeze(s_sig.c, SEEDBYTES, &state);
    //c_pack(c,sig);

    //poly_challenge(&cp, sig);
    poly_challenge(&cp, s_sig.c);
    //poly_ntt(&cp);

    /* Compute z, reject if it reveals secret */
    this->pvo.polyvecl_pointwise_poly(s_sig.z,cp,this->idsk.s1id,Q);
    //polyvecl_pointwise_poly(&z, &cp, &s1id,Q);
    //polyvecl_pointwise_poly_montgomery(&z, &cp, &s1);
    //polyvecl_invntt_tomont(&z);
    this->pvo.polyvecl_add_mod(s_sig.z,s_sig.z,y,Q);
    //polyvecl_add_mod(&z, &z, &y,Q);

    //polyvecl_reduce(&z);

    //if(polyvecl_chknorm(&z, GAMMA1 - BETA)){
    //	printf("polyvecl_chknorm(&z, GAMMA1 - BETA) %d\n",cnt++);
    //  goto rej;
    //}

    /* Check that subtracting cs2 does not change high bits of w and low bits
    * do not reveal secret information */
    //polyveck_pointwise_poly_montgomery(&h, &cp, &s2);
    //polyveck_invntt_tomont(&h);
    //polyveck_sub(&w0, &w0, &h);
    //polyveck_reduce(&w0);
    polyveck cs2id;
    this->pvo.polyveck_pointwise_poly(cs2id,cp,this->idsk.s2id,Q);
    //polyveck_pointwise_poly(&cs2id,&cp,&s2id,Q);
    polyveck_sub(&w0,&w0,&cs2id);

    //polyveck_pointwise_poly(&h, &cp, &s2id,Q);
    //polyveck_sub(&w0, &w0, &h);

    if(polyveck_chknorm(&w0, GAMMA2 - BETA)){
    //printf("polyveck_chknorm(&w0, GAMMA2 - BETA)\n");
        goto rej;
    }
    //sig_pack(sig,c,&z,&w);
    //*siglen=SEEDBYTES+POLY_MODQ_LENGTH*L+POLY_MODQ_LENGTH*K;
    //return 0;
}
void lattice_sig::sign_agg_signature(isdsr_packet &p, single_sig &s_sig){
/*(
	uint8_t *agg_sig,
	uint8_t *prev_agg_sig,
	uint8_t *sig,
	uint8_t prev_agg_sig_wlen){
*/	
    aggregate_sig agg_sig;
    if(p.get_ri_length()<2){
        for(int i=0;i<SEEDBYTES;i++){
            agg_sig.c[i]=0;
        }
        this->pvo.polyvecl_initialize(agg_sig.z);
        agg_sig.w.clear();
        agg_sig.wlength=0;
    }
    else{
        this->deserialize_aggregate_sig(agg_sig,*(p.get_sig()));
    }
    for(int i=0;i<SEEDBYTES;i++){
		agg_sig.c[i]=(agg_sig.c[i]+s_sig.c[i])%Q;
	}
    this->pvo.polyvecl_add_mod(agg_sig.z,agg_sig.z,s_sig.z,Q);
    agg_sig.w.push_back(s_sig.w);
    agg_sig.wlength=agg_sig.w.size();
    p.get_sig()->resize(this->aggregate_sig_length(agg_sig));
    this->serialize_aggregate_sig(agg_sig,*(p.get_sig()));
}

