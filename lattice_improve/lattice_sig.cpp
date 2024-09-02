#include "lattice_sig.hpp"
#include <fstream>
#include <cstring>
#include <iostream>
using namespace oit::ist::nws::adhoc_routing;

void print_poly(poly &p){
    std::cout<<"["<<std::to_string(p.coeffs[0]);
    for(int i=1;i<N;i++){
        std::cout<<","<<std::to_string(p.coeffs[i]);
    }
    std::cout<<"]"<<std::endl;
}
void print_polyveck(polyveck &p){
    std::cout<<"[";
    for(int i=0;i<K;i++){
        print_poly(p.vec[i]);
        std::cout<<std::endl;
    }
    std::cout<<"]"<<std::endl;
}

void print_polyvecl(polyvecl &p){
    std::cout<<"[";
    for(int i=0;i<L;i++){
        print_poly(p.vec[i]);
        std::cout<<std::endl;
    }
    std::cout<<"]"<<std::endl;
}
void print_keccak_state(keccak_state &k){
    std::cout<<"print keccak state"<<std::endl;
    std::cout<<"pos="<<std::to_string(k.pos)<<std::endl;
    std::cout<<"s=["<<std::to_string(k.s[0]);
    for(int i=1;i<25;i++){
        std::cout<<","<<std::to_string(k.s[i]);
    }
    std::cout<<"]"<<std::endl;
}
void print_bytearray(std::string name,std::uint8_t *a, int length){
    std::cout<<name<<"=["<<std::to_string(a[0]);
    for(int i=1;i<length;i++){
        std::cout<<","<<std::to_string(a[i]);
    }
    std::cout<<"]"<<std::endl;
}
void lattice_sig::initialize_signature(lattice_signature &sig){
    for(int i=0;i<SEEDBYTES;i++){
        sig.c[i]=0;
    }
    this->pop.polyveck_initialize(sig.w);
    this->pop.polyvecl_initialize(sig.z);
}
void lattice_sig::serialize_pkid(uint8_t* buf, public_key_id &pk_id){
    for(int i=0;i<SEEDBYTES;i++){
        buf[i]=this->pkid.rho[i];
    }
    int index=SEEDBYTES;
    for(int i=0;i<K;i++){
        for(int j=0;j<N;j++){
            buf[index+j*3+0]=pk_id.tid.vec[i].coeffs[j];
            buf[index+j*3+1]=pk_id.tid.vec[i].coeffs[j]>>8;
            buf[index+j*3+2]=pk_id.tid.vec[i].coeffs[j]>>16;
        }
        index+=POLY_MODQ_LENGTH;
    }
}
void lattice_sig::serialize_poly_modQ(int index, std::vector<std::uint8_t> &buf, const poly &p){
    for(int i=0;i<N;i++){
        buf[index+i*3+0]=p.coeffs[i];
        buf[index+i*3+1]=p.coeffs[i]>>8;
        buf[index+i*3+2]=p.coeffs[i]>>16;
    }
}
void lattice_sig::deserialize_poly_modQ(int index, const std::vector<std::uint8_t> &buf, poly &p){
    for(int i=0;i<N;i++){
		p.coeffs[i]=buf[index+i*3+0];
		p.coeffs[i]|=((int32_t)buf[index+i*3+1])<<8;
		p.coeffs[i]|=(((int32_t)buf[index+i*3+2])<<24)>>8;
		//printf("unpack[%x %x %x]=coeffs[%d] %x\n",pack[i*3+0],pack[i*3+1],pack[i*3+2],i,p->coeffs[i]);
	}
}
void lattice_sig::serialize_polyveck(int index, std::vector<std::uint8_t> &buf, const polyveck &p){
    for(int i=0;i<K;i++){
        this->serialize_poly_modQ(index+i*POLY_MODQ_LENGTH,buf,p.vec[i]);
    }
}
void lattice_sig::deserialize_polyveck(int index, const std::vector<std::uint8_t> &buf, polyveck &p){
    for(int i=0;i<K;i++){
        this->deserialize_poly_modQ(index+i*POLY_MODQ_LENGTH,buf,p.vec[i]);
    }
}
void lattice_sig::serialize_polyvecl(int index, std::vector<std::uint8_t> &buf, const polyvecl &p){
     for(int i=0;i<L;i++){
        this->serialize_poly_modQ(index+i*POLY_MODQ_LENGTH,buf,p.vec[i]);
    }
}
void lattice_sig::deserialize_polyvecl(int index, const std::vector<std::uint8_t> &buf, polyvecl &p){
    for(int i=0;i<L;i++){
        this->deserialize_poly_modQ(index+i*POLY_MODQ_LENGTH,buf,p.vec[i]);
    }
}
void lattice_sig::serialize_signature(std::vector<std::uint8_t> &buf, const lattice_signature &sig){
    std::copy(std::begin(sig.c),std::end(sig.c),buf.begin());
    this->serialize_polyvecl(INDEX_SIG_Z,buf,sig.z);
    this->serialize_polyveck(INDEX_SIG_W,buf,sig.w);
}
void lattice_sig::deserialize_signature(const std::vector<std::uint8_t> &buf, lattice_signature &sig){
    std::copy(buf.begin(),buf.begin()+SEEDBYTES,sig.c);
    this->deserialize_polyvecl(INDEX_SIG_Z,buf,sig.z);
    this->deserialize_polyveck(INDEX_SIG_W,buf,sig.w);
}
uint64_t lattice_sig::invmod (uint64_t a, uint64_t p)
{
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
void lattice_sig::setup(){
    std::ifstream ifs_mpk("lattice_mpk.key", std::ios::binary);
    std::ifstream ifs_msk("lattice_msk.key", std::ios::binary);
    
    uint8_t mpk_buf[CRYPTO_PUBLICKEYBYTES];
    uint8_t msk_buf[CRYPTO_SECRETKEYBYTES];
    ifs_mpk.read((char*)(mpk_buf),CRYPTO_PUBLICKEYBYTES);
    ifs_msk.read((char*)(msk_buf),CRYPTO_SECRETKEYBYTES);
    unpack_pk(this->mpk.rho,&(this->mpk.t1),mpk_buf);
    unpack_sk(this->msk.rho,this->msk.tr,this->msk.key,&(this->msk.t0),&(this->msk.s1),&(this->msk.s2),msk_buf);
}
int32_t lattice_sig::hash_id(std::array<std::uint8_t,ADDR_SIZE> &idarray){
	uint8_t idh[4];
    uint8_t idtmp[ADDR_SIZE];
    std::copy(idarray.begin(),idarray.end(),idtmp);
	shake256(idh,4,idtmp,ADDR_SIZE);
	int32_t* idhash_value=(int32_t*)idh;
	return caddq(*idhash_value%Q);
}
void lattice_sig::generate_tid(polyveck &tid,int32_t idhash,polyveck &t){
    for(int i=0;i<K;i++){
        for(int j=0;j<N;j++){
            tid.vec[i].coeffs[j]=(((int64_t)(t.vec[i].coeffs[j]))*idhash)%Q;
        }
    }
}
void lattice_sig::generate_pkid(){
    for(int i=0;i<SEEDBYTES;i++){
        this->pkid.rho[i]=this->mpk.rho[i];
    }
    polyvecl mat[K];
    polyvec_matrix_expand(mat, this->mpk.rho);
	int32_t idhash_value=hash_id(this->id);
    
	pop.polyvec_pointwise(this->pkid.t,mat,msk.s1);
    pop.polyveck_add_mod(this->pkid.t,this->pkid.t,this->msk.s2,Q);
    polyveck_caddq(&(this->pkid.t));
    ///t = As1+s2
    this->generate_tid(this->pkid.tid,idhash_value,this->pkid.t);
    //pkid.tid=idhash*t
}
void lattice_sig::generate_skid(){
    for(int i=0;i<SEEDBYTES;i++){
        this->skid.rho[i]=this->msk.rho[i];
        this->skid.key[i]=this->msk.key[i];
    }
    uint8_t idhash[CRHBYTES];
    uint8_t idtmp[ADDR_SIZE];
    polyveck idh2;
    polyveck tid;
    std::copy(this->id.begin(),this->id.end(),idtmp);
    shake256(idhash,CRHBYTES,idtmp,ADDR_SIZE);

    uint8_t pkid_bytearray[PUBLIC_KEY_ID_BYTES];
    this->serialize_pkid(pkid_bytearray,this->pkid);
    shake256(this->skid.trid, SEEDBYTES, pkid_bytearray, PUBLIC_KEY_ID_BYTES);
    polyveck_uniform_eta(&idh2, idhash, L);
    for(int i=0;i<K;i++){
        for(int j=0;j<N;j++){
            this->skid.s2id.vec[i].coeffs[j]=(((int64_t)(this->msk.s2.vec[i].coeffs[j]))*idh2.vec[i].coeffs[j])%Q;
        }
    }
    //tid-s2id=As1id
    this->pop.polyveck_sub_mod(tid,this->pkid.tid,this->skid.s2id,Q);
    polyvecl mat[K];
    polyvec_matrix_expand(mat, this->mpk.rho);
    matrix44 m44;
    matrix44 adj44;
    
    int32_t b[4];
    int32_t inv;
	for(int i=0;i<N;i++){
		for(int j=0;j<16;j++){
            m44.put((j/4)+1,(j%4)+1,mat[j/4].vec[j%4].coeffs[i]);
		}
		for(int j=0;j<K;j++){
			b[j]=caddq(tid.vec[j].coeffs[i]);
		}
        inv=m44.det(Q);
		inv=caddq(inv);
		inv=this->invmod(inv,Q);
		m44.adjugate_matrix(adj44,Q);
		
        int32_t tmp;
        for(int r=1;r<=4;r++){
            for(int c=1;c<=4;c++){
                tmp=adj44.get(r,c);
                adj44.put(r,c,caddq(tmp));
            }
        }
        for(int j=0;j<K;j++){
            tmp=(
                (((int64_t)(adj44.get(j+1,1))*b[0])%Q)+
                (((int64_t)(adj44.get(j+1,2))*b[1])%Q)+
                (((int64_t)(adj44.get(j+1,3))*b[2])%Q)+
                (((int64_t)(adj44.get(j+1,4))*b[3])%Q))%Q;
            this->skid.s1id.vec[j].coeffs[i]=(((int64_t)tmp)*inv)%Q;
        }
	}
}
void lattice_sig::key_derivation(){
    this->generate_pkid();
    this->generate_skid();
}
void lattice_sig::sign(isdsr_packet &p){
    std::uint8_t ri_bytes[p.get_ri_length()*ADDR_SIZE];
    for(int i=0;i<p.get_ri_length();i++){
        std::copy(p.get_ri()->at(i).begin(),p.get_ri()->at(i).end(),ri_bytes+(ADDR_SIZE*i));
    }
    lattice_signature sig;
    this->initialize_signature(sig);
    uint16_t nonce = 0;
    polyvecl mat[K], y;
    polyveck w1, w0;
    poly cp;
    keccak_state state;
    uint8_t keymu[SEEDBYTES+CRHBYTES];
    uint8_t mu[CRHBYTES];
    //uint8_t *mu=keymu+SEEDBYTES;

    
    /* Compute CRH(tr, msg) */
    shake256_init(&state);
    shake256_absorb(&state, this->skid.trid, SEEDBYTES);
    shake256_absorb(&state, ri_bytes, p.get_ri_length()*ADDR_SIZE);
    shake256_finalize(&state);
    //print_keccak_state(state);
    shake256_squeeze(mu, CRHBYTES, &state);

    uint8_t rhoprime[CRHBYTES];
    std::copy(std::begin(this->msk.key),std::end(this->msk.key),keymu);
    std::copy(std::begin(mu),std::end(mu),keymu+SEEDBYTES);
    
    #ifdef DILITHIUM_RANDOMIZED_SIGNING
    randombytes(rhoprime, CRHBYTES);
    #else
    shake256(rhoprime, CRHBYTES, keymu, SEEDBYTES + CRHBYTES);
    #endif
    /* Expand matrix and transform vectors */
    //vector A=mat
    polyvec_matrix_expand(mat, this->skid.rho);
    rej:
    /* Sample intermediate vector y */
    polyvecl_uniform_gamma1(&y, rhoprime, nonce++);
    /* Matrix-vector multiplication */
    //z = y;
    this->pop.polyvec_pointwise(sig.w,mat,y);//Ay
    
    /* Decompose w and call the random oracle */
    polyveck_caddq(&(sig.w));
    polyveck_decompose(&w1, &w0, &(sig.w));

    shake256_init(&state);
    shake256_absorb(&state, mu, CRHBYTES);
    shake256_finalize(&state);
    shake256_squeeze(sig.c,SEEDBYTES, &state);

    poly_challenge(&cp, sig.c);

    /* Compute z, reject if it reveals secret */
    this->pop.polyvecl_pointwise_poly(sig.z, cp, this->skid.s1id,Q);
    this->pop.polyvecl_add_mod(sig.z, sig.z, y,Q);
    
    /* Check that subtracting cs2 does not change high bits of w and low bits
    * do not reveal secret information */
    polyveck cs2id;
    this->pop.polyveck_pointwise_poly(cs2id,cp,this->skid.s2id,Q);
    polyveck_sub(&w0,&w0,&cs2id);
    
    if(polyveck_chknorm(&w0, GAMMA2 - BETA)){
        //printf("polyveck_chknorm(&w0, GAMMA2 - BETA)\n");
        goto rej;
    }

    if(p.get_ri_length()==1){
        p.get_sig()->resize(SIG_LENGTH);
        this->serialize_signature(*(p.get_sig()),sig);
        return;
    }
    lattice_signature prev_sig;
    lattice_signature agg_sig;
    this->initialize_signature(prev_sig);
    this->initialize_signature(agg_sig);
    this->deserialize_signature(*(p.get_sig()),prev_sig);
    for(int i=0;i<SEEDBYTES;i++){
        agg_sig.c[i]=sig.c[i]+prev_sig.c[i];
    }
    this->pop.polyvecl_add_mod(agg_sig.z,sig.z,prev_sig.z,Q);
    this->pop.polyveck_add_mod(agg_sig.w,sig.w,prev_sig.w,Q);
    
    this->serialize_signature(*(p.get_sig()),agg_sig);
}
bool lattice_sig::verify(isdsr_packet &p){
    lattice_signature sig;
    this->initialize_signature(sig);
    this->deserialize_signature(*(p.get_sig()),sig);
    this->serialize_signature(*(p.get_sig()),sig);

    std::uint8_t ri_length=p.get_ri_length();
    std::uint8_t ri_bytes[ri_length*ADDR_SIZE];
    for(int i=0;i<p.get_ri_length();i++){
        std::copy(p.get_ri()->at(i).begin(),p.get_ri()->at(i).end(),ri_bytes+(ADDR_SIZE*i));
    }
    
    polyveck Az;
    polyvecl mat[K];
	polyveck ctid,ct_tmp;
    polyveck Az_ctid;
    polyvec_matrix_expand(mat, this->pkid.rho);
    this->pop.polyvec_pointwise(Az,mat,sig.z);
	//polyveck_caddq(&Az);
    this->pop.polyveck_initialize(ctid);

    public_key_id pkid_tmp;
    for(int i=0;i<SEEDBYTES;i++){
        pkid_tmp.rho[i]=this->mpk.rho[i];
    }
    this->pop.copy_polyveck(pkid_tmp.t,this->pkid.t);
    std::array<std::uint8_t,ADDR_SIZE> id_tmp;
    std::int32_t idhash_value;
    std::uint8_t pkid_bytearray[PUBLIC_KEY_ID_BYTES];
    std::uint8_t trid[SEEDBYTES];
    std::uint8_t mu[CRHBYTES];
    std::uint8_t cid[SEEDBYTES]={0};
    std::uint8_t c_tmp[SEEDBYTES];
	keccak_state state;
    poly pc;
    for(int i=1;i<=ri_length;i++){
        id_tmp[0]=ri_bytes[(i-1)*ADDR_SIZE+0];
		id_tmp[1]=ri_bytes[(i-1)*ADDR_SIZE+1];
		id_tmp[2]=ri_bytes[(i-1)*ADDR_SIZE+2];
		id_tmp[3]=ri_bytes[(i-1)*ADDR_SIZE+3];
        idhash_value=hash_id(id_tmp);
        this->generate_tid(pkid_tmp.tid,idhash_value,pkid_tmp.t);
        this->serialize_pkid(pkid_bytearray,pkid_tmp);
		// tr = CRH(rho || t)
		shake256(trid, SEEDBYTES, pkid_bytearray, PUBLIC_KEY_ID_BYTES);
        shake256_init(&state);
		shake256_absorb(&state, trid, SEEDBYTES);
		shake256_absorb(&state, ri_bytes, i*ADDR_SIZE);
		shake256_finalize(&state);
        //mu = CRH(tr||M)
		shake256_squeeze(mu, CRHBYTES, &state);

		shake256_init(&state);
		shake256_absorb(&state, mu, CRHBYTES);
		shake256_finalize(&state);
		//c = CRH(mu||w1)
		shake256_squeeze(c_tmp, SEEDBYTES, &state);

        for(int j=0;j<SEEDBYTES;j++){
			//printf("cid[%d]=%d,c_tmp[%d]=%d ",j,cid[j],j,c_tmp[j]);
			cid[j]=(cid[j]+c_tmp[j])%Q;
		}
        poly_challenge(&pc, c_tmp);
        this->pop.polyveck_pointwise_poly(ct_tmp,pc,pkid_tmp.tid,Q);
        this->pop.polyveck_add_mod(ctid,ctid,ct_tmp,Q);
    }
    for(int i=0;i<SEEDBYTES;i++){
		//printf("c_agg[%d]=%d,cid[%d]=%d ",i,c_agg[i],i,cid[i]);
		if(sig.c[i]!=cid[i]){
			printf("c[%d] is different\n",i);
			return 0;
		}
	}
    //Az = tc+w
    this->pop.polyveck_sub_mod(Az_ctid,Az,ctid,Q);
    polyveck_caddq(&Az_ctid);
    polyveck Az_ctid1, Az_ctid0;
    polyveck sigw1, sigw0;
    polyveck_decompose(&Az_ctid1, &Az_ctid0, &Az_ctid);
    //polyveck_caddq(&(sig.w));
    polyveck_decompose(&sigw1, &sigw0, &(sig.w));
    return this->pop.compare_polyveck(Az_ctid1,sigw1);
}