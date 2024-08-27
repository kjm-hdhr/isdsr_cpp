#ifndef ADHOC_ID_UTIL_HPP
#define ADHOC_ID_UTIL_HPP

#include <string>
#include <cstdint>
#include "../adhoc/adhoc_routing_constants.hpp"

namespace oit::ist::nws::adhoc_routing{

    class id_util{
        public:
        static std::string id_to_string(std::uint8_t* id){
            std::string ret="["+std::to_string(id[0]);
            for(int i=1;i<ADDR_SIZE;i++){
                ret+=","+std::to_string(id[i]);
            }
            return ret+"]";
        }
    };
};
#endif