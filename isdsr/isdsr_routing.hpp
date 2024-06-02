#ifndef ISDSR_ROUTING_HPP
#define ISDSR_ROUTING_HPP

#include <vector>
#include "../dsr/dsr_routing.hpp"
#include "isdsr_packet.hpp"
#include "signature_scheme.hpp"

namespace oit::ist::nws::adhoc_routing{

class isdsr_routing:public dsr_routing{
	
    protected:
    signature_scheme *ss;
	public:
	isdsr_routing(){}
	~isdsr_routing(){}
	string routing_name(){return string("isdsr")+" "+ss->signature_scheme_name();}
    void set_signature_scheme(signature_scheme *ss){this->ss=ss;}
	virtual array<uint8_t,ADDR_SIZE>* processing_rreq(std::vector<uint8_t> &buf);
	virtual array<uint8_t,ADDR_SIZE>* processing_rrep(std::vector<uint8_t> &buf);
	virtual array<uint8_t,ADDR_SIZE>* processing_rerr(std::vector<uint8_t> &buf);
	virtual array<uint8_t,ADDR_SIZE>* processing_data(std::vector<uint8_t> &buf);
	virtual array<uint8_t,ADDR_SIZE>* generate_initiali_request(array<uint8_t,ADDR_SIZE> dest, std::vector<uint8_t> &buf);
};
}
#endif

