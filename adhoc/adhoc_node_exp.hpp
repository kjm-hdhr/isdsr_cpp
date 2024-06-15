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
    void establish_route(array<std::uint8_t,ADDR_SIZE> &dest);
    void set_repeat_times(int r){this->repeat_time=r;}
    void set_repeat_interval(int i){this->repeat_interval=i;}
    void receive_msg();
    void start();
    void stop();
    void measure_time();
    
    
};
};
#endif