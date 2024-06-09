#include <iostream>
#include "../adhoc/adhoc_node.hpp"
#include "../isdsr/isdsr_packet.hpp"
#include "../isdsr/isdsr_routing.hpp"
#include "lattice_sig.hpp"
#include <array>
#include <vector>
using namespace oit::ist::nws::adhoc_routing;

int main(int argc, char** argv){
    array<uint8_t,ADDR_SIZE> src={10,0,0,2};
    array<uint8_t,ADDR_SIZE> dest={10,0,0,10};
    array<uint8_t,ADDR_SIZE> f1={10,0,0,4};
    array<uint8_t,ADDR_SIZE> f2={10,0,0,5};
    array<uint8_t,ADDR_SIZE> f3={10,0,0,6};
    array<uint8_t,ADDR_SIZE> f4={10,0,0,7};

    isdsr_packet p1(RREQ,src,dest);
    lattice_sig lattice1;
    lattice1.set_id(src);
    lattice_sig lattice2;
    lattice2.set_id(dest);

    vector<uint8_t> buf1;
    vector<uint8_t> buf2;
    lattice1.sign(p1);
    bool v=lattice2.verify(p1);

    std::cout<<"verify: "<<std::to_string(v)<<std::endl;
}