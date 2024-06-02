#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>
#include <stdlib.h>
#include "isdsr_ecdsa.h"


master_pk mpk;
master_sk msk;
id_secret_key isk;


void serialize_signature(const signature *sig, uint8_t *buf){
	int g1size=mclBn_getG1ByteSize();
	int g2size=mclBn_getG2ByteSize();
	mclBnG1_serialize(&buf[0],g1size,&(sig->g1));
	mclBnG1_serialize(&buf[g1size],g1size,&(sig->g2));
	mclBnG1_serialize(&buf[g1size*2],g1size,&(sig->g3g1));
	mclBnG2_serialize(&buf[g1size*3],g2size,&(sig->g3g2));
}
void deserialize_signature(signature *sig, const uint8_t *buf){
	int g1size=mclBn_getG1ByteSize();
	int g2size=mclBn_getG2ByteSize();
	mclBnG1_deserialize(&(sig->g1),&buf[0],g1size);
	mclBnG1_deserialize(&(sig->g2),&buf[g1size],g1size);
	mclBnG1_deserialize(&(sig->g3g1),&buf[g1size*2],g1size);
	mclBnG2_deserialize(&(sig->g3g2),&buf[g1size*3],g2size);
}
void deserialize_keys(){
	
	int g1size=mclBn_getG1ByteSize();
	int g2size=mclBn_getG2ByteSize();
	int frsize=mclBn_getFrByteSize();
	int mpk_size=g1size+g2size*3;
  int msk_size=frsize*2;
  uint8_t mpk_buf[mpk_size];
  uint8_t msk_buf[msk_size];
	FILE *file;

  file = fopen("ecdsa_mpk.key", "rb");
  fread(mpk_buf, mpk_size, 1, file);
  fclose(file);
  file = fopen("ecdsa_msk.key", "rb");
  fread(msk_buf, msk_size, 1, file);
  fclose(file);
  mclBnG1_deserialize(&(mpk.mpk1g1),&mpk_buf[0],g1size);
  mclBnG2_deserialize(&(mpk.mpk1g2),&mpk_buf[g1size],g2size);
  mclBnG2_deserialize(&(mpk.mpk2),&mpk_buf[g1size+g2size],g2size);
  mclBnG2_deserialize(&(mpk.mpk3),&mpk_buf[g1size+g2size*2],g2size);
	mclBnFr_deserialize(&(msk.msk1),&msk_buf[0],frsize);
	mclBnFr_deserialize(&(msk.msk2),&msk_buf[frsize],frsize);
}

void setup(){
	int ret = mclBn_init(MCL_BLS12_381, MCLBN_COMPILED_TIME_VAR);

	if (ret != 0) {
		printf("err ret=%d\n", ret);
		return ;
	}
	deserialize_keys();
}
void key_derivation(uint8_t own_id[IP_LENGTH]){
	mclBnG1 tmp;
  H1(&tmp,id,IP_LENGTH);
  mclBnG1_mul(&(isk.isk1),&tmp,&(msk.msk1));

  H2(&(tmp),(unsigned char*)id,IP_LENGTH);
  mclBnG1_mul(&(isk.isk2),&(tmp),&(msk.msk2));
}
void signing(isdsr_p *p){
	//printf("ibsas signing start msg=%s\n",msg);
	//printSignatures(sig);

	signature sig;
	if(p->dsr->rilen==1){
		mclBnG1_clear(&(sig.g1));
		mclBnG1_clear(&(sig.g2));
		mclBnG1_clear(&(sig.g3g1));
		mclBnG2_clear(&(sig.g3g2));
	}
	else{
		deserialize_signature(&sig,p->sig);
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
	int idmeslength=IP_LENGTH+p->dsr->rilen*IP_LENGTH;
	uint8_t idm[idmeslength];
	memcpy(&idm[0],id,IP_LENGTH);
  memcpy(&idm[IP_LENGTH],p->dsr->ri,p->dsr->rilen*IP_LENGTH);
  //char idm[idmeslength];
  //sprintf(idm,"%s%s",id,msg);
  mclBnFr h3;
  H3(&h3,(unsigned char*)idm,idmeslength);// H3(ID || m)
  mclBnG1_mul(&h3isk1,&(isk.isk1),&h3);// sk1 * H3(ID||m), sk1:alpha1*H1(ID)
  mclBnG1_add(&(sig.g1),&(sig.g1), &rsig3);
  mclBnG1_add(&(sig.g1),&(sig.g1), &xsig2p);
  mclBnG1_add(&(sig.g1),&(sig.g1), &(isk.isk2));
  mclBnG1_add(&(sig.g1),&(sig.g1), &h3isk1);

  mclBnG1_mul(&xg1, &(mpk.mpk1g1),&x);// newsig3g1=xg
  mclBnG2_mul(&xg2, &(mpk.mpk1g2),&x);// newsig3g2=xg
  mclBnG1_add(&(sig.g3g1),&(sig.g3g1), &xg1);// newsig3g1=xg+sig3g1 sig3' is done
  mclBnG2_add(&(sig.g3g2),&(sig.g3g2), &xg2);// newsig3g2=xg+sig3g2 sig3' is done
	serialize_signature(&sig,p->sig);

	return;
}
int verify(isdsr_p *p){
	signature sig;
	deserialize_signature(&sig,p->sig);
	mclBnGT t1,t2;
  mclBn_pairing(&t1, &(sig.g1),&(mpk.mpk1g2));
  mclBn_pairing(&t2, &(sig.g2),&(sig.g3g2));

  int i=0,ret=1;
	uint8_t idmsg[IP_LENGTH+IP_LENGTH*p->dsr->rilen];
	memset(idmsg,0,IP_LENGTH+IP_LENGTH*p->dsr->rilen);

  mclBnG1 t3,t4,t5,t7;
  mclBnFr t6;
  mclBnG1_clear(&t3);
  mclBnG1_clear(&t7);
  i=0;
  do{
		H2(&t4,&(p->dsr->ri[i*IP_LENGTH]),IP_LENGTH);
		mclBnG1_add(&t3,&t3,&t4);
		H1(&t5,&(p->dsr->ri[i*IP_LENGTH]),IP_LENGTH);
		memcpy(&idmsg[0],&(p->dsr->ri[i*IP_LENGTH]),IP_LENGTH);
		memcpy(&idmsg[IP_LENGTH],&(p->dsr->ri[0]),IP_LENGTH*(i+1));
		H3(&t6,&(idmsg[0]),IP_LENGTH+IP_LENGTH*(i+1));
    	mclBnG1_mul(&t5,&t5,&t6);
		mclBnG1_add(&t7,&t7,&t5);
		i++;
	}while(i<p->dsr->rilen);

  mclBnGT t8,t9;
  mclBn_pairing(&t8, &t3,&(mpk.mpk3));
  mclBn_pairing(&t9, &t7,&(mpk.mpk2));

  mclBnGT_mul(&t2,&t2,&t8);
  mclBnGT_mul(&t2,&t2,&t9);

  ret=mclBnGT_isEqual(&t1,&t2);
  return ret;
}

void H1(mclBnG1* g1, uint8_t* msg,int length){
  mclBnG1_hashAndMapTo(g1,msg, length);
}
void H2(mclBnG1* g1, const uint8_t* msg, int length){
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256( msg,length , hash );
	mclBnG1_hashAndMapTo(g1,hash, SHA256_DIGEST_LENGTH);
}
void H3(mclBnFr* fr, const uint8_t* msg, int length){
	uint8_t hash[SHA256_DIGEST_LENGTH];
	SHA256( msg,length , hash );
	mclBnFr_setLittleEndian(fr, hash, SHA256_DIGEST_LENGTH);
}

void generate_initial_signature(isdsr_p *p){
	p->siglen=IBSAS_SIG_LEN;
	p->sig=(uint8_t*)malloc(p->siglen);
	memset(p->sig,0,p->siglen);
	signing(p);
}

uint8_t isdsr_rreq_processing(isdsr_p *p){
	if(find_id_in_ri(p->dsr)){
		return 0;
	}
	if(!verify(p)){
		return 0;
	}
	int i;
	if(check_same_id(id,p->dsr->dest)){
		p->dsr->ptype=RREP;
		uint8_t tmp;
		for(i=0;i<IP_LENGTH;i++){
			tmp=p->dsr->src[i];
			p->dsr->src[i]=p->dsr->dest[i];
			p->dsr->dest[i]=tmp;
		}
	}
	add_ri(p->dsr,id);
	signing(p);
	p->dsr->plen=isdsr_packet_size(p);
	set_next_id(p->dsr);
	return p->dsr->ptype;
}

uint8_t isdsr_rrep_processing(isdsr_p *p){
	if(!verify(p)){
		printf("received RREP was not valid\n");
	}
	if(check_same_id(id,p->dsr->dest)){
		printf("route established\n");
		return 0;
	}
	set_next_id(p->dsr);
	return RREQ;
}

uint8_t isdsr_rerr_processing(isdsr_p *p){
	return RERR;
}

uint8_t isdsr_data_processing(isdsr_p *p){
	return DATA;
}