#include <iostream>
#include "../adhoc/adhoc_node.hpp"
#include "../isdsr/isdsr_packet.hpp"
#include "../isdsr/isdsr_routing.hpp"
#include "lattice_sig.hpp"
#include <array>
#include <vector>
#include <iterator>
using namespace oit::ist::nws::adhoc_routing;

int main(int argc, char** argv){
    std::array<std::uint8_t,ADDR_SIZE> src={10,0,0,2};
    std::array<std::uint8_t,ADDR_SIZE> dest={10,0,0,10};
    std::array<std::uint8_t,ADDR_SIZE> f1={10,0,0,4};
    std::array<std::uint8_t,ADDR_SIZE> f2={10,0,0,5};
    std::array<std::uint8_t,ADDR_SIZE> f3={10,0,0,6};
    std::array<std::uint8_t,ADDR_SIZE> f4={10,0,0,7};
    std::vector<std::uint8_t> buf1(60000);
    std::cerr<<" generate nodes "<<std::endl;
    isdsr_packet p(RREQ,src,dest);
    isdsr_routing ir1,ir2;
    ir1.set_id(src);
    ir2.set_id(f1);
    std::cerr<<" set node id1 "<<std::endl;
    lattice_sig lattice1;
    lattice_sig lattice2;
    ir1.set_signature_scheme(&lattice1);
    ir2.set_signature_scheme(&lattice2);

    std::cerr<<" set schemes "<<std::endl;
    p.add_id(src);
    lattice1.sign(p);
    std::uint8_t b_array[p.packet_size()];
    
    //buf1.push_back(0);
    buf1.resize(8000);
    //std::vector<std::uint8_t> buf1(p.packet_size());
    std::cerr<<"buf1 capacity:"<<std::to_string(buf1.capacity())<<std::endl;
    p.serialize(buf1);
    std::vector<std::uint8_t> buf2;
    //ir1.generate_initiali_request(dest,buf1);
    //ir2.packet_processing(buf1);

}