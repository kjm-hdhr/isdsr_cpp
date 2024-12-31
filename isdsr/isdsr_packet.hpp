#ifndef ISDSR_PACKET_HPP
#define ISDSR_PACKET_HPP
#include "../dsr/dsr_packet.hpp"

#define SIG_LENGTH_SIZE 4
#define SIG_SIZE this->sig.size()

#define INDEX_SIG_LENGTH INDEX_RI+this->ri.size()*ADDR_SIZE
#define INDEX_SIG INDEX_SIG_LENGTH+SIG_LENGTH_SIZE
namespace oit::ist::nws::adhoc_routing{

class isdsr_packet : public dsr_packet{
	protected:
	std::uint32_t sig_length;
	vector<std::uint8_t> sig;
	public:
	isdsr_packet();
    isdsr_packet(std::uint8_t type, array<std::uint8_t,ADDR_SIZE> &src_id, array<std::uint8_t,ADDR_SIZE> &dest_id);
	~isdsr_packet();
	vector<std::uint8_t>* get_sig();
	std::uint32_t get_sig_length();
	void set_sig(vector<std::uint8_t> &sig);
	virtual std::uint32_t packet_size() override;
	virtual void initialize() override;
	virtual std::uint32_t serialize(vector<std::uint8_t> &buf) override;
	virtual std::uint32_t deserialize(const vector<std::uint8_t> &buf) override;
	virtual string to_string() override;
};
};
#endif