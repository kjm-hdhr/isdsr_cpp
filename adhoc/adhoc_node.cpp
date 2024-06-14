#include "adhoc_node.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include<ifaddrs.h>
#include <iostream>
using namespace oit::ist::nws::adhoc_routing;

adhoc_node::adhoc_node(){

}
adhoc_node::adhoc_node(string &if_name){
    this->if_name=if_name;
    this->initialize(if_name);
    this->arfm.set_own_id(this->ip_addr);
}
adhoc_node::~adhoc_node(){

}
int adhoc_node::get_interface(){
    this->ip_addr.fill(0);
    this->brd_addr.fill(0);
    struct ifaddrs * addrs = nullptr;
    int ret = getifaddrs(&addrs);
    if(ret != 0) {
        fprintf(stderr, "!!! getifaddrs [%s:L%d]\n", __FILE__, __LINE__);
        return 1;
    }
    for(auto p = addrs; p!=nullptr; p=p->ifa_next){
        std::string_view s(p->ifa_name);
        if(this->if_name==s){
            if(p->ifa_addr->sa_family==AF_INET){
                //std::uint32_t ip_addr=((struct sockaddr_in*)p->ifa_addr)->sin_addr.s_addr;
                //std::cout<<"ip addr "<<std::to_string(ip_addr)<<std::endl;
                adhoc_util::ip_to_array(((struct sockaddr_in*)p->ifa_addr)->sin_addr.s_addr,this->ip_addr);
                //std::uint32_t mask=((struct sockaddr_in*)(p->ifa_netmask))->sin_addr.s_addr;
                //std::cout<<"ip mask "<<std::to_string(mask)<<std::endl;
                adhoc_util::ip_to_array(((struct sockaddr_in*)(p->ifa_netmask))->sin_addr.s_addr,this->brd_addr);
                for(int i=0;i<ADDR_SIZE;i++){
                    this->brd_addr[i]=this->brd_addr[i]&this->ip_addr[i];
                    if(this->brd_addr[i]==0){
                        this->brd_addr[i]=0xFF;
                    }
                }
                freeifaddrs(addrs);
                return 0;
            }
        
        }
    }
    freeifaddrs(addrs);
    return -1;
}
void adhoc_node::initialize(string &if_name){
    this->get_interface();
    this->send_sock=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    int yes=1;
    setsockopt(this->send_sock,
            SOL_SOCKET, SO_BROADCAST, (char *)&yes, sizeof(yes));
	this->rcv_sock=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(PORT);
    bind(rcv_sock,((struct sockaddr *)&addr),sizeof(addr));
}
void adhoc_node::set_routing(adhoc_routing* routing){
    this->routing=routing;
    this->routing->set_id(ip_addr);
    this->routing->set_broadcast(this->brd_addr);
}

void adhoc_node::receive_msg(){
	std::uint32_t rcv=0;
    vector<std::uint8_t> fragmented_buf;
    std::uint8_t rcv_length[ARF_HEADER_LENGTH+FRAGMENT_LENGTH_SIZE];
    std::uint32_t fragment_length;
    arf_packet arfp;
    vector<std::uint8_t> buf;
    array<std::uint8_t,ADDR_SIZE> *next;
    //std::cerr<<"receive msg 1"<<std::endl;
    vector<std::uint8_t> fl_array(4);
    while(loop){

        //std::cerr<<"receive msg 2"<<std::endl;
        rcv=recv(this->rcv_sock,rcv_length,ARF_HEADER_LENGTH+FRAGMENT_LENGTH_SIZE,MSG_PEEK);
        //std::cerr<<"receive msg 3 rcv:"<<rcv<<std::endl;
        //std::cerr<<"rcv_length["<<std::to_string(rcv_length[0]);
        //for(int i=1;i<(ARF_HEADER_LENGTH+FRAGMENT_LENGTH_SIZE);i++){
        //    std::cerr<<","<<std::to_string(rcv_length[i]);
        //}
        //std::cerr<<"]"<<std::endl;
        //std::copy(std::begin(rcv_length)+INDEX_FRAGMENT_LENGTH,std::end(rcv_length),buf.begin());
        std::copy(std::begin(rcv_length)+INDEX_FRAGMENT_LENGTH,std::end(rcv_length),fl_array.begin());
        //std::cerr<<"receive msg 4"<<std::endl;
        //fragment_length=adhoc_util::deserialize_uint32(0,buf);
        fragment_length=adhoc_util::deserialize_uint32(0,fl_array);
        //std::cerr<<"receive msg 5"<<std::endl;
        std::uint8_t rcv_buf[fragment_length];
        //std::cerr<<"receive msg 6"<<std::endl;
        rcv=recv(this->rcv_sock,rcv_buf,fragment_length,0);
        //std::cerr<<"receive msg 7"<<std::endl;
        fragmented_buf.resize(fragment_length);

        std::cerr<<"receive msg 8"<<std::endl;
        for(std::uint32_t i=0;i<fragment_length;i++){
            fragmented_buf[i]=rcv_buf[i];
            std::cerr<<","<<std::to_string(fragmented_buf[i]);
        }
        std::cerr<<std::endl;
        arfp.deserialize(fragmented_buf);
        std::cerr<<"arfp:"<<arfp.to_string()<<std::endl;
        int defrag=arfm.defragment(arfp,buf);
        if(defrag==0){
            continue;
        }
        next=this->routing->packet_processing(buf);
        if(next!=nullptr){
            this->send_msg(*next,buf);
            std::cerr<<"rec next:"<<adhoc_util::to_string_iparray(*next)<<std::endl;
        }
        std::cerr<<"receive msg 9"<<std::endl;
    }
}

void adhoc_node::send_msg(array<std::uint8_t,ADDR_SIZE> &next,vector<std::uint8_t> &buf){
    
    std::cerr<<"adhoc node send_msg next:"<<adhoc_util::to_string_iparray(next)<<std::endl;
    std::cerr<<"buf["<<std::to_string(buf[0]);
    for(size_t i=1;i<buf.size();i++){
        std::cerr<<","<<std::to_string(buf[i]);
    }
    std::cerr<<"]"<<std::endl;
    std::uint32_t next_ip=adhoc_util::array_to_ip(next);
    vector<arf_packet> arfp_v;
    this->arfm.fragment(arfp_v,buf);
    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_port = htons(PORT);
    dest.sin_addr.s_addr=next_ip;
    vector<std::uint8_t> send_buf;
    for(size_t i=0;i<arfp_v.size();i++){
        send_buf.clear();
        send_buf.resize(arfp_v.at(i).get_fragment_length());
        arfp_v.at(i).serialize(send_buf);
        std::uint8_t data[send_buf.size()];
        std::copy(send_buf.begin(),send_buf.end(),data);
        sendto(this->send_sock,data,send_buf.size(),0,(struct sockaddr *)(&dest),sizeof(struct sockaddr_in));
    }
    //sendto(send_sock,byte_array,length,0,(struct sockaddr *)dest,sizeof(struct sockaddr_in));

}

void adhoc_node::data_send(ar_packet* pkt){
    array<std::uint8_t,ADDR_SIZE> *next=pkt->get_next();
    std::uint32_t next_ip=adhoc_util::array_to_ip(*next);
    vector<std::uint8_t> buf;
    buf.resize(pkt->packet_size());
    pkt->serialize(buf);
    vector<arf_packet> arfp_v;
    this->arfm.fragment(arfp_v,buf);
    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_port = htons(PORT);
    dest.sin_addr.s_addr=next_ip;
    vector<std::uint8_t> send_buf;

    for(size_t i=0;i<arfp_v.size();i++){
        send_buf.clear();
        send_buf.resize(arfp_v.at(i).get_fragment_length());
        arfp_v.at(i).serialize(send_buf);
        std::uint8_t data[send_buf.size()];
        std::copy(send_buf.begin(),send_buf.end(),data);
        sendto(this->send_sock,data,send_buf.size(),0,(struct sockaddr *)(&dest),sizeof(struct sockaddr_in));
    }
}
void adhoc_node::start(){
    this->loop=true;

    std::cerr<<"start 1"<<std::endl;
    this->rcv_th=std::thread(&adhoc_node::receive_msg, this);

    std::cerr<<"start 2"<<std::endl;
    this->rcv_th.join();
}
void adhoc_node::stop(){
    this->loop=false;
    //this->rcv_th.join();
}
void adhoc_node::establish_route(array<std::uint8_t,ADDR_SIZE> &dest){
    vector<std::uint8_t> buf;
    array<std::uint8_t,ADDR_SIZE>* next;
    next=this->routing->generate_initiali_request(dest,buf);
    this->send_msg(*next,buf);
}
string adhoc_node::to_string(){
    string ret="if name: "+this->if_name;
    ret+=" ip addr: ["+std::to_string(this->ip_addr.at(0));
    for(size_t i=1;i<this->ip_addr.size();i++){
        ret+=","+std::to_string(this->ip_addr.at(i));
    }
    ret+="]";

    ret+=" brd: ["+std::to_string(this->brd_addr.at(0));
    for(size_t i=1;i<this->ip_addr.size();i++){
        ret+=","+std::to_string(this->brd_addr.at(i));
    }
    ret+="]";
    return ret;
}