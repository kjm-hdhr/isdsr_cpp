#ifndef DSR_PACKET_HPP
#define DSR_PACKET_HPP
#include "../adhoc/adhoc_routing_packet.hpp"
#include "../adhoc/adhoc_util.hpp"

#define RI_LENGTH_SIZE 1
#define INDEX_RI_LENGTH INDEX_DEST+ADDR_SIZE
#define INDEX_RI INDEX_RI_LENGTH+RI_LENGTH_SIZE

namespace oit::ist::nws::adhoc_routing{


class dsr_packet:public ar_packet{

    protected:
    std::uint8_t ri_length;
    std::vector<array<std::uint8_t,ADDR_SIZE>> ri;
    public:
    dsr_packet();
    dsr_packet(std::uint8_t type, std::array<std::uint8_t,ADDR_SIZE> &src_id, std::array<std::uint8_t,ADDR_SIZE> &dest_id);
    ~dsr_packet();
    void add_id(std::array<std::uint8_t,ADDR_SIZE> &id);
    int find_id(std::array<std::uint8_t,ADDR_SIZE> &id);//return an index of id
    std::array<std::uint8_t,ADDR_SIZE>* previous_id(std::array<std::uint8_t,ADDR_SIZE> &id);//return a previous id of own id
    int get_ri_length(){this->ri_length=ri.size();return this->ri_length;}
    std::vector<array<std::uint8_t,ADDR_SIZE>>* get_ri(){return &(this->ri);}
    virtual std::uint32_t packet_size();
	virtual void initialize();
	virtual std::uint32_t serialize(std::vector<std::uint8_t> &buf);
	virtual std::uint32_t deserialize(const std::vector<std::uint8_t> &buf);
	virtual string to_string();
};
};

#endif