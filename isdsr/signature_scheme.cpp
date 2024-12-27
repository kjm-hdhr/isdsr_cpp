#include "signature_scheme.hpp"
using namespace oit::ist::nws::adhoc_routing;

signature_scheme::signature_scheme(){}
signature_scheme::signature_scheme(std::array<uint8_t, ADDR_SIZE> &id){
    this->set_id(id);
};
void signature_scheme::set_id(array<uint8_t, ADDR_SIZE> &id){
    std::copy(id.begin(),id.end(),this->id.begin());
}