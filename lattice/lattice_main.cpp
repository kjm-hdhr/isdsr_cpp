#include <iostream>
#include "../adhoc/adhoc_node.hpp"
#include "../adhoc/adhoc_node_exp.hpp"
#include "../isdsr/isdsr_routing.hpp"
#include "lattice_sig.hpp"
#include <array>
#include <memory>
#include <unistd.h>

#include <arpa/inet.h>
using namespace oit::ist::nws::adhoc_routing;

int main(int argc, char** argv){
    //args -i ifname -d dest ip
    const char* optstring = "i:d:r:";
    opterr = 0; // disable error log

    // non-option or end of argument list or error('?')までloop
    int c;
    char *c_ifname=nullptr;
    char *c_dest=nullptr;
    char *c_repeat=nullptr;
    bool exp=false;
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
            case 'r':{
                c_repeat=optarg;
                exp=true;
                break;
            }
            default:{
                std::cerr<<"unknown arg: -"<<std::to_string((char)c)<<std::endl;
                std::cerr<<" arg -i interface name, -d ip address"<<std::endl;
                std::exit(0);
            }
        }
    }
    if((c_dest==nullptr) xor (c_repeat==nullptr)){
        std::cerr<<"both arg d and arg r are needed in an experiment"<<std::endl;
        std::exit(0);
    }
    string ifname(c_ifname);
    isdsr_routing isdsr;
    lattice_sig lattice;

    if(exp){
        adhoc_node_exp ane(ifname);
        ane.set_routing(&isdsr);
        isdsr.set_signature_scheme(&lattice);
        lattice.set_id(*(isdsr.get_id()));
        std::cout<<"routing "<<std::endl;
        std::cout<<isdsr.to_string()<<std::endl;
        struct in_addr inaddr;
        inet_aton(c_dest,&inaddr);
        array<uint8_t,ADDR_SIZE> dest;
        adhoc_util::ip_to_array(inaddr.s_addr,dest);
        std::cout<<"dest ip:"<<adhoc_util::to_string_iparray(dest)<<std::endl;
        ane.set_dest(dest);
        ane.set_repeat_times(std::stoi(string(c_repeat)));
        ane.set_repeat_interval(2);
        std::cerr<<"exp1"<<std::endl;
        ane.start();
    }
    else{
    adhoc_node an(ifname);
    an.set_routing(&isdsr);
    isdsr.set_signature_scheme(&lattice);
    lattice.set_id(*(isdsr.get_id()));
    std::cout<<"routing "<<std::endl;
    std::cout<<isdsr.to_string()<<std::endl;
    //an.receive_msg();
    /*
    if(c_dest!=nullptr){
        struct in_addr inaddr;
        inet_aton(c_dest,&inaddr);
        array<uint8_t,ADDR_SIZE> dest;
        adhoc_util::ip_to_array(inaddr.s_addr,dest);
        std::cout<<"dest ip:"<<adhoc_util::to_string_iparray(dest)<<std::endl;
        an.establish_route(dest);
    }
    */
    an.start();
    }
}