#include "isdsr_packet.hpp"

using namespace oit::ist::nws::adhoc_routing;

isdsr_packet::isdsr_packet(){
    this->initialize();
}
isdsr_packet::isdsr_packet(uint8_t type, array<uint8_t,ADDR_SIZE> &src_id, array<uint8_t,ADDR_SIZE> &dest_id):dsr_packet(type,src_id,dest_id){
    this->initialize();
}
isdsr_packet::~isdsr_packet(){
    this->sig.clear();
}
uint32_t isdsr_packet::packet_size(){
    uint32_t ret=this->dsr_packet::packet_size();
    ret+=SIG_LENGTH_SIZE;
    this->sig_length=this->sig.size();
    ret+=this->sig_length;
    return ret;
}
void isdsr_packet::initialize(){
    this->dsr_packet::initialize();
    this->sig_length=0;
    this->sig.clear();
}

uint32_t isdsr_packet::serialize(vector<uint8_t> &buf){
    if(buf.size()<this->packet_size()){
        buf.resize(this->packet_size());
    }
    this->dsr_packet::serialize(buf);
    this->sig_length=this->sig.size();
    adhoc_util::serialize_uint32(INDEX_SIG_LENGTH,buf,this->sig_length);
    std::copy(this->sig.begin(),this->sig.end(),buf.begin()+INDEX_SIG);
    return INDEX_SIG+this->sig_length;
}
uint32_t isdsr_packet::deserialize(const vector<uint8_t> &buf){
    this->dsr_packet::deserialize(buf);
    this->sig_length=adhoc_util::deserialize_uint32(INDEX_SIG_LENGTH,buf);
    this->sig.resize(this->sig_length);
    std::copy(buf.begin()+INDEX_SIG,buf.end(),this->sig.begin());
    return INDEX_SIG+this->sig_length;
}