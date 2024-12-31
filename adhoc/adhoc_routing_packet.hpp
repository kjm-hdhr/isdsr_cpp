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
	std::uint32_t seq;
	std::uint8_t type;
	uint32_t packet_length;
	array<std::uint8_t,ADDR_SIZE> src;
	array<std::uint8_t,ADDR_SIZE> dest;
	array<std::uint8_t,ADDR_SIZE> next;
	public:
	ar_packet();
	ar_packet(std::uint8_t type, array<std::uint8_t,ADDR_SIZE> &src_id, array<std::uint8_t,ADDR_SIZE> &dest_id);
	~ar_packet();
	void set_seq(std::uint32_t seq);
	void set_type(std::uint8_t type);
	void set_src(array<std::uint8_t,ADDR_SIZE> &id);
	void set_dest(array<std::uint8_t,ADDR_SIZE> &id);
	void set_next(array<std::uint8_t,ADDR_SIZE> &id);
	std::uint32_t get_seq();
	std::uint8_t get_type();
	std::uint8_t get_type_from_byte_array(vector<std::uint8_t> &buf);
	array<std::uint8_t,ADDR_SIZE>* get_src();
	array<std::uint8_t,ADDR_SIZE>* get_dest();
	array<std::uint8_t,ADDR_SIZE>* get_next();
	bool is_src(array<std::uint8_t,ADDR_SIZE> &id);
	bool is_dest(array<std::uint8_t,ADDR_SIZE> &id);
	void swap_src_dest();
	virtual uint32_t packet_size();
	virtual void initialize();
	virtual uint32_t serialize(vector<std::uint8_t> &buf);
	virtual uint32_t deserialize(const vector<std::uint8_t> &buf);
	virtual string to_string();
};
};
#endif