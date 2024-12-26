#ifndef DSR_ROUTING_HPP
#define DSR_ROUTING_HPP

#include <vector>
#include "dsr_packet.hpp"
#include "../adhoc/adhoc_routing.hpp"

namespace oit::ist::nws::adhoc_routing{

class dsr_routing:public adhoc_routing{
	public:
	dsr_routing(){}
	~dsr_routing(){}
	string routing_name(){return string("dsr");}
	virtual std::array<std::uint8_t,ADDR_SIZE>* processing_rreq(std::vector<std::uint8_t> &buf) override;
	virtual std::array<std::uint8_t,ADDR_SIZE>* processing_rrep(std::vector<std::uint8_t> &buf) override;
	virtual std::array<std::uint8_t,ADDR_SIZE>* processing_rerr(std::vector<std::uint8_t> &buf) override;
	virtual std::array<std::uint8_t,ADDR_SIZE>* processing_data(std::vector<std::uint8_t> &buf) override;
	virtual std::array<std::uint8_t,ADDR_SIZE>* generate_initial_request(std::array<std::uint8_t,ADDR_SIZE> dest, std::vector<std::uint8_t> &buf) override;
};
}
#endif

