#ifndef ADHOC_UTIL_HPP
#define ADHOC_UTIL_HPP

#include "adhoc_routing_constants.hpp"
#include <cstdint>
#include <vector>
#include <array>
#include <iostream>
#define LENGTH_UINT32 4
#define LENGTH_UINT16 2

using namespace std;

namespace oit::ist::nws::adhoc_routing{
class adhoc_util{
    public:
    static void ip_to_array(const uint32_t ip_addr, array<uint8_t,ADDR_SIZE> &ar){
        for(int i=0;i<ADDR_SIZE;i++){
            ar[i]=(ip_addr>>(8*i))&0xFF;
        }
    };
    static uint32_t array_to_ip(const array<uint8_t,ADDR_SIZE> &ar){
        uint32_t ret=0;
        for(int i=0;i<ADDR_SIZE;i++){
            ret+=(((uint32_t)ar[i])&0x000000FF)<<(8*i);
        }
        return ret;
    };
    static void serialize_uint32(uint32_t index, vector<uint8_t> &buf, const uint32_t value){
        for(int i=0;i<LENGTH_UINT32;i++){
            buf[index+i]=(value>>(8*i))&0xFF;
        }
    };
    static void serialize_uint16(uint32_t index, vector<uint8_t> &buf, const uint16_t value){
        for(int i=0;i<LENGTH_UINT16;i++){
            buf[index+i]=(value>>(8*i))&0xFF;
        }
    };
    static uint32_t deserialize_uint32(uint32_t index, const vector<uint8_t> &buf){
        uint32_t ret=0;
        for(int i=0;i<LENGTH_UINT32;i++){
            ret+=(((uint32_t)buf[index+i])&0x000000FF)<<(8*i);
        }
        return ret;
    };
    static uint16_t deserialize_uint16(uint32_t index, const vector<uint8_t> &buf){
        uint16_t ret=0;
        for(int i=0;i<LENGTH_UINT16;i++){
            ret+=(((uint16_t)buf[index+i])&0x00FF)<<(8*i);
        }
        return ret;
    };
    static string to_string_iparray(array<uint8_t,ADDR_SIZE> &a){
        string ret="["+std::to_string(a[0]);
        for(int i=1;i<ADDR_SIZE;i++){
            ret+=","+std::to_string(a[i]);
        }
        return ret+"]";
    }
    static string to_string_vector(vector<uint8_t> &v){
        if(v.size()<1){
            return "";
        }
        string ret="v["+std::to_string(v[0]);
        for(size_t i=1;i<v.size();i++){
            //std::cerr<<"i:"<<std::to_string(i)<<" v[]:"<<std::to_string(v[i])<<std::endl;
            ret+=","+std::to_string(v[i]);
        }
        return ret+"]";
    }
};
};
#endif