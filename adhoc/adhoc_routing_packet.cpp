#include "adhoc_routing_packet.hpp"
#include <algorithm>
#include <iostream>
using namespace oit::ist::nws::adhoc_routing;

ar_packet::ar_packet(){
	this->initialize();
}
ar_packet::ar_packet(uint8_t type, array<uint8_t,ADDR_SIZE> &src_id, array<uint8_t,ADDR_SIZE> &dest_id):ar_packet(){
	//std::cerr<<"ar packet constructor"<<std::endl;
	this->type=type;
	this->set_src(src_id);
	this->set_dest(dest_id);
	//std::cerr<<"ar pakcet "<<this->to_string()<<std::endl;
}
ar_packet::~ar_packet(){

}

uint32_t ar_packet::serialize(vector<uint8_t> &buf){
	this->packet_length=this->packet_size();

	if(buf.size()<this->packet_size()){
		buf.resize(this->packet_size());
	}
    buf[INDEX_TYPE]=this->type;
    for(int i=0;i<PACKET_LENGTH_SIZE;i++){
        buf[INDEX_PACKET_LENGTH+i]=(this->packet_length>>(8*i))&0xFF;
    }
    std::copy(this->src.begin(),this->src.end(),buf.begin()+INDEX_SRC);
    std::copy(this->dest.begin(),this->dest.end(),buf.begin()+INDEX_DEST);
	return INDEX_DEST+ADDR_SIZE;
}
uint32_t ar_packet::deserialize(const vector<uint8_t> &buf){
    this->type=buf[INDEX_TYPE];
    this->packet_length=0;
    for(int i=0;i<PACKET_LENGTH_SIZE;i++){
        this->packet_length+=(((uint32_t)buf[INDEX_PACKET_LENGTH+i])&0x000000FF)<<(8*i);
    }
    std::copy(buf.begin()+INDEX_SRC,buf.begin()+INDEX_SRC+ADDR_SIZE,this->src.begin());
    std::copy(buf.begin()+INDEX_DEST,buf.begin()+INDEX_DEST+ADDR_SIZE,this->dest.begin());
	return INDEX_DEST+ADDR_SIZE;
}

std::string ar_packet::to_string(){
	this->packet_length=this->packet_size();
	string ret="type:"+std::to_string(this->type);
	ret+=" packet length:"+std::to_string(this->packet_length);
	ret+=" src"+adhoc_util::to_string_iparray(this->src);
	ret+=" dest"+adhoc_util::to_string_iparray(this->dest);
	return ret;
}