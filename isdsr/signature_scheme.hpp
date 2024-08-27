#ifndef SIGNATURE_SCHEME
#define SIGNATURE_SCHEME

#include "isdsr_packet.hpp"
namespace oit::ist::nws::adhoc_routing{
class signature_scheme{

    protected:
    array<uint8_t,ADDR_SIZE> id;

    public:
    signature_scheme(){};
    signature_scheme(array<uint8_t, ADDR_SIZE> &id){this->set_id(id);};
    void set_id(array<uint8_t, ADDR_SIZE> &id){std::copy(id.begin(),id.end(),this->id.begin());}
    virtual void setup()=0;
    virtual void key_derivation()=0;
    virtual void sign(isdsr_packet &pkt)=0;
    virtual bool verify(isdsr_packet &pkt)=0;
    virtual string signature_scheme_name()=0;
};
};
#endif