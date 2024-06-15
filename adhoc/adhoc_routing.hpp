#ifndef ADHOC_ROUTING_HPP
#define ADHOC_ROUTING_HPP

#include "adhoc_routing_packet.hpp"
namespace oit::ist::nws::adhoc_routing{

class adhoc_routing{
	protected:
    std::uint32_t seq;
    std::array<std::uint8_t,ADDR_SIZE> id;
    std::array<std::uint8_t,ADDR_SIZE> next;
    std::array<std::uint8_t,ADDR_SIZE> broadcast;

	public:
	adhoc_routing(){this->seq=0;}
	~adhoc_routing(){}
	void set_id(std::array<std::uint8_t,ADDR_SIZE> &id){
        std::copy(id.begin(),id.end(),this->id.begin());
    }
	void set_next(std::array<std::uint8_t,ADDR_SIZE> &id){
        std::copy(id.begin(),id.end(),this->next.begin());
    }
	void set_broadcast(std::array<std::uint8_t,ADDR_SIZE> &id){
        std::copy(id.begin(),id.end(),this->broadcast.begin());
    }
	std::array<std::uint8_t,ADDR_SIZE>* get_id(){return &(this->id);}
	std::array<std::uint8_t,ADDR_SIZE>* get_next(){return &(this->next);}
	std::array<std::uint8_t,ADDR_SIZE>* get_broadcast(){return &(this->broadcast);}
    std::string to_string(){
        string ret="routing name"+this->routing_name();
        ret+=" id:"+adhoc_util::to_string_iparray(this->id);
        ret+=" brd:"+adhoc_util::to_string_iparray(this->broadcast);
        return ret;
    }

	virtual std::array<std::uint8_t,ADDR_SIZE>* packet_processing(std::vector<std::uint8_t> &buf){
        
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
	virtual std::array<std::uint8_t,ADDR_SIZE>* processing_rreq(std::vector<std::uint8_t> &buf)=0;
	virtual std::array<std::uint8_t,ADDR_SIZE>* processing_rrep(std::vector<std::uint8_t> &buf)=0;
	virtual std::array<std::uint8_t,ADDR_SIZE>* processing_rerr(std::vector<std::uint8_t> &buf)=0;
	virtual std::array<std::uint8_t,ADDR_SIZE>* processing_data(std::vector<std::uint8_t> &buf)=0;
	virtual std::array<std::uint8_t,ADDR_SIZE>* generate_initiali_request(std::array<std::uint8_t,ADDR_SIZE> dest, std::vector<std::uint8_t> &buf)=0;
    virtual std::string routing_name()=0;
};
}
#endif