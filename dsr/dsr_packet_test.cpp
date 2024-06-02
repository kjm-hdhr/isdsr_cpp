#include "dsr_packet.hpp"
#include <iostream>
using namespace oit::ist::nws::adhoc_routing;

int main(int argc, char** argv){

    array<uint8_t,ADDR_SIZE> src={10,0,0,1};
    array<uint8_t,ADDR_SIZE> dest={10,0,0,5};
    array<uint8_t,ADDR_SIZE> f1={10,0,0,4};
    array<uint8_t,ADDR_SIZE> f2={10,0,0,10};
    array<uint8_t,ADDR_SIZE> f3={10,0,0,20};
    std::cout<<"src[0]:";
    std::cout<<(int)src[0]<<std::endl;
    dsr_packet p(1,src,dest);
    //dsr_packet p;
    p.add_id(src);
    std::cout<<"find f1 "<<std::to_string(p.find_id(f1))<<std::endl;
    std::cout<<"find src "<<std::to_string(p.find_id(src))<<std::endl;
    p.add_id(f1);
    std::cout<<"find f1 "<<std::to_string(p.find_id(f1))<<std::endl;
    std::cout<<p.to_string()<<std::endl;
    int psize=p.packet_size();
    std::cout<<"packet size="<<psize<<std::endl;
    std::vector<std::uint8_t> buf(psize);
    p.serialize(buf);
    for(int i=0;i<buf.size();i++){
        std::cout<<" "<<(int)buf.at(i);
    }
    std::cout<<std::endl;

    p.add_id(f2);
    p.add_id(f3);
    std::cout<<p.to_string()<<std::endl;
    array<uint8_t,ADDR_SIZE> *tmp;
    tmp=p.previous_id(f2);
    std::cout<<"previous id"<<adhoc_util::to_string_iparray(*tmp)<<std::endl;
    dsr_packet p2;
    p2.deserialize(buf);
    p2.to_string();

}
