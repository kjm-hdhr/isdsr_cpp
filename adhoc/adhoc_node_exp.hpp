#ifndef ADHOC_NODE_EXP_HPP
#define ADHOC_NODE_EXP_HPP

#include "adhoc_node.hpp"
#include "adhoc_routing.hpp"
#include "adhoc_routing_fragment.hpp"
#include <chrono>
#include <thread>
#include <map>
#include <unordered_map>
namespace oit::ist::nws::adhoc_routing{

#define PORT 20000

class adhoc_node_exp : public adhoc_node{
   
    protected:
    int repeat_time;
    int repeat_interval;
    adhoc_node_exp();
    std::unordered_map<std::uint32_t,std::chrono::steady_clock::time_point> rtt_s;
    std::unordered_map<std::uint32_t,std::chrono::steady_clock::time_point> rtt_r;
    public:
    adhoc_node_exp(string &if_name);
    ~adhoc_node_exp();
    void virtual establish_route(array<std::uint8_t,ADDR_SIZE> &dest);
    void set_repeat_times(int r){this->repeat_time=r;}
    void set_repeat_interval(int i){this->repeat_interval=i;}
    void virtual receive_msg();
    void virtual start();
    void virtual stop();
    void virtual measure_time();
    
    
};
class adhoc_node_exp_rtt : public adhoc_node_exp{
   
    protected:
    std::uint8_t hops;
    adhoc_node_exp_rtt():adhoc_node_exp(){};
    public:
    adhoc_node_exp_rtt(string &if_name):adhoc_node_exp(if_name){};
    ~adhoc_node_exp_rtt(){};
    void set_hops(std::uint8_t h){this->hops=h;};
    //void generate_exp_packet();
    void establish_route(array<std::uint8_t,ADDR_SIZE> &dest);
    void set_repeat_times(int r){this->repeat_time=r;}
    void set_repeat_interval(int i){this->repeat_interval=i;}
    void virtual receive_msg();
    void virtual start();
    //void virtual stop();
    //void virtual measure_time();
    
    
};
};
#endif