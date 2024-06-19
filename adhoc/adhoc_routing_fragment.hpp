#ifndef ADHOC_ROUTING_FRAGMENT_HPP
#define ADHOC_ROUTING_FRAGMENT_HPP


#define TIME_OUT 1000
#define DEFAULT_FRAGMENT_SIZE 10000
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
    arf_header(){}
    ~arf_header(){}
    void set_seq_no(std::uint8_t seq_no){this->seq_no=seq_no;}
	void set_fragment_id(std::uint8_t fragment_id){this->fragment_id=fragment_id;}
	void set_num_of_fratments(std::uint8_t num_of_fragments){this->num_of_fragments=num_of_fragments;}
	void set_total_length(std::uint32_t total_length){this->total_length=total_length;}
    void set_remote_address(array<std::uint8_t,ADDR_SIZE> &remote){std::copy(remote.begin(),remote.end(),this->remote_address.begin());}
    void set_arf_header(arf_header &arfh){
        this->seq_no=arfh.seq_no;this->fragment_id=arfh.fragment_id;
        this->num_of_fragments=arfh.num_of_fragments;this->total_length=arfh.total_length;
        std::copy(arfh.remote_address.begin(),arfh.remote_address.end(),this->remote_address.begin());}
	std::uint32_t get_seq_no(){return this->seq_no;}
	std::uint8_t get_fragment_id(){return this->fragment_id;}
	std::uint8_t get_num_of_fragments(){return this->num_of_fragments;}
	std::uint32_t get_total_length(){return this->total_length;}
    array<std::uint8_t,ADDR_SIZE>* get_remote_address(){return &(this->remote_address);}
	std::uint32_t header_length(){return SEQUENCE_NUMBER_SIZE+FRAGMENT_ID_SIZE+NUM_OF_FRAGMENTS_SIZE+TOTAL_LENGTH_SIZE+REMOTE_ADDRESS_SIZE;}
    virtual void serialize(vector<std::uint8_t> &buf);
	virtual void deserialize(const vector<std::uint8_t> &buf);
    string to_string();
};
class arf_packet:public arf_header{//adhoc routing fragment packet
    protected:
    std::uint32_t fragment_length;
    vector<std::uint8_t> payload;
    public:
    arf_packet(){}
    ~arf_packet(){}
    void set_arf_packet(arf_packet &arfp){
        this->set_arf_header(arfp);
        this->fragment_length=arfp.fragment_length;
        this->payload.clear();
        this->payload.resize(arfp.payload.size());
        std::copy(arfp.payload.begin(),arfp.payload.end(),this->payload.begin());
    }
    std::uint32_t get_fragment_length(){
        this->fragment_length=FRAGMENT_LENGTH_SIZE+this->payload.size()+this->header_length();
        return this->fragment_length;}
    void put_data(std::uint32_t index_begin, std::uint32_t index_end, const vector<std::uint8_t> &buf);
    int move_date(std::uint32_t index_begin, vector<std::uint8_t> &buf);
    virtual void serialize(vector<std::uint8_t> &buf);
	virtual void deserialize(const vector<std::uint8_t> &buf);
    string to_string();
};

class arf_portion:public arf_header{//adhoc routing fragment portion
    protected:
    chrono::seconds start;
    vector<arf_packet> storage;
    void is_timeout();
    public:
    arf_portion(){this->set_start();}
    ~arf_portion(){}
    int store_arf_packet(arf_packet &arfp);
    bool find_arf_packet_with_fragment_id(arf_packet& arfp);
    int storage_size(){return this->storage.size();}
    void set_start(){
        chrono::system_clock::time_point p = chrono::system_clock::now();
        this->start = chrono::duration_cast<chrono::seconds>(p.time_since_epoch());
    }
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
	arf_manager(){this->seq_no=0;this->fragment_size=0;};
	arf_manager(uint16_t size){this->seq_no=0;this->fragment_size=size;};
    ~arf_manager(){};
    void set_own_id(array<std::uint8_t,ADDR_SIZE> &id){std::copy(id.begin(),id.end(),this->own_id.begin());};
    void set_initial_seq_no(std::uint32_t seq_no){this->seq_no=seq_no;}
    void set_fragment_size(uint16_t size){this->fragment_size=size;};
    int store_arfp(arf_packet &arfp);
	void fragment(vector<arf_packet> &pkts,const vector<std::uint8_t> &buf);
    int defragment(arf_packet &arfp, vector<std::uint8_t> &buf);
    string to_string();
};
};
#endif