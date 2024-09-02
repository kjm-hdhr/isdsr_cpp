#include "isdsr_routing.hpp"
#include <algorithm>
#include <iostream>
using namespace oit::ist::nws::adhoc_routing;

void isdsr_routing::set_signature_scheme(signature_scheme *ss){
	this->ss=ss;
	this->ss->set_id(this->id);
	this->ss->setup();
	this->ss->key_derivation();
}
array<std::uint8_t,ADDR_SIZE>* isdsr_routing::processing_rreq(std::vector<std::uint8_t> &buf){
	isdsr_packet p;
    
	array<std::uint8_t,ADDR_SIZE>* next=nullptr;
	p.deserialize(buf);

	std::chrono::steady_clock::time_point vs=std::chrono::steady_clock::now();

    bool verification=this->ss->verify(p);

	std::chrono::steady_clock::time_point ve=std::chrono::steady_clock::now();
	this->t_verify+=((double)std::chrono::duration_cast<std::chrono::nanoseconds>(ve-vs).count());
	this->c_verify++;
	//this->time_verify.push_back(((double)std::chrono::duration_cast<std::chrono::nanoseconds>(ve-vs).count()));
	
	std::cerr<<"----- verification -----"<<std::to_string(verification)<<std::endl;
    if(!verification){
        return nullptr;
    }
	if(p.find_id(this->id)!=-1){
		std::cerr<<"already sent this packet"<<std::endl;
		return nullptr;
	}
	if(p.is_src(this->id)){
		std::cerr<<"the source node of this packet"<<std::endl;
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
	std::chrono::steady_clock::time_point ss=std::chrono::steady_clock::now();
    this->ss->sign(p);
	std::chrono::steady_clock::time_point se=std::chrono::steady_clock::now();
	this->t_sign+=((double)std::chrono::duration_cast<std::chrono::nanoseconds>(se-ss).count());
	this->c_sign++;
	//this->time_sign.push_back(((double)std::chrono::duration_cast<std::chrono::nanoseconds>(se-ss).count()));
	std::copy(next->begin(),next->end(),this->next.begin());
	p.set_next(this->next);
	p.serialize(buf);

	std::cerr<<"next ip:"<<adhoc_util::to_string_iparray(*next)<<std::endl;
	std::cout<<"---------- node id ----------"<<adhoc_util::to_string_iparray(this->id)<<std::endl;
    //double ave_verify=0;
	//double ave_sign=0;
	//for(int i=0;i<this->time_sign.size();i++){
	//	ave_sign+=this->time_sign.at(i);
	//}
	//for(int i=0;i<this->time_verify.size();i++){
	//	ave_verify+=this->time_verify.at(i);
	//}
	//std::cout<<" signing time:"<<std::to_string(ave_sign)<<"/"<<std::to_string(this->time_sign.size())<<"="<<std::to_string((ave_sign/this->time_sign.size()))<<std::endl;
	//std::cout<<" verification time:"<<std::to_string(ave_verify)<<"/"<<std::to_string(this->time_verify.size())<<"="<<std::to_string((ave_verify/this->time_verify.size()))<<std::endl;
	std::cout<<" signing time:"<<std::to_string(this->t_sign)<<"/"<<std::to_string(this->c_sign)<<"="<<std::to_string((this->t_sign/this->c_sign))<<std::endl;
	std::cout<<" verification time:"<<std::to_string(this->t_verify)<<"/"<<std::to_string(this->c_verify)<<"="<<std::to_string((this->t_verify/this->c_verify))<<std::endl;
	return &(this->next);
}
array<std::uint8_t,ADDR_SIZE>* isdsr_routing::processing_rrep(std::vector<std::uint8_t> &buf){
	isdsr_packet p;
    
	array<std::uint8_t,ADDR_SIZE>* next=nullptr;
	p.deserialize(buf);

	std::chrono::steady_clock::time_point vs=std::chrono::steady_clock::now();

    bool verification=this->ss->verify(p);

	std::chrono::steady_clock::time_point ve=std::chrono::steady_clock::now();
	this->t_verify+=((double)std::chrono::duration_cast<std::chrono::nanoseconds>(ve-vs).count());
	this->c_verify++;
	//this->time_verify.push_back(((double)std::chrono::duration_cast<std::chrono::nanoseconds>(ve-vs).count()));
	std::cout<<" signing time:"<<std::to_string(this->t_sign)<<"/"<<std::to_string(this->c_sign)<<"="<<std::to_string((this->t_sign/this->c_sign))<<std::endl;
	std::cout<<" verification time:"<<std::to_string(this->t_verify)<<"/"<<std::to_string(this->c_verify)<<"="<<std::to_string((this->t_verify/this->c_verify))<<std::endl;
	
	std::cerr<<"----- verification -----"<<std::to_string(verification)<<std::endl;
    if(!verification){
        return nullptr;
    }
	//std::cerr<<"receive reply:"<<p.to_string()<<std::endl;
	if(p.find_id(id)==-1){
		return nullptr;
	}
	if(p.is_src(id)){
		return nullptr;
	}
	if(p.is_dest(id)){
		std::cout<<"route established processing rrep"<<std::endl;
		std::cout<<"return address:"<<adhoc_util::to_string_iparray(this->id)<<std::endl;
		return &(this->id);
	}
	next=p.previous_id(id);
	std::copy(next->begin(),next->end(),this->next.begin());
	p.set_next(this->next);
	return &(this->next);
}
array<std::uint8_t,ADDR_SIZE>* isdsr_routing::processing_rerr(std::vector<std::uint8_t> &buf){
	return nullptr;
}
array<std::uint8_t,ADDR_SIZE>* isdsr_routing::processing_data(std::vector<std::uint8_t> &buf){
	return nullptr;
}
array<std::uint8_t,ADDR_SIZE>* isdsr_routing::generate_initiali_request(array<std::uint8_t,ADDR_SIZE> dest, std::vector<std::uint8_t> &buf){
	isdsr_packet p(RREQ,id,dest);
	this->seq++;
	p.set_seq(this->seq);
	p.add_id(this->id);
	std::cerr<<"routing seq:"<<std::to_string(this->seq)<<std::endl;
	std::cerr<<this->to_string()<<std::endl;
	std::cerr<<"isdsr initial req"<<std::endl;
	//std::cerr<<p.dsr_packet::to_string()<<std::endl;
	std::chrono::steady_clock::time_point ss=std::chrono::steady_clock::now();
    this->ss->sign(p);
	std::chrono::steady_clock::time_point se=std::chrono::steady_clock::now();
	this->t_sign+=((double)std::chrono::duration_cast<std::chrono::nanoseconds>(se-ss).count());
	this->c_sign++;
	std::cerr<<"isdsr initial req generate signature"<<std::endl;
	//bool v=this->ss->verify(p);
	//std::cerr<<"isdsr initial req generate signature verify "<<std::to_string(v)<<std::endl;
	p.serialize(buf);
	//std::cerr<<"isdsr_routing initial request: "<<p.to_string()<<std::endl;
	//std::cerr<<"isdsr_routing initial request buf["<<std::to_string(buf.at(0));
	//for(size_t i=1;i<buf.size();i++){
	//	std::cerr<<","<<std::to_string(buf.at(i));
	//}
	//std::cerr<<"]"<<std::endl;
	//std::cerr<<"isdsr_packet addr:"<<&p<<std::endl;
	return &(this->broadcast);
}

