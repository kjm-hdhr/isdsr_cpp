#ifndef ADHOC_NODE_HPP
#define ADHOC_NODE_HPP

#include "adhoc_routing.hpp"
#include "adhoc_routing_fragment.hpp"
#include <thread>
#include <chrono>
#include <map>
#include <unordered_map>

namespace oit::ist::nws::adhoc_routing{

#define PORT 20000

class adhoc_node{
    
    protected:
    string if_name;
    array<std::uint8_t,ADDR_SIZE> ip_addr;
    array<std::uint8_t,ADDR_SIZE> brd_addr;
    array<std::uint8_t,ADDR_SIZE> ip_dest;
    arf_manager arfm;
    std::thread rcv_th;

    adhoc_routing* routing;

    int send_sock;
    int rcv_sock;
    bool loop;
    std::uint32_t seq;

    adhoc_node();
    void initialize(string &if_name);
    int get_interface();
    bool is_own_id(array<std::uint8_t,ADDR_SIZE> &id);
    int receive(std::vector<std::uint8_t> &buf);
    virtual void after_received(std::vector<std::uint8_t> &buf);

    public:
    adhoc_node(string &if_name);
    ~adhoc_node();
    void set_dest(array<std::uint8_t,ADDR_SIZE> &dest);
    void set_routing(adhoc_routing* routing);
    void receive_msg();
    void send_msg(array<std::uint8_t,ADDR_SIZE> &next,vector<std::uint8_t> &buf);
    string to_string();
    virtual void start();
    void stop();
    virtual void establish_route(array<std::uint8_t,ADDR_SIZE> &dest);
};
class adhoc_node_exp : public adhoc_node{
   
    protected:
    int repeat_time;
    int repeat_interval;
    int hops;
    adhoc_node_exp();
    virtual void after_received(std::vector<std::uint8_t> &buf) override;
    std::unordered_map<std::uint32_t,std::chrono::steady_clock::time_point> rtt_s;
    std::unordered_map<std::uint32_t,std::chrono::steady_clock::time_point> rtt_r;
    public:
    adhoc_node_exp(string &if_name);
    ~adhoc_node_exp();
    void set_hops(int h){this->hops=h;};
    void virtual establish_route(array<std::uint8_t,ADDR_SIZE> &dest) override;
    void set_repeat_times(int r){this->repeat_time=r;}
    void set_repeat_interval(int i){this->repeat_interval=i;}
    void virtual start() override;
    void measure_time();
};
};
#endif