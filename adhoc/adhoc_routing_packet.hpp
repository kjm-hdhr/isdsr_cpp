#ifndef ADHOC_ROUTING_PACKET_HPP
#define ADHOC_ROUTING_PACKET_HPP


#include <cstdint>
#include <vector>
#include <string>
#include <array>
#include <algorithm>
#include "adhoc_routing_constants.hpp"
#include "adhoc_util.hpp"
namespace oit::ist::nws::adhoc_routing{

using namespace std;
class ar_packet{
    protected:
	uint8_t type;
	uint32_t packet_length;
	array<uint8_t,ADDR_SIZE> src;
	array<uint8_t,ADDR_SIZE> dest;
	array<uint8_t,ADDR_SIZE> next;
	public:
	ar_packet();
	ar_packet(uint8_t type, array<uint8_t,ADDR_SIZE> &src_id, array<uint8_t,ADDR_SIZE> &dest_id);
	~ar_packet();
	void set_type(uint8_t type){this->type=type;}
	void set_src(array<uint8_t,ADDR_SIZE> &id){std::copy(id.begin(),id.end(),this->src.begin());}
	void set_dest(array<uint8_t,ADDR_SIZE> &id){std::copy(id.begin(),id.end(),this->dest.begin());}
	void set_next(array<uint8_t,ADDR_SIZE> &id){std::copy(id.begin(),id.end(),this->next.begin());}
	uint8_t get_type(){return this->type;}
	uint8_t get_type_from_byte_array(vector<uint8_t> &buf){return buf[INDEX_TYPE];}
	array<uint8_t,ADDR_SIZE>* get_src(){return &(this->src);}
	array<uint8_t,ADDR_SIZE>* get_dest(){return &(this->dest);}
	array<uint8_t,ADDR_SIZE>* get_next(){return &(this->next);}
	bool is_src(array<uint8_t,ADDR_SIZE> &id){return std::equal(id.begin(),id.end(),this->src.begin(),this->src.end());}
	bool is_dest(array<uint8_t,ADDR_SIZE> &id){return std::equal(id.begin(),id.end(),this->dest.begin(),this->dest.end());}
	void swap_src_dest(){array<uint8_t,ADDR_SIZE> tmp;
        std::copy(this->src.begin(),this->src.end(),tmp.begin());
        std::copy(this->dest.begin(),this->dest.end(),this->src.begin());
        std::copy(tmp.begin(),tmp.end(),this->dest.begin());}
	virtual uint32_t packet_size(){return TYPE_SIZE+PACKET_LENGTH_SIZE+ADDR_SIZE+ADDR_SIZE;}
	virtual void initialize(){this->type=0;
        this->packet_length=0;
        this->src.fill(0);
        this->dest.fill(0);}
	virtual uint32_t serialize(vector<uint8_t> &buf);
	virtual uint32_t deserialize(const vector<uint8_t> &buf);
	virtual string to_string();
};
};
#endif