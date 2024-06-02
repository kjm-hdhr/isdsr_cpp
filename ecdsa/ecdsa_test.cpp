#include <iostream>
#include "../adhoc/adhoc_node.hpp"
#include "../isdsr/isdsr_packet.hpp"
#include "../isdsr/isdsr_routing.hpp"
#include "ecdsa_sig.hpp"
#include <array>
#include <vector>
using namespace oit::ist::nws::adhoc_routing;

int main(int argc, char** argv){
    array<uint8_t,ADDR_SIZE> src={10,0,0,3};
    array<uint8_t,ADDR_SIZE> dest={10,0,0,10};
    array<uint8_t,ADDR_SIZE> f1={10,0,0,4};
    array<uint8_t,ADDR_SIZE> f2={10,0,0,5};
    array<uint8_t,ADDR_SIZE> f3={10,0,0,6};
    array<uint8_t,ADDR_SIZE> f4={10,0,0,7};

    isdsr_packet p(RREQ,src,dest);
    isdsr_routing ir1,ir2;
    ir1.set_id(src);
    ir2.set_id(f1);
    ecdsa_sig ecdsa1;
    ecdsa_sig ecdsa2;
    ecdsa1.set_id(src);
    ecdsa2.set_id(f1);
    ir1.set_signature_scheme(&ecdsa1);
    ir2.set_signature_scheme(&ecdsa2);
    vector<uint8_t> buf1;
    vector<uint8_t> buf2;
    ir1.generate_initiali_request(dest,buf1);
    ir2.packet_processing(buf1);

}