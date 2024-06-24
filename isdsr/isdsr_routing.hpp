#ifndef ISDSR_ROUTING_HPP
#define ISDSR_ROUTING_HPP

#include <vector>
#include <chrono>
#include "../dsr/dsr_routing.hpp"
#include "isdsr_packet.hpp"
#include "signature_scheme.hpp"

namespace oit::ist::nws::adhoc_routing{

class isdsr_routing:public dsr_routing{
	
    protected:
    signature_scheme *ss;
	std::vector<double> time_verify;
	std::vector<double> time_sign;
	double t_verify;
	double t_sign;
	int c_verify;
	int c_sign;
	public:
	isdsr_routing(){this->t_sign=0;this->t_verify=0;this->c_sign=0;this->c_verify=0;}
	~isdsr_routing(){}
	string routing_name(){return string("isdsr")+" "+ss->signature_scheme_name();}
    void set_signature_scheme(signature_scheme *ss);
	virtual array<std::uint8_t,ADDR_SIZE>* processing_rreq(std::vector<std::uint8_t> &buf);
	virtual array<std::uint8_t,ADDR_SIZE>* processing_rrep(std::vector<std::uint8_t> &buf);
	virtual array<std::uint8_t,ADDR_SIZE>* processing_rerr(std::vector<std::uint8_t> &buf);
	virtual array<std::uint8_t,ADDR_SIZE>* processing_data(std::vector<std::uint8_t> &buf);
	virtual array<std::uint8_t,ADDR_SIZE>* generate_initiali_request(array<std::uint8_t,ADDR_SIZE> dest, std::vector<std::uint8_t> &buf);
};
}
#endif

