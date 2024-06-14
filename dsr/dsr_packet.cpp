#include "dsr_packet.hpp"
#include <iostream>
using namespace oit::ist::nws::adhoc_routing;

dsr_packet::dsr_packet(){
    this->initialize();
}
dsr_packet::dsr_packet(std::uint8_t type, array<std::uint8_t,ADDR_SIZE> &src_id, array<std::uint8_t,ADDR_SIZE> &dest_id):ar_packet(type,src_id,dest_id){
    this->ri_length=0;
    this->ri.clear();
    //std::cerr<<"dsr packet constructor "<<this->to_string()<<std::endl;
}
dsr_packet::~dsr_packet(){
}
void dsr_packet::add_id(array<std::uint8_t,ADDR_SIZE> &id){
    this->ri.push_back(id);
    this->ri_length=this->ri.size();
}
int dsr_packet::find_id(array<std::uint8_t,ADDR_SIZE> &id){
    for(size_t i=0;i<this->ri.size();i++){
        if(std::equal(ri.at(i).begin(),ri.at(i).end(),id.begin(),id.end())){
            return i;
        }
    }
    return -1;
}
array<std::uint8_t,ADDR_SIZE>* dsr_packet::previous_id(array<std::uint8_t,ADDR_SIZE> &id){
    //return a previous id of own id
    int index=this->find_id(id);
    if(index==-1){
        return nullptr;
    }
    if(index==0){
        return nullptr;
    }
    return &(this->ri.at(index-1));
}
std::uint32_t dsr_packet::packet_size(){
    std::uint32_t ret=this->ar_packet::packet_size();
    this->ri_length=this->ri.size();
    ret+=RI_LENGTH_SIZE;
    ret+=this->ri_length*ADDR_SIZE;
    return ret;
}
void dsr_packet::initialize(){
    this->ar_packet::initialize();
    this->ri_length=0;
    this->ri.clear();
}
std::uint32_t dsr_packet::serialize(vector<std::uint8_t> &buf){
    if(buf.size()<this->packet_size()){
        buf.resize(this->packet_size());
    }
    this->ar_packet::serialize(buf);
    this->ri_length=this->ri.size();
    buf[INDEX_RI_LENGTH]=this->ri_length;
    for(size_t i=0;i<this->ri.size();i++){
        std::copy(this->ri.at(i).begin(),this->ri.at(i).end(),buf.begin()+INDEX_RI+ADDR_SIZE*i);
    }
    return INDEX_RI+this->ri_length*ADDR_SIZE;
}
std::uint32_t dsr_packet::deserialize(const vector<std::uint8_t> &buf){
    this->ar_packet::deserialize(buf);
    this->ri_length=buf[INDEX_RI_LENGTH];
    this->ri.resize(this->ri_length);
    for(int i=0;i<this->ri_length;i++){
        array<std::uint8_t,ADDR_SIZE> tmp;
        int index_begin=INDEX_RI+(i*ADDR_SIZE);
        int index_end=INDEX_RI+(i*ADDR_SIZE)+ADDR_SIZE;
        std::copy(buf.begin()+index_begin,buf.begin()+index_end,tmp.begin());
        this->ri[i]=tmp;
    }
    return INDEX_RI+this->ri_length*ADDR_SIZE;
}
string dsr_packet::to_string(){
    string ret=this->ar_packet::to_string();
    ret+=" ri length: "+std::to_string(this->ri_length);
    ret+=" ri[";
    for(size_t i=0;i<ri.size();i++){
        ret+=adhoc_util::to_string_iparray(this->ri.at(i));
    }
    return ret+"]";
}