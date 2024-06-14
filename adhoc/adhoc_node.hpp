#ifndef ADHOC_NODE_HPP
#define ADHOC_NODE_HPP

#include "adhoc_routing.hpp"
#include "adhoc_routing_fragment.hpp"
#include <thread>
namespace oit::ist::nws::adhoc_routing{

#define PORT 20000

class adhoc_node{
    class an_receiver{
        protected:
        bool loop;
        void receive_msg();
        public:
        an_receiver(){}
        ~an_receiver(){}
        void start();
        void stop();
    };
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

    
    adhoc_node();

    public:
    adhoc_node(string &if_name);
    ~adhoc_node();
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