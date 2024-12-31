#ifndef ADHOC_ROUTING_FRAGMENT_HPP
#define ADHOC_ROUTING_FRAGMENT_HPP


#define TIME_OUT 1000
#define DEFAULT_FRAGMENT_SIZE 20000
#include <cstdint>
#include <vector>
#include <string>
#include <array>
#include <chrono>
#include "adhoc_routing_constants.hpp"
#include "adhoc_util.hpp"

namespace oit::ist::nws::adhoc_routing{

using namespace std;

class arf_header{//adhoc routing fragment header
    protected:
	std::uint32_t seq_no;
	std::uint8_t fragment_id;
	std::uint8_t num_of_fragments;
    std::uint32_t total_length;
    array<std::uint8_t,ADDR_SIZE> remote_address;
    public:
    arf_header();
    ~arf_header();
    void set_seq_no(std::uint8_t seq_no);
	void set_fragment_id(std::uint8_t fragment_id);
	void set_num_of_fratments(std::uint8_t num_of_fragments);
	void set_total_length(std::uint32_t total_length);
    void set_remote_address(array<std::uint8_t,ADDR_SIZE> &remote);
    void set_arf_header(arf_header &arfh);
	std::uint32_t get_seq_no();
	std::uint8_t get_fragment_id();
	std::uint8_t get_num_of_fragments();
	std::uint32_t get_total_length();
    array<std::uint8_t,ADDR_SIZE>* get_remote_address();
	std::uint32_t header_length();
    virtual void serialize(vector<std::uint8_t> &buf);
	virtual void deserialize(const vector<std::uint8_t> &buf);
    string to_string();
};
class arf_packet:public arf_header{//adhoc routing fragment packet
    protected:
    std::uint32_t fragment_length;
    vector<std::uint8_t> payload;
    public:
    arf_packet();
    ~arf_packet();
    void set_arf_packet(arf_packet &arfp);
    std::uint32_t get_fragment_length();
    void put_data(std::uint32_t index_begin, std::uint32_t index_end, const vector<std::uint8_t> &buf);
    int move_date(std::uint32_t index_begin, vector<std::uint8_t> &buf);
    virtual void serialize(vector<std::uint8_t> &buf) override;
	virtual void deserialize(const vector<std::uint8_t> &buf) override;
    string to_string();
};

class arf_portion:public arf_header{//adhoc routing fragment portion
    protected:
    chrono::seconds start;
    vector<arf_packet> storage;
    //void is_timeout();
    public:
    arf_portion();
    ~arf_portion();
    int store_arf_packet(arf_packet &arfp);
    bool find_arf_packet_with_fragment_id(arf_packet& arfp);
    int storage_size();
    void set_start();
    void sort_arfp();
    std::uint32_t combine_arfp(vector<std::uint8_t> &buf);
    string to_string();
};

class arf_manager{
    protected:
    std::uint32_t seq_no;
    uint16_t fragment_size;
    array<std::uint8_t,ADDR_SIZE> own_id;
    vector<arf_portion> storage;

	public:
	arf_manager();
	arf_manager(uint16_t size);
    ~arf_manager();
    void set_own_id(array<std::uint8_t,ADDR_SIZE> &id);
    void set_initial_seq_no(std::uint32_t seq_no);
    void set_fragment_size(uint16_t size);
    int store_arfp(arf_packet &arfp);
	void fragment(vector<arf_packet> &pkts,const vector<std::uint8_t> &buf);
    int defragment(arf_packet &arfp, vector<std::uint8_t> &buf);
    string to_string();
};
};
#endif