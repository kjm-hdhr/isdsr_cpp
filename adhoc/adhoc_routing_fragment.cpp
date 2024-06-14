#include "adhoc_routing_fragment.hpp"
#include "adhoc_util.hpp"
#include <iostream>
#include <algorithm>

using namespace oit::ist::nws::adhoc_routing;

void arf_header::serialize(vector<std::uint8_t> &buf){
    if(buf.size()<this->header_length()){
        buf.resize(this->header_length());
    }
    adhoc_util::serialize_uint32(INDEX_SEQUENCE_NUMBER,buf,this->seq_no);
    buf[INDEX_FRAGMENT_ID]=this->fragment_id;
    buf[INDEX_NUM_OF_FRAGMENTS]=this->num_of_fragments;
    adhoc_util::serialize_uint32(INDEX_TOTAL_LENGTH,buf,this->total_length);
    std::copy(this->remote_address.begin(),this->remote_address.end(),buf.begin()+INDEX_REMOTE_ADDRESS);
}
void arf_header::deserialize(const vector<std::uint8_t> &buf){
    this->seq_no=adhoc_util::deserialize_uint32(INDEX_SEQUENCE_NUMBER,buf);
    this->fragment_id=buf[INDEX_FRAGMENT_ID];
    this->num_of_fragments=buf[INDEX_NUM_OF_FRAGMENTS];
    this->total_length=adhoc_util::deserialize_uint32(INDEX_TOTAL_LENGTH,buf);
    std::copy(buf.begin()+INDEX_REMOTE_ADDRESS,buf.begin()+INDEX_REMOTE_ADDRESS+REMOTE_ADDRESS_SIZE,this->remote_address.begin());
}

string arf_header::to_string(){
    string ret="seq_no: "+std::to_string(this->seq_no);
    ret+=" fragment_id: "+std::to_string(this->fragment_id);
    ret+=" num of fragments: "+std::to_string(this->num_of_fragments);
    ret+=" total length: "+std::to_string(this->total_length);
    ret+=" remote"+adhoc_util::to_string_iparray(this->remote_address);
    return ret;
}

void arf_packet::put_data(std::uint32_t index_begin, std::uint32_t index_end, const vector<std::uint8_t> &buf){
    this->payload.clear();
    this->payload.resize(index_end-index_begin);
    std::copy(buf.begin()+index_begin,buf.begin()+index_end,this->payload.begin());
}

void arf_packet::serialize(vector<std::uint8_t> &buf){
    this->fragment_length=this->get_fragment_length();
    if(buf.size()<this->fragment_length){
        buf.resize(this->fragment_length);
    }
    this->arf_header::serialize(buf);
    adhoc_util::serialize_uint32(INDEX_FRAGMENT_LENGTH,buf,this->fragment_length);
    std::copy(this->payload.begin(),this->payload.end(),buf.begin()+INDEX_PAYLOAD);
}

void arf_packet::deserialize(const vector<std::uint8_t> &buf){
    this->arf_header::deserialize(buf);
    this->fragment_length=adhoc_util::deserialize_uint32(INDEX_FRAGMENT_LENGTH,buf);
    this->payload.clear();
    this->payload.resize(this->fragment_length-FRAGMENT_LENGTH_SIZE-this->header_length());
    std::copy(buf.begin()+INDEX_PAYLOAD,buf.end(),this->payload.begin());
}
string arf_packet::to_string(){
    string ret=this->arf_header::to_string();
    this->fragment_length=this->get_fragment_length();
    ret+=" fragment length: "+std::to_string(this->fragment_length);
    ret+=" payload["+std::to_string(payload.at(0));
    for(size_t i=1;i<this->payload.size();i++){
        ret+=","+std::to_string(payload.at(i));
    }
    ret+="]";
    return ret;
}

int arf_packet::move_date(std::uint32_t index_begin, vector<std::uint8_t> &buf){
    std::copy(this->payload.begin(),this->payload.end(),buf.begin()+index_begin);
    return this->payload.size();
}

bool arf_portion::find_arf_packet_with_fragment_id(arf_packet& arfp){
    if(this->seq_no!=arfp.get_seq_no()){
        return false;
    }
    for (size_t i=0;i<this->storage.size();i++){
        if(this->storage.at(i).get_fragment_id()==arfp.get_fragment_id()){
            return true;
        }
    }
    return false;
}
int arf_portion::store_arf_packet(arf_packet &arfp){
    arf_packet arfp_t;
    arfp_t.set_arf_packet(arfp);
    this->storage.push_back(arfp_t);
    return this->storage.size();
}
void arf_portion::sort_arfp(){
    std::sort(this->storage.begin(),this->storage.end(),[](arf_packet &s1, arf_packet &s2){return s1.get_fragment_id()<s2.get_fragment_id();});
}
std::uint32_t arf_portion::combine_arfp(vector<std::uint8_t> &buf){
    this->sort_arfp();
    std::uint32_t index=0;
    buf.resize(this->total_length);
    for(size_t i=0;i<this->storage.size();i++){
        index+=this->storage.at(i).move_date(index,buf);
    }
    this->storage.clear();
    return index;
}
string arf_portion::to_string(){
    string ret=this->arf_header::to_string();
    ret+=" portion size: "+std::to_string(this->storage.size())+"\n";
    for(size_t i=0;i<this->storage.size();i++){
        ret+=this->storage.at(i).to_string()+"\n";
    }
    return ret;
}

void arf_manager::fragment(vector<arf_packet> &pkts,const vector<std::uint8_t> &buf){

    if(this->fragment_size<1){
        this->fragment_size=DEFAULT_FRAGMENT_SIZE;
    }
    int num=buf.size()/this->fragment_size;
    int mod=buf.size()%this->fragment_size;
    
    this->seq_no++;
    int8_t id=0;
    int index=0;
    pkts.clear();
    for(int i=0;i<num;i++){
        arf_packet arfp;
        arfp.set_seq_no(this->seq_no);
        id++;
        arfp.set_fragment_id(id);
        arfp.set_num_of_fratments(num+((mod==0)?0:1));
        arfp.set_total_length(buf.size());
        arfp.set_remote_address(this->own_id);
        arfp.put_data(index,index+this->fragment_size,buf);
        index+=this->fragment_size;
        pkts.push_back(arfp);
    }
    
    std::cerr<<"buf size:"<<buf.size()<<std::endl;
    std::cerr<<"fragment size:"<<this->fragment_size<<std::endl;
    std::cerr<<"buf size/fragment size:"<<std::to_string(buf.size()/this->fragment_size)<<std::endl;
    std::cerr<<"buf size%fragment size:"<<std::to_string(buf.size()%this->fragment_size)<<std::endl;

    if(mod==0){
        return;
    }

    arf_packet arfp;
    arfp.set_seq_no(this->seq_no);
    id++;
    arfp.set_fragment_id(id);
    arfp.set_num_of_fratments(num+1);
    arfp.set_total_length(buf.size());
    arfp.set_remote_address(this->own_id);
    std::cerr<<"fragment buf size: "<<std::to_string(buf.size())<<std::endl;
    arfp.put_data(index,buf.size(),buf);
    std::cerr<<"fragment arfp:"<<arfp.to_string()<<std::endl;
    pkts.push_back(arfp);

}

int arf_manager::store_arfp(arf_packet &arfp){
    arf_portion *arf_p=nullptr;
    for(size_t i=0;i<this->storage.size();i++){
        arf_p=&(this->storage.at(i));
        bool seq=arf_p->get_seq_no()==arfp.get_seq_no();
        if((seq=arf_p->get_seq_no()==arfp.get_seq_no())&&
            std::equal(arf_p->get_remote_address()->begin(),arf_p->get_remote_address()->end(),
                arfp.get_remote_address()->begin())){
            if(!arf_p->find_arf_packet_with_fragment_id(arfp)){
                arf_p->store_arf_packet(arfp);
            }
            return i;
        }
    }
    arf_portion arf_pt;
    arf_pt.set_arf_header(arfp);
    arf_pt.store_arf_packet(arfp);
    this->storage.push_back(arf_pt);
    return this->storage.size()-1;
}

int arf_manager::defragment(arf_packet &arfp, vector<std::uint8_t> &buf){
    int ret=0;
    int stored_index=this->store_arfp(arfp);
    if(this->storage.at(stored_index).storage_size()==arfp.get_num_of_fragments()){
        ret=this->storage.at(stored_index).combine_arfp(buf);
        this->storage.erase(this->storage.begin()+stored_index);
    }
    return ret;
}

string arf_manager::to_string(){
    string ret="current seq no: "+std::to_string(this->seq_no);
    ret+=" fragment size:"+std::to_string(this->fragment_size)+"\n";
    for(size_t i=0;i<this->storage.size();i++){
        ret+=this->storage.at(i).to_string()+"\n";
    }
    return ret;
}