#include "ecdsa_sig.hpp"
#include <openssl/sha.h>
#include <fstream>
using namespace oit::ist::nws::adhoc_routing;

void ecdsa_sig::H1(mclBnG1 &g1, const array<uint8_t,ADDR_SIZE> &msg){
    int m[msg.size()];
    std::copy(msg.begin(),msg.end(),m);
    mclBnG1_hashAndMapTo(&g1,m, msg.size());
}
void ecdsa_sig::H2(mclBnG1 &g1, const array<uint8_t,ADDR_SIZE> &msg){
	uint8_t hash[SHA256_DIGEST_LENGTH];
    uint8_t m[msg.size()];
    std::copy(msg.begin(),msg.end(),m);
	SHA256( m, msg.size() , hash );
	mclBnG1_hashAndMapTo(&g1,hash, SHA256_DIGEST_LENGTH);
}
void ecdsa_sig::H3(mclBnFr &fr, const vector<uint8_t> &msg){
	uint8_t hash[SHA256_DIGEST_LENGTH];
    uint8_t m[msg.size()];
    std::copy(msg.begin(),msg.end(),m);

	SHA256( m, msg.size() , hash );
	mclBnFr_setLittleEndian(&fr, hash, SHA256_DIGEST_LENGTH);
}

void ecdsa_sig::deserialize_keys(){
    std::ifstream ifs_mpk("ecdsa_mpk.key", std::ios::binary);
    std::ifstream ifs_msk("ecdsa_msk.key", std::ios::binary);
    
    uint8_t mpk_buf[MPK_LENGTH];
    uint8_t msk_buf[MSK_LENGTH];
    ifs_mpk.read((char*)mpk_buf,MPK_LENGTH);
    ifs_msk.read((char*)msk_buf,MSK_LENGTH);
    mclBnG1_deserialize(&(this->mpk.mpk1g1),&mpk_buf[INDEX_MPK_1G1],G1_LENGTH);
    mclBnG2_deserialize(&(this->mpk.mpk1g2),&mpk_buf[INDEX_MPK_1G2],G2_LENGTH);
    mclBnG2_deserialize(&(this->mpk.mpk2),&mpk_buf[INDEX_MPK_2G2],G2_LENGTH);
    mclBnG2_deserialize(&(this->mpk.mpk3),&mpk_buf[INDEX_MPK_3G2],G2_LENGTH);
	mclBnFr_deserialize(&(this->msk.msk1),&msk_buf[INDEX_MSK_1],FR_LENGTH);
	mclBnFr_deserialize(&(this->msk.msk2),&msk_buf[INDEX_MSK_2],FR_LENGTH);

}
void ecdsa_sig::serialize_sig(const signature &sig, vector<uint8_t> &buf){

    buf.resize(SIG_LENGTH);
    uint8_t tmp[SIG_LENGTH];

	mclBnG1_serialize(&tmp[INDEX_SIG_1G1],G1_LENGTH,&(sig.g1));
	mclBnG1_serialize(&tmp[INDEX_SIG_2G1],G1_LENGTH,&(sig.g2));
	mclBnG1_serialize(&tmp[INDEX_SIG_3G1],G1_LENGTH,&(sig.g3g1));
	mclBnG2_serialize(&tmp[INDEX_SIG_3G2],G2_LENGTH,&(sig.g3g2));
    std::copy(tmp,tmp+SIG_LENGTH,buf.begin());
}
void ecdsa_sig::deserialize_sig(signature &sig, const vector<uint8_t> &buf){
	int g1size=mclBn_getG1ByteSize();
	int g2size=mclBn_getG2ByteSize();
    uint8_t tmp[SIG_LENGTH];
    std::copy(buf.begin(),buf.end(),tmp);
	mclBnG1_deserialize(&(sig.g1),&buf[INDEX_SIG_1G1],G1_LENGTH);
	mclBnG1_deserialize(&(sig.g2),&buf[INDEX_SIG_2G1],G1_LENGTH);
	mclBnG1_deserialize(&(sig.g3g1),&buf[INDEX_SIG_3G1],G1_LENGTH);
	mclBnG2_deserialize(&(sig.g3g2),&buf[INDEX_SIG_3G2],G2_LENGTH);
}
void ecdsa_sig::setup(){
	int ret = mclBn_init(MCL_BLS12_381, MCLBN_COMPILED_TIME_VAR);
	if (ret != 0) {
		printf("err ret=%d\n", ret);
		return ;
	}
	this->deserialize_keys();
}
void ecdsa_sig::key_derivation(){
	mclBnG1 tmp;
    this->H1(tmp,this->id);
    mclBnG1_mul(&(this->isk.isk1),&tmp,&(this->msk.msk1));
    this->H2(tmp,this->id);
    mclBnG1_mul(&(this->isk.isk2),&tmp,&(this->msk.msk2));
}
void ecdsa_sig::sign(isdsr_packet &p){
	//printf("ibsas signing start msg=%s\n",msg);
	//printSignatures(sig);

	signature sig;
	if(p.get_ri_length()==1){
		mclBnG1_clear(&(sig.g1));
		mclBnG1_clear(&(sig.g2));
		mclBnG1_clear(&(sig.g3g1));
		mclBnG2_clear(&(sig.g3g2));
	}
	else{
		deserialize_sig(sig,*(p.get_sig()));
	}
	
    mclBnFr x,r;
    mclBnFr_setInt(&x, 13);
    mclBnFr_setInt(&r, 7);
    //mclBnFr_setByCSPRNG(&x);
    //mclBnFr_setByCSPRNG(&r);

    mclBnG1 h3isk1;
    mclBnG1 rsig3;
    mclBnG1 xsig2p;
    mclBnG1 xg1;
    mclBnG2 xg2;
    mclBnG1 rg;

    mclBnG1_mul(&rg, &(mpk.mpk1g1),&r);//rg
    mclBnG1_add(&(sig.g2),&(sig.g2),&rg);// newsig2=rg+sig2 sig2' is done

    mclBnG1_mul(&rsig3, &(sig.g3g1),&r);// r*sig3
    mclBnG1_mul(&xsig2p,&(sig.g2),&x);// tmp = sig2'*x
    int idmeslength=ADDR_SIZE+p.get_ri_length()*ADDR_SIZE;

    vector<uint8_t> tmp(idmeslength);
    std::copy(this->id.begin(),this->id.end(),tmp.begin());
    vector<array<uint8_t,ADDR_SIZE>> *ri=p.get_ri();
    for(int i=0;i<p.get_ri_length();i++){
        std::copy(ri->at(i).begin(),ri->at(i).end(),tmp.begin()+(ADDR_SIZE*(i+1)));
    }
    mclBnFr h3;
    H3(h3,tmp);// H3(ID || m)
    mclBnG1_mul(&h3isk1,&(this->isk.isk1),&h3);// sk1 * H3(ID||m), sk1:alpha1*H1(ID)
    mclBnG1_add(&(sig.g1),&(sig.g1), &rsig3);
    mclBnG1_add(&(sig.g1),&(sig.g1), &xsig2p);
    mclBnG1_add(&(sig.g1),&(sig.g1), &(this->isk.isk2));
    mclBnG1_add(&(sig.g1),&(sig.g1), &h3isk1);

    mclBnG1_mul(&xg1, &(this->mpk.mpk1g1),&x);// newsig3g1=xg
    mclBnG2_mul(&xg2, &(this->mpk.mpk1g2),&x);// newsig3g2=xg
    mclBnG1_add(&(sig.g3g1),&(sig.g3g1), &xg1);// newsig3g1=xg+sig3g1 sig3' is done
    mclBnG2_add(&(sig.g3g2),&(sig.g3g2), &xg2);// newsig3g2=xg+sig3g2 sig3' is done

    tmp.clear();

	serialize_sig(sig,tmp);
    p.set_sig(tmp);
	return;
}

bool ecdsa_sig::verify(isdsr_packet &p){
	signature sig;
	deserialize_sig(sig,*(p.get_sig()));
	mclBnGT t1,t2;
    mclBn_pairing(&t1, &(sig.g1),&(this->mpk.mpk1g2));
    mclBn_pairing(&t2, &(sig.g2),&(sig.g3g2));

    vector<uint8_t> idmsg;
    vector<array<uint8_t,ADDR_SIZE>> *ri=p.get_ri();
    mclBnG1 t3,t4,t5,t7;
    mclBnFr t6;
    mclBnG1_clear(&t3);
    mclBnG1_clear(&t7);

    int i=0;
    do{
        this->H2(t4,ri->at(i));
        mclBnG1_add(&t3,&t3,&t4);
        this->H1(t5,ri->at(i));
        idmsg.resize(ADDR_SIZE+ADDR_SIZE*(i+1));
        std::copy(ri->at(i).begin(),ri->at(i).end(),idmsg.begin());
        for(int j=0;j<(i+1);j++){
            std::copy(ri->at(j).begin(),ri->at(j).end(),idmsg.begin()+(ADDR_SIZE+(ADDR_SIZE*j)));
        }
        this->H3(t6,idmsg);
        mclBnG1_mul(&t5,&t5,&t6);
        mclBnG1_add(&t7,&t7,&t5);
        i++;
    }while(i<p.get_ri_length());

    mclBnGT t8,t9;
    mclBn_pairing(&t8, &t3,&(mpk.mpk3));
    mclBn_pairing(&t9, &t7,&(mpk.mpk2));

    mclBnGT_mul(&t2,&t2,&t8);
    mclBnGT_mul(&t2,&t2,&t9);

    int ret=mclBnGT_isEqual(&t1,&t2);
    return (ret==1);
}
