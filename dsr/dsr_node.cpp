#include <iostream>
#include "../adhoc/adhoc_node.hpp"
#include "dsr_routing.hpp"
#include <array>
#include <unistd.h>

#include <arpa/inet.h>
using namespace oit::ist::nws::adhoc_routing;

int main(int argc, char** argv){
    //args -i ifname -d dest ip
    const char* optstring = "i:d:";
    opterr = 0; // disable error log

    // non-option or end of argument list or error('?')までloop
    int c;
    char *c_ifname=nullptr;
    char *c_dest=nullptr;
    while ((c=getopt(argc, argv, optstring)) != -1) {
        //printf("opt=%c ", c);
        switch(c){
            case 'i':{
                c_ifname=optarg;
                break;
            }
            case 'd':{
                c_dest=optarg;
                break;
            }
            default:{
                std::cerr<<"unknown arg: -"<<std::to_string((char)c)<<std::endl;
                std::cerr<<" arg -i interface name, -d ip address"<<std::endl;
                std::exit(0);
            }
        }
    }
    string ifname(c_ifname);
    adhoc_node an(ifname);
    dsr_routing dsr;
    
    an.set_routing(&dsr);
    std::cout<<"routing "<<std::endl;
    std::cout<<dsr.to_string()<<std::endl;
    //an.receive_msg();
    
    if(c_dest!=nullptr){
        struct in_addr inaddr;
        inet_aton(c_dest,&inaddr);
        array<uint8_t,ADDR_SIZE> dest;
        adhoc_util::ip_to_array(inaddr.s_addr,dest);
        std::cout<<"dest ip:"<<adhoc_util::to_string_iparray(dest)<<std::endl;
        an.establish_route(dest);
    }
    an.start();
}