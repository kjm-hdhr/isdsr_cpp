#include "adhoc_node_exp.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include<ifaddrs.h>
#include <iostream>
using namespace oit::ist::nws::adhoc_routing;

adhoc_node_exp::adhoc_node_exp():adhoc_node(){

}
adhoc_node_exp::adhoc_node_exp(string &if_name):adhoc_node(if_name){
}
adhoc_node_exp::~adhoc_node_exp(){

}
void adhoc_node_exp::receive_msg(){
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
        
        //std::cerr<<"receive msg 8"<<std::endl;
        for(std::uint32_t i=0;i<fragment_length;i++){
            fragmented_buf[i]=rcv_buf[i];
            //std::cerr<<","<<std::to_string(fragmented_buf[i]);
        }
        //std::cerr<<std::endl;
        
        arfp.deserialize(fragmented_buf);
        //std::cerr<<"arfp:"<<arfp.to_string()<<std::endl;
        int defrag=arfm.defragment(arfp,buf);
        if(defrag==0){
            continue;
        }
        std::uint32_t seq=adhoc_util::deserialize_uint32(0,buf);
        next=this->routing->packet_processing(buf);

        std::cerr<<"receive msg 8"<<std::endl;
        if(next!=nullptr){
            std::cout<<"------------- next address = "<<adhoc_util::to_string_iparray(*next)<<std::endl;
            if(this->is_own_id(*next)){
                this->rtt_r.insert(std::make_pair(seq,std::chrono::steady_clock::now()));
                std::cout<<"route established seq:"<<std::to_string(seq)<<std::endl;
                continue;
            }
            this->send_msg(*next,buf);
            std::cerr<<"rec next:"<<adhoc_util::to_string_iparray(*next)<<std::endl;
        }
        //std::cerr<<"receive msg 9"<<std::endl;
    }
}

void adhoc_node_exp::start(){
    this->loop=true;
    std::cerr<<"start 1"<<std::endl;
    this->rcv_th=std::thread(&adhoc_node_exp::receive_msg, this);
    for(int i=0;i<this->repeat_time;i++){
        std::cout<<"repeat:"<<std::to_string(i)<<" of "<<std::to_string(this->repeat_time)<<std::endl;
        this->establish_route(this->ip_dest);
        std::this_thread::sleep_for(std::chrono::seconds(this->repeat_interval));
    }
    std::this_thread::sleep_for(std::chrono::seconds(5));
    this->measure_time();
    std::cerr<<"start 2"<<std::endl;
    this->rcv_th.join();
}
void adhoc_node_exp::stop(){
    this->loop=false;
    //this->rcv_th.join();
}
void adhoc_node_exp::establish_route(array<std::uint8_t,ADDR_SIZE> &dest){
    vector<std::uint8_t> buf;
    array<std::uint8_t,ADDR_SIZE>* next;
    next=this->routing->generate_initiali_request(dest,buf);
    std::uint32_t seq=adhoc_util::deserialize_uint32(0,buf);
    std::cerr<<"route establishment seq:"<<std::to_string(seq)<<std::endl;
    this->rtt_s.insert(std::make_pair(seq,std::chrono::steady_clock::now()));
    this->send_msg(*next,buf);
}

void adhoc_node_exp::measure_time(){
    std::vector<double> durations;
    for(std::pair<std::uint32_t,std::chrono::steady_clock::time_point> t:this->rtt_s){
        //std::cerr<<"measure1"<<std::endl;
        if (this->rtt_r.find(t.first) != this->rtt_r.end()) {
            //std::cerr<<"measure2"<<std::endl;
            durations.push_back(((double)std::chrono::duration_cast<std::chrono::milliseconds>(rtt_r.at(t.first)-t.second).count())/1000);
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