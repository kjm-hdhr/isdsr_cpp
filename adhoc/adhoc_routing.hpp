#ifndef ADHOC_ROUTING_HPP
#define ADHOC_ROUTING_HPP

#include "adhoc_routing_packet.hpp"
namespace oit::ist::nws::adhoc_routing{

class adhoc_routing{
	protected:
    array<uint8_t,ADDR_SIZE> id;
    array<uint8_t,ADDR_SIZE> next;
    array<uint8_t,ADDR_SIZE> broadcast;

	public:
	adhoc_routing(){}
	~adhoc_routing(){}
	void set_id(array<uint8_t,ADDR_SIZE> &id){
        std::copy(id.begin(),id.end(),this->id.begin());
    }
	void set_next(array<uint8_t,ADDR_SIZE> &id){
        std::copy(id.begin(),id.end(),this->next.begin());
    }
	void set_broadcast(array<uint8_t,ADDR_SIZE> &id){
        std::copy(id.begin(),id.end(),this->broadcast.begin());
    }
	array<uint8_t,ADDR_SIZE>* get_id(){return &(this->id);}
	array<uint8_t,ADDR_SIZE>* get_next(){return &(this->next);}
	array<uint8_t,ADDR_SIZE>* get_broadcast(){return &(this->broadcast);}
    string to_string(){
        string ret="routing name"+this->routing_name();
        ret+=" id:"+adhoc_util::to_string_iparray(this->id);
        ret+=" brd:"+adhoc_util::to_string_iparray(this->broadcast);
        return ret;
    }

	virtual array<uint8_t,ADDR_SIZE>* packet_processing(std::vector<uint8_t> &buf){
        
        switch(buf[INDEX_TYPE]){
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
	virtual array<uint8_t,ADDR_SIZE>* processing_rreq(std::vector<uint8_t> &buf)=0;
	virtual array<uint8_t,ADDR_SIZE>* processing_rrep(std::vector<uint8_t> &buf)=0;
	virtual array<uint8_t,ADDR_SIZE>* processing_rerr(std::vector<uint8_t> &buf)=0;
	virtual array<uint8_t,ADDR_SIZE>* processing_data(std::vector<uint8_t> &buf)=0;
	virtual array<uint8_t,ADDR_SIZE>* generate_initiali_request(array<uint8_t,ADDR_SIZE> dest, std::vector<uint8_t> &buf)=0;
    virtual string routing_name()=0;
};
}
#endif