#include "adhoc_routing.hpp"

using namespace oit::ist::nws::adhoc_routing;
adhoc_routing::adhoc_routing(){this->seq=0;}
adhoc_routing::~adhoc_routing(){}
void adhoc_routing::set_id(std::array<std::uint8_t,ADDR_SIZE> &id){
    std::copy(id.begin(),id.end(),this->id.begin());
}
void adhoc_routing::set_next(std::array<std::uint8_t,ADDR_SIZE> &id){
    std::copy(id.begin(),id.end(),this->next.begin());
}
void adhoc_routing::set_broadcast(std::array<std::uint8_t,ADDR_SIZE> &id){
    std::copy(id.begin(),id.end(),this->broadcast.begin());
}
std::array<std::uint8_t,ADDR_SIZE>* adhoc_routing::get_id(){return &(this->id);}
std::array<std::uint8_t,ADDR_SIZE>* adhoc_routing::get_next(){return &(this->next);}
std::array<std::uint8_t,ADDR_SIZE>* adhoc_routing::get_broadcast(){return &(this->broadcast);}
std::string adhoc_routing::to_string(){
    string ret="routing name:"+this->routing_name();
    ret+=" id:"+adhoc_util::to_string_iparray(this->id);
    ret+=" brd:"+adhoc_util::to_string_iparray(this->broadcast);
    return ret;
}

std::array<std::uint8_t,ADDR_SIZE>* adhoc_routing::packet_processing(std::vector<std::uint8_t> &buf){
    switch(buf.at(INDEX_TYPE)){
        case RREQ:{
            return this->processing_rreq(buf);
        }
        case RREP:{
            return this->processing_rrep(buf);
        }
        case RERR:{
            return this->processing_rerr(buf);
        }
        case DATA:{
            return this->processing_data(buf);
        }
        default:{

        }
    }
    return nullptr;
}