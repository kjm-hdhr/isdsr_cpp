#ifndef ADHOC_NODE_HPP
#define ADHOC_NODE_HPP

#include "adhoc_routing.hpp"
#include "adhoc_routing_fragment.hpp"
#include <thread>
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

    void initialize(string &if_name);
    int get_interface();
    bool is_own_id(array<std::uint8_t,ADDR_SIZE> &id){return std::equal(id.begin(),id.end(),this->ip_addr.begin(),this->ip_addr.end());}
    adhoc_node();
    std::uint32_t seq;
    public:
    adhoc_node(string &if_name);
    ~adhoc_node();
    void set_dest(array<std::uint8_t,ADDR_SIZE> &dest){std::copy(dest.begin(),dest.end(),this->ip_dest.begin());}
    void set_routing(adhoc_routing* routing);
    void establish_route(array<std::uint8_t,ADDR_SIZE> &dest);
    void receive_msg();
    void send_msg(array<std::uint8_t,ADDR_SIZE> &next,vector<std::uint8_t> &buf);
    string to_string();
    void data_send(ar_packet* pkt);
    void start();
    void stop();
    
    
};
};
#endif