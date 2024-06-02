#ifndef ECDSA_SIG_HPP
#define ECDSA_SIG_HPP

#include <mcl/bn_c384_256.h>
#include <array>
#include "../signature_scheme.hpp"

namespace oit::ist::nws::adhoc_routing{

#define IBSAS_SIG_LEN mclBn_getG1ByteSize()*3+mclBn_getG2ByteSize()
#define G1_LENGTH mclBn_getG1ByteSize()
#define G2_LENGTH mclBn_getG2ByteSize()
#define FR_LENGTH mclBn_getFrByteSize()

#define INDEX_MPK_1G1 0
#define INDEX_MPK_1G2 G1_LENGTH
#define INDEX_MPK_2G2 INDEX_MPK_1G2+G2_LENGTH
#define INDEX_MPK_3G2 INDEX_MPK_2G2+G2_LENGTH
#define MPK_LENGTH G1_LENGTH+G2_LENGTH*3
typedef struct{
  mclBnG1 mpk1g1;
  mclBnG2 mpk1g2;
  mclBnG2 mpk2;
  mclBnG2 mpk3;
} master_pk;

#define INDEX_MSK_1 0
#define INDEX_MSK_2 INDEX_MSK_1+FR_LENGTH
#define MSK_LENGTH FR_LENGTH*2
typedef struct{
  mclBnFr msk1;
  mclBnFr msk2;
} master_sk;

#define INDEX_ISK_1 0
#define INDEX_ISK_2 INDEX_ISK_1+G1_LENGTH
#define ISK_LENGTH G1_LENGTH*2
typedef struct{
  mclBnG1 isk1;
  mclBnG1 isk2;
} id_secret_key;

#define INDEX_SIG_1G1 0
#define INDEX_SIG_2G1 INDEX_SIG_1G1+G1_LENGTH
#define INDEX_SIG_3G1 INDEX_SIG_2G1+G1_LENGTH
#define INDEX_SIG_3G2 INDEX_SIG_3G1+G1_LENGTH
#define SIG_LENGTH G1_LENGTH*3+G2_LENGTH
typedef struct{
  mclBnG1 g1;
  mclBnG1 g2;
  mclBnG1 g3g1;
  mclBnG2 g3g2;
} signature;

class ecdsa_sig:public signature_scheme{
    protected:
    master_pk mpk;
    master_sk msk;
    id_secret_key isk;

    void H1(mclBnG1 &g1, const array<uint8_t,ADDR_SIZE> &msg);
    void H2(mclBnG1 &g1, const array<uint8_t,ADDR_SIZE> &msg);
    void H3(mclBnFr &fr, const vector<uint8_t> &msg);
    void deserialize_keys();
    void serialize_sig(const signature &sig, vector<uint8_t> &buf);
    void deserialize_sig(signature &sig, const vector<uint8_t> &buf);
    public:
    ecdsa_sig(){}
    ~ecdsa_sig(){}
    virtual void setup();
    virtual void key_derivation();
    virtual void sign(isdsr_packet &pkt);
    virtual bool verify(isdsr_packet &pkt);
    virtual string signature_scheme_name(){return "ecdsa";};
};
};
#endif