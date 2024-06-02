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
    std::vector<arf_packet> pkts;
    std::vector<arf_packet> pkts2;
    
    array<uint8_t,ADDR_SIZE> id;
    id[0]=127;id[1]=0;id[2]=0;id[3]=1;
    array<uint8_t,ADDR_SIZE> id2;
    id2[0]=127;id2[1]=0;id2[2]=0;id2[3]=5;
    arf_manager afm;
    afm.set_own_id(id);
    arf_manager afm2;
    afm2.set_own_id(id2);
    afm.set_fragment_size(25);
    afm2.set_fragment_size(10);
    afm.set_initial_seq_no(100);
    afm2.set_initial_seq_no(100);
    afm.fragment(pkts,v);
    afm2.fragment(pkts2,v);
    for(int i=0;i<pkts.size();i++){
        std::cout<<pkts.at(i).to_string()<<std::endl;
    }
    for(int i=0;i<pkts2.size();i++){
        std::cout<<pkts2.at(i).to_string()<<std::endl;
    }
    std::vector<uint8_t> v12;
    std::vector<uint8_t> v22;
    for(int i=0;i<3;i++){
        afm.defragment(pkts.at(i),v12);
        afm.defragment(pkts2.at(i),v22);
    }
    std::cout<<"arf_manager1"<<std::endl;
    std::cout<<afm.to_string()<<std::endl;
    for(int i=3;i<pkts2.size();i++){
        afm.defragment(pkts2.at(i),v22);
    }
    std::cout<<"arf_manager2"<<std::endl;
    std::cout<<afm.to_string()<<std::endl;
    


}