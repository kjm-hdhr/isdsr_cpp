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
	virtual array<uint8_t,ADDR_SIZE>* processing_rreq(std::vector<uint8_t> &buf);
	virtual array<uint8_t,ADDR_SIZE>* processing_rrep(std::vector<uint8_t> &buf);
	virtual array<uint8_t,ADDR_SIZE>* processing_rerr(std::vector<uint8_t> &buf);
	virtual array<uint8_t,ADDR_SIZE>* processing_data(std::vector<uint8_t> &buf);
	virtual array<uint8_t,ADDR_SIZE>* generate_initiali_request(array<uint8_t,ADDR_SIZE> dest, std::vector<uint8_t> &buf);
};
}
#endif

