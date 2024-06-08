#include "isdsr_packet.hpp"
#include <iostream>
using namespace oit::ist::nws::adhoc_routing;

isdsr_packet::isdsr_packet(){
    this->initialize();
}
isdsr_packet::isdsr_packet(uint8_t type, array<uint8_t,ADDR_SIZE> &src_id, array<uint8_t,ADDR_SIZE> &dest_id):dsr_packet(type,src_id,dest_id){
    this->sig_length=0;
    this->sig.clear();
}
isdsr_packet::~isdsr_packet(){
    std::cerr<<"sig buf addr:"<<&(this->sig)<<std::endl;
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
    std::cerr<<"serialize orignal buf size:"<<std::to_string(buf.size())<<std::endl;
    if(buf.size()<this->packet_size()){
        buf.resize(this->packet_size());
    }
    std::cerr<<"serialize update buf size1:"<<std::to_string(buf.size())<<std::endl;
    this->dsr_packet::serialize(buf);
    std::cerr<<"serialize update buf size2:"<<std::to_string(buf.size())<<std::endl;
    for(size_t i=0;i<buf.size();i++){
        std::cerr<<","<<std::to_string(buf[i]);
    }
    std::cerr<<std::endl;
    std::cerr<<"packet size:"<<std::to_string(this->packet_size())<<std::endl;
    std::cerr<<"buf size:"<<std::to_string(buf.size())<<std::endl;
    //std::cerr<<"isdsr_packet serialize dsr"<<adhoc_util::to_string_vector(buf)<<std::endl;
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
    std::cerr<<"isdsr_packet deserialize:"<<this->to_string()<<std::endl;
    return INDEX_SIG+this->sig_length;
}
string isdsr_packet::to_string(){
    string ret=this->dsr_packet::to_string();
    ret+=" sig length:"+std::to_string(this->sig_length);
    ret+=" sig["+std::to_string(this->sig[0]);
    for(size_t i=1;i<this->sig.size();i++){
        ret+=","+std::to_string(this->sig[i]);
    }
    ret+="]";
    return ret;
}