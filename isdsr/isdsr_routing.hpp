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
	isdsr_routing();
	~isdsr_routing();
	string routing_name();
    void set_signature_scheme(signature_scheme *ss);
	virtual void set_id(std::array<std::uint8_t,ADDR_SIZE> &id) override;
	virtual std::array<std::uint8_t,ADDR_SIZE>* processing_rreq(std::vector<std::uint8_t> &buf) override;
	virtual std::array<std::uint8_t,ADDR_SIZE>* processing_rrep(std::vector<std::uint8_t> &buf) override;
	virtual std::array<std::uint8_t,ADDR_SIZE>* processing_rerr(std::vector<std::uint8_t> &buf) override;
	virtual std::array<std::uint8_t,ADDR_SIZE>* processing_data(std::vector<std::uint8_t> &buf) override;
	virtual std::array<std::uint8_t,ADDR_SIZE>* generate_initial_request(std::array<std::uint8_t,ADDR_SIZE> dest, std::vector<std::uint8_t> &buf) override;
};
}
#endif

