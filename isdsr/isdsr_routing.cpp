#include "isdsr_routing.hpp"
#include <algorithm>
#include <iostream>
using namespace oit::ist::nws::adhoc_routing;

array<uint8_t,ADDR_SIZE>* isdsr_routing::processing_rreq(std::vector<uint8_t> &buf){
	isdsr_packet p;
    
	array<uint8_t,ADDR_SIZE>* next=nullptr;
	p.deserialize(buf);

    bool verification=this->ss->verify(p);
    if(!verification){
        return nullptr;
    }
	if(p.find_id(this->id)!=-1){
		return nullptr;
	}
	if(p.is_src(this->id)){
		return nullptr;
	}
	std::cerr<<"req packet:"<<p.to_string()<<std::endl;
	p.add_id(this->id);
	std::cerr<<"req packet:"<<p.to_string()<<std::endl;
	next=&(this->broadcast);
	if(p.is_dest(id)){
		p.set_type(RREP);
		p.swap_src_dest();
		next=p.previous_id(id);
	}
    this->ss->sign(p);
	std::copy(next->begin(),next->end(),this->next.begin());
	p.set_next(this->next);
	p.serialize(buf);

	std::cerr<<"next ip:"<<adhoc_util::to_string_iparray(*next)<<std::endl;
    
	return &(this->next);
}
array<uint8_t,ADDR_SIZE>* isdsr_routing::processing_rrep(std::vector<uint8_t> &buf){
	dsr_packet p;
	array<uint8_t,ADDR_SIZE>* next=nullptr;
	p.deserialize(buf);
	std::cerr<<"receive reply:"<<p.to_string()<<std::endl;
	if(p.find_id(id)==-1){
		return nullptr;
	}
	if(p.is_src(id)){
		return nullptr;
	}
	if(p.is_dest(id)){
		std::cout<<"route established"<<std::endl;
		return nullptr;
	}
	next=p.previous_id(id);
	std::copy(next->begin(),next->end(),this->next.begin());
	p.set_next(this->next);
	return &(this->next);
}
array<uint8_t,ADDR_SIZE>* isdsr_routing::processing_rerr(std::vector<uint8_t> &buf){
	return nullptr;
}
array<uint8_t,ADDR_SIZE>* isdsr_routing::processing_data(std::vector<uint8_t> &buf){
	return nullptr;
}
array<uint8_t,ADDR_SIZE>* isdsr_routing::generate_initiali_request(array<uint8_t,ADDR_SIZE> dest, std::vector<uint8_t> &buf){
	dsr_packet p(RREQ,id,dest);
	p.add_id(id);
	p.serialize(buf);
	return &(this->broadcast);
}

