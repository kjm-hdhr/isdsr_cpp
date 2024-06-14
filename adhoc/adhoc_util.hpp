#ifndef ADHOC_UTIL_HPP
#define ADHOC_UTIL_HPP

#include "adhoc_routing_constants.hpp"
#include <cstdint>
#include <vector>
#include <array>
#include <iostream>
#define LENGTH_UINT32 4
#define LENGTH_UINT16 2


namespace oit::ist::nws::adhoc_routing{
class adhoc_util{
    public:
    static void ip_to_array(const std::uint32_t ip_addr, std::array<std::uint8_t,ADDR_SIZE> &ar){
        for(int i=0;i<ADDR_SIZE;i++){
            ar[i]=(ip_addr>>(8*i))&0xFF;
        }
    };
    static std::uint32_t array_to_ip(const std::array<std::uint8_t,ADDR_SIZE> &ar){
        std::uint32_t ret=0;
        for(int i=0;i<ADDR_SIZE;i++){
            ret+=(((std::uint32_t)ar[i])&0x000000FF)<<(8*i);
        }
        return ret;
    };
    static void serialize_uint32(std::uint32_t index, std::vector<std::uint8_t> &buf, const std::uint32_t value){
        for(int i=0;i<LENGTH_UINT32;i++){
            buf.at(index+i)=(value>>(8*i))&0xFF;
        }
    };
    static void serialize_uint16(std::uint32_t index, std::vector<std::uint8_t> &buf, const std::uint16_t value){
        for(int i=0;i<LENGTH_UINT16;i++){
            buf.at(index+i)=(value>>(8*i))&0xFF;
        }
    };
    static std::uint32_t deserialize_uint32(std::uint32_t index, const std::vector<std::uint8_t> &buf){
        std::uint32_t ret=0;
        for(int i=0;i<LENGTH_UINT32;i++){
            ret+=(((std::uint32_t)buf.at(index+i))&0x000000FF)<<(8*i);
        }
        return ret;
    };
    static std::uint16_t deserialize_uint16(std::uint32_t index, const std::vector<std::uint8_t> &buf){
        std::uint16_t ret=0;
        for(int i=0;i<LENGTH_UINT16;i++){
            ret+=(((std::uint16_t)buf.at(index+i))&0x00FF)<<(8*i);
        }
        return ret;
    };
    static std::string to_string_iparray(std::array<std::uint8_t,ADDR_SIZE> &a){
        std::string ret="["+std::to_string(a.at(0));
        for(int i=1;i<ADDR_SIZE;i++){
            ret+=","+std::to_string(a.at(i));
        }
        return ret+"]";
    }
    static std::string to_string_vector(std::vector<std::uint8_t> &v){
        if(v.size()<1){
            return "";
        }
        std::string ret="v["+std::to_string(v.at(0));
        for(size_t i=1;i<v.size();i++){
            //std::cerr<<"i:"<<std::to_string(i)<<" v[]:"<<std::to_string(v[i])<<std::endl;
            ret+=","+std::to_string(v.at(i));
        }
        return ret+"]";
    }
};
};
#endif