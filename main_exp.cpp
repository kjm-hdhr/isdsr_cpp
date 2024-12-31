#include <iostream>
#include "./adhoc/adhoc_node.hpp"

#include <array>
#include <memory>
#include <unistd.h>

#include <arpa/inet.h>


#if defined (ECDSA)
#include "./isdsr/isdsr_routing.hpp"
#include "./ecdsa/ecdsa_sig.hpp"
#elif defined (LATTICE)
#include "./isdsr/isdsr_routing.hpp"
#include "./lattice/lattice_sig.hpp"
#else
#include "./dsr/dsr_routing.hpp"
#endif

using namespace oit::ist::nws::adhoc_routing;

int main(int argc, char** argv){
    //args -i ifname -d dest ip
    const char* optstring = "p:i:d:r:h:";
    opterr = 0; // disable error log

    // non-option or end of argument list or error('?')までloop
    int c;
    char *c_ifname=nullptr;
    char *c_dest=nullptr;
    char *c_repeat=nullptr;
    char *c_hops=nullptr;
    bool exp_rtt_sd=false;
    bool exp_rtt_next=false;
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
                exp_rtt_sd=true;
                break;
            }
            case 'h':{
                c_hops=optarg;
                exp_rtt_next=true;
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

    
#if defined (ECDSA) || defined (LATTICE)
    isdsr_routing isdsr;
    signature_scheme *ss;
#else
    dsr_routing dsr;
#endif

#if defined (ECDSA)
    ecdsa_sig ecdsa;
    ss=&ecdsa;
#elif defined (LATTICE)
    lattice_sig lattice;
    ss=&lattice;
#else

#endif
    if(exp_rtt_sd){
        adhoc_node_exp ane(ifname);
        std::cout<<"routing "<<std::endl;
#if defined (ECDSA) || defined (LATTICE)
        ane.set_routing(&isdsr);
        isdsr.set_signature_scheme(ss);
        ss->set_id(*(isdsr.get_id()));
        std::cout<<isdsr.to_string()<<std::endl;
#else
        ane.set_routing(&dsr);
        std::cout<<dsr.to_string()<<std::endl;
#endif
        struct in_addr inaddr;
        inet_aton(c_dest,&inaddr);
        array<uint8_t,ADDR_SIZE> dest;
        adhoc_util::ip_to_array(inaddr.s_addr,dest);
        std::cout<<"dest ip:"<<adhoc_util::to_string_iparray(dest)<<std::endl;
        ane.set_dest(dest);
        ane.set_repeat_times(std::stoi(string(c_repeat)));
        if(exp_rtt_next){
            ane.set_hops(std::stoi(string(c_hops)));
        }
        else{
            ane.set_hops(0);
        }
        ane.set_repeat_interval(1);
        std::cerr<<"exp1"<<std::endl;
        ane.start();
    }
    else{
        adhoc_node an(ifname);
#if defined (ECDSA) || defined (LATTICE)
        an.set_routing(&isdsr);
        isdsr.set_signature_scheme(ss);
        ss->set_id(*(isdsr.get_id()));
        std::cout<<"routing "<<std::endl;
        std::cout<<isdsr.to_string()<<std::endl;
#else
        an.set_routing(&dsr);
        std::cout<<"routing "<<std::endl;
        std::cout<<dsr.to_string()<<std::endl;
#endif
        an.start();
    }
}