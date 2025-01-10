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
    this->seq=0;
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
                adhoc_util::ip_to_array(((struct sockaddr_in*)p->ifa_addr)->sin_addr.s_addr,this->ip_addr);
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
bool adhoc_node::is_own_id(array<std::uint8_t,ADDR_SIZE> &id){
    return std::equal(id.begin(),id.end(),this->ip_addr.begin(),this->ip_addr.end());
}

void adhoc_node::set_dest(array<std::uint8_t,ADDR_SIZE> &dest){
    std::copy(dest.begin(),dest.end(),this->ip_dest.begin());
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

int adhoc_node::receive(std::vector<std::uint8_t> &buf){
    std::vector<std::uint8_t> fragmented_buf;
    std::uint8_t rcv_length[ARF_HEADER_LENGTH+FRAGMENT_LENGTH_SIZE];
    std::uint32_t fragment_length;
    std::vector<std::uint8_t> fl_array(4);
    arf_packet arfp;

    recv(this->rcv_sock,rcv_length,ARF_HEADER_LENGTH+FRAGMENT_LENGTH_SIZE,MSG_PEEK);
    std::copy(std::begin(rcv_length)+INDEX_FRAGMENT_LENGTH,std::end(rcv_length),fl_array.begin());
    fragment_length=adhoc_util::deserialize_uint32(0,fl_array);
    std::uint8_t rcv_buf[fragment_length];
    recv(this->rcv_sock,rcv_buf,fragment_length,0);
    fragmented_buf.resize(fragment_length);
    
    for(std::uint32_t i=0;i<fragment_length;i++){
        fragmented_buf[i]=rcv_buf[i];
    }
    
    arfp.deserialize(fragmented_buf);
    return this->arfm.defragment(arfp,buf);
}
void adhoc_node::after_received(std::vector<std::uint8_t> &buf){
    array<std::uint8_t,ADDR_SIZE> *next;
    std::cerr<<"receive message buf length="<<std::to_string(buf.size())<<std::endl;
        next=this->routing->packet_processing(buf);
        
        if(next!=nullptr){
            if(this->is_own_id(*next)){
                std::cout<<"route established adhoc node"<<std::endl;
                return;
            }
            this->send_msg(*next,buf);
            std::cerr<<"rec next:"<<adhoc_util::to_string_iparray(*next)<<std::endl;
        }
        std::cerr<<"receive msg 9"<<std::endl;
}

void adhoc_node::receive_msg(){
    arf_packet arfp;
    vector<std::uint8_t> buf;
    while(loop){
       if(this->receive(buf)==0){
            continue;
        }
        this->after_received(buf);
    }
}

void adhoc_node::send_msg(array<std::uint8_t,ADDR_SIZE> &next,vector<std::uint8_t> &buf){
    
    std::cerr<<"adhoc node send_msg next:"<<adhoc_util::to_string_iparray(next)<<std::endl;
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

}
void adhoc_node::start(){
    this->loop=true;
    this->rcv_th=std::thread(&adhoc_node::receive_msg, this);
    this->rcv_th.join();
}
void adhoc_node::stop(){
    this->loop=false;
}
void adhoc_node::establish_route(array<std::uint8_t,ADDR_SIZE> &dest){
    vector<std::uint8_t> buf;
    array<std::uint8_t,ADDR_SIZE>* next;
    next=this->routing->generate_initial_request(dest,buf);
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

adhoc_node_exp::adhoc_node_exp():adhoc_node(){
    this->repeat_interval=0;
    this->repeat_time=0;
    this->hops=0;
}
adhoc_node_exp::adhoc_node_exp(string &if_name):adhoc_node(if_name){
    this->repeat_interval=0;
    this->repeat_time=0;
    this->hops=0;
}
adhoc_node_exp::~adhoc_node_exp(){

}

void adhoc_node_exp::after_received(std::vector<std::uint8_t> &buf){
    array<std::uint8_t,ADDR_SIZE> *next;
    next=this->routing->packet_processing(buf);
    if(next!=nullptr){
        std::cout<<"------------- next address = "<<adhoc_util::to_string_iparray(*next)<<std::endl;
        
        if(this->hops>0){
            std::uint32_t seq=adhoc_util::deserialize_uint32(0,buf);
            this->rtt_r.insert(std::make_pair(seq,std::chrono::steady_clock::now()));
            return;
        }
        
        if(this->is_own_id(*next)){
            std::uint32_t seq=adhoc_util::deserialize_uint32(0,buf);
            this->rtt_r.insert(std::make_pair(seq,std::chrono::steady_clock::now()));
            std::cout<<"route established seq:"<<std::to_string(seq)<<std::endl;
            return;
        }
        this->send_msg(*next,buf);
        std::cerr<<"rec next:"<<adhoc_util::to_string_iparray(*next)<<std::endl;
    }
}

void adhoc_node_exp::start(){
    this->loop=true;
    std::cerr<<"start 1"<<std::endl;
    this->rcv_th=std::thread(&adhoc_node_exp::receive_msg, this);
    this->establish_route(this->ip_dest);
    std::this_thread::sleep_for(std::chrono::seconds(5));
    this->measure_time();
    std::cerr<<"start 2"<<std::endl;
    this->rcv_th.join();
}

void adhoc_node_exp::establish_route(array<std::uint8_t,ADDR_SIZE> &dest){
    vector<std::uint8_t> buf;
    array<std::uint8_t,ADDR_SIZE>* next;

    std::cerr<<"establish route routing:"<<routing->routing_name()<<std::endl;
    if(this->hops<=1){
        std::cerr<<"establish route 1 hop"<<std::endl;
        next=this->routing->generate_initial_request(dest,buf);
    }
    else{
        array<std::uint8_t,ADDR_SIZE> tmp={10,0,0,101};
        this->routing->set_id(tmp);
        next=this->routing->generate_initial_request(dest,buf);
        for(int i=1;i<this->hops-1;i++){
            tmp[3]=tmp[3]+1;
            this->routing->set_id(tmp);
            this->routing->packet_processing(buf);
        }
        this->routing->set_id(this->ip_addr);
        routing->packet_processing(buf);
    }
    std::cerr<<"ane_rtt establish route buf size="<<std::to_string(buf.size())<<std::endl;
    std::uint32_t seq=0;
    for(int i=0;i<this->repeat_time;i++){
        seq++;
        adhoc_util::serialize_uint32(0,buf,seq);
        std::cerr<<"message sent including "<<std::to_string(this->hops)<< " hops seq:"<<std::to_string(seq)<<std::endl;
        this->rtt_s.insert(std::make_pair(seq,std::chrono::steady_clock::now()));
        this->send_msg(*next,buf);
        std::this_thread::sleep_for(std::chrono::seconds(this->repeat_interval));
    }

}

void adhoc_node_exp::measure_time(){
    std::vector<double> durations;
    for(std::pair<std::uint32_t,std::chrono::steady_clock::time_point> t:this->rtt_s){
        if (this->rtt_r.find(t.first) != this->rtt_r.end()) {
            durations.push_back(((double)std::chrono::duration_cast<std::chrono::nanoseconds>(rtt_r.at(t.first)-t.second).count())/1000);
        }
    }
    double sum=0;
    for(double d:durations){
        std::cout<<"duration d:"<<std::to_string(d)<<std::endl;
        sum+=d;
    }
    double avg=sum/durations.size();

    std::cout<<"repeat time:"<<std::to_string(this->repeat_time);
    std::cout<<" repeat interval:"<<std::to_string(this->repeat_interval)<<std::endl;
    std::cout<<"from "<<adhoc_util::to_string_iparray(this->ip_addr);
    std::cout<<" to "<<adhoc_util::to_string_iparray(this->ip_dest)<< std::endl;
    std::cout<<std::to_string(durations.size())<<" route establishment";
    std::cout<<" average rtt:"<<std::to_string(avg)<<std::endl;
}