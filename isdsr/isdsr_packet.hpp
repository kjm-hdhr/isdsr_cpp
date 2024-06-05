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
	uint32_t sig_length;
	vector<uint8_t> sig;
	public:
	isdsr_packet();
    isdsr_packet(uint8_t type, array<uint8_t,ADDR_SIZE> &src_id, array<uint8_t,ADDR_SIZE> &dest_id);
	~isdsr_packet();
	vector<uint8_t>* get_sig(){return &(this->sig);}
	uint32_t get_sig_length(){this->sig_length=this->sig.size();return this->sig_length;}
	void set_sig(vector<uint8_t> &sig){this->sig.resize(sig.size());std::copy(sig.begin(),sig.end(),this->sig.begin());};
	virtual uint32_t packet_size();
	virtual void initialize();
	virtual uint32_t serialize(vector<uint8_t> &buf);
	virtual uint32_t deserialize(const vector<uint8_t> &buf);
	virtual string to_string();
};
};
#endif