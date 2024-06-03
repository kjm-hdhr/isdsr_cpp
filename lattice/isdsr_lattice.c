#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "packing_agg.h"
#include "sign_agg.h"
#include "../isdsr/isdsr.h"

uint8_t mpk[CRYPTO_PUBLICKEYBYTES];
uint8_t msk[CRYPTO_SECRETKEYBYTES];
uint8_t pk_agg[PUBLIC_KEY_AGG_BYTES];
uint8_t skid[SECRET_KEY_AGG_BYTES];

void setup(){
	FILE *file;

  file = fopen("lattice_mpk.key", "rb");
  fread(mpk, CRYPTO_PUBLICKEYBYTES, 1, file);
  fclose(file);
  file = fopen("lattice_msk.key", "rb");
  fread(msk, CRYPTO_SECRETKEYBYTES, 1, file);
  fclose(file);
}
void key_derivation(uint8_t own_id[IP_LENGTH]){
	memset(pk_agg,0,PUBLIC_KEY_AGG_BYTES);
  memset(skid,0,SECRET_KEY_AGG_BYTES);
	crypto_sign_keypair_skid(pk_agg, skid,own_id,msk);
}
void signing(isdsr_p *p){
	uint8_t sig[EACH_SIG_LENGTH];
	size_t siglen;
	uint8_t *prev_aggsig=p->sig;
	p->sig=(uint8_t*)malloc(SEEDBYTES+POLY_MODQ_LENGTH*L+POLY_MODQ_LENGTH*K*p->dsr->rilen+1);
	memset(p->sig,0,SEEDBYTES+POLY_MODQ_LENGTH*L+POLY_MODQ_LENGTH*K*p->dsr->rilen+1);
	crypto_sign_signatre_skid(
    sig, &siglen,p->dsr->ri,p->dsr->rilen*IP_LENGTH,skid);
	crypto_sign_signature_agg(p->sig,prev_aggsig,sig,p->dsr->rilen-1);
	p->siglen=SEEDBYTES+POLY_MODQ_LENGTH*L+POLY_MODQ_LENGTH*K*p->dsr->rilen+1;
	
	free(prev_aggsig);
}

int verify(isdsr_p *p){
	int v=crypto_sign_verify_agg(pk_agg,p->sig,p->dsr->ri,p->dsr->rilen);
	return v;
}

void generate_initial_signature(isdsr_p *p){
	p->sig=(uint8_t*)malloc(EACH_SIG_LENGTH+1);
	memset(p->sig,0,EACH_SIG_LENGTH+1);
	signing(p);
}

uint8_t isdsr_rreq_processing(isdsr_p *p){
	if(find_id_in_ri(p->dsr)){
		return 0;
	}
	int v=verify(p);
	if(v==0){
		printf("not valid \n");
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
	int v=verify(p);
	if(v==0){
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
