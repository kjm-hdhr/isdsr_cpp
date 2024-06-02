#include "adhoc_routing_fragment.hpp"

#include <vector>
#include <iostream>
#include <cstdint>
using namespace oit::ist::nws::adhoc_routing;
int main(int argc, char** argv){
    std::vector<uint8_t> v;
    std::vector<uint8_t> v2;
    std::vector<uint8_t> v3;
    for(int i=0;i<100;i++){
        v.push_back(i%0x100);
        v2.push_back(i%0x10);
        v3.push_back(i%20);
    }
    arf_manager afm;
    std::vector<arf_packet> pkts3;
    afm.set_fragment_size(1000);
    afm.set_initial_seq_no(300);
    afm.fragment(pkts3,v3);
    std::vector<uint8_t> v32;
    std::cout<<"arf_manager3"<<std::endl;
    std::cout<<afm.to_string()<<std::endl;
    for(int i=0;i<pkts3.size();i++){
        afm.defragment(pkts3.at(i),v32);
    }
    std::cout<<"v32=["+std::to_string(v32.at(0));
    for(int i=1;i<v32.size();i++){
        std::cout<<","+std::to_string(v32.at(i));
    }
    std::cout<<"]"<<std::endl;
}