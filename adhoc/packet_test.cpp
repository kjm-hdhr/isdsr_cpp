#include "adhoc_routing_packet.hpp"
#include <iostream>
using namespace oit::ist::nws::adhoc_routing;

int main(int argc, char** argv){
    std::array<uint8_t,ADDR_SIZE> src={10,0,0,1};
    std::array<uint8_t,ADDR_SIZE> dest={10,0,0,5};
    std::cout<<"src[0]:";
    std::cout<<(int)src[0]<<std::endl;
    ar_packet rp1;
    rp1.set_type(1);
    rp1.set_src(src);
    rp1.set_dest(dest);
    
    std::cout<<"rp1="<<rp1.to_string()<<std::endl;
    std::cout<<"is src="<<rp1.is_src(src);
    std::cout<<" is dest="<<rp1.is_dest(dest)<<std::endl;
    std::vector<uint8_t> buf;
    rp1.serialize(buf);
    std::cout<<"buf size:"<<buf.size()<<std::endl;
    std::cout<<"buf=["<<std::to_string(buf[0]);
    for(int i=1;i<buf.size();i++){
        std::cout<<","<<std::to_string(buf[i]);
    }
    std::cout<<"]"<<std::endl;

    ar_packet rp2;
    rp2.deserialize(buf);
    std::cout<<"rp2="<<rp2.to_string()<<std::endl;
    std::cout<<"is src="<<rp1.is_src(src);
    std::cout<<" is dest="<<rp1.is_dest(dest)<<std::endl;
    rp2.swap_src_dest();
    std::cout<<"rp2 swapped="<<rp2.to_string()<<std::endl;
}
