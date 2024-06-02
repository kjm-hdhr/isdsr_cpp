#ifndef ISDSR_ECDSA_H
#define ISDSR_ECDSA_H

#include <mcl/bn_c384_256.h>
#include <stdint.h>
#include "../isdsr/isdsr.h"

#define IBSAS_SIG_LEN mclBn_getG1ByteSize()*3+mclBn_getG2ByteSize()
typedef struct{
  mclBnG1 mpk1g1;
  mclBnG2 mpk1g2;
  mclBnG2 mpk2;
  mclBnG2 mpk3;
} master_pk;

typedef struct{
  mclBnFr msk1;
  mclBnFr msk2;
} master_sk;

typedef struct{
  mclBnG1 isk1;
  mclBnG1 isk2;
} id_secret_key;

typedef struct{
  mclBnG1 g1;
  mclBnG1 g2;
  mclBnG1 g3g1;
  mclBnG2 g3g2;
} signature;

extern master_pk mpk;
extern master_sk msk;

void H1(mclBnG1*, uint8_t*,int);
void H2(mclBnG1*, const uint8_t*, int);
void H3(mclBnFr*, const uint8_t*, int);
void serialize_signature(const signature *sig, uint8_t *buf);
void deserialize_signature(signature *sig, const uint8_t *buf);

#endif