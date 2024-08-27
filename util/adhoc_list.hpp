#ifndef ADHOC_LIST_HPP
#define ADHOC_LIST_HPP
#include <cstdint>
#include <memory>
#include <string>
#include "../adhoc/adhoc_routing_constants.hpp"
#include "adhoc_id_util.hpp"

namespace oit::ist::nws::adhoc_routing{

    class ri_list_node{
    
    protected:
        std::shared_ptr<ri_list_node> prev;
        std::shared_ptr<ri_list_node> next;
        std::uint8_t id[ADDR_SIZE];
    public:
        ri_list_node(){this->prev=nullptr;this->next=nullptr;};
        ~ri_list_node(){this->clear();};
        void clear(){this->prev=nullptr;this->next=nullptr;};
        void set_id(std::uint8_t *id){for(int i=0;i<ADDR_SIZE;i++){this->id[i]=id[i];}};
        void set_next(std::shared_ptr<ri_list_node> next){this->next=next;};
        void set_prev(std::shared_ptr<ri_list_node> prev){this->prev=prev;};
        std::shared_ptr<ri_list_node> get_next(){return this->next;};
        std::shared_ptr<ri_list_node> get_prev(){return this->prev;};
        std::uint8_t* get_id(){return id;};
        
    };
    class ri_list{
        protected:
        std::shared_ptr<ri_list_node> head;
        std::shared_ptr<ri_list_node> tail;
        std::uint8_t size;
        public:
        ri_list(){size=0; head=std::make_shared<ri_list_node>();tail=head;};
        ~ri_list(){this->clear();};
        void add_id(std::uint8_t *id){
            size++;

            auto tmp=std::make_shared<ri_list_node>();
            tmp->set_id(id);
            tail->set_next(tmp);
            tmp->set_prev(tail);
            tail=tmp;
        };
        std::uint8_t* get_id(int index){
            if(index>=size){
                return nullptr;
            }
            std::shared_ptr<ri_list_node> tmp=head;
            for(int i=0;i<index;i++){
                tmp=tmp->get_next();
            }
            return tmp->get_next()->get_id();
        };

        std::uint8_t get_size(){return size;};

        void clear(){
            if(head==tail){
                return;
            }
            std::shared_ptr<ri_list_node> tmp;
            while(head!=tail){
                tmp=tail->get_prev();
                tail->clear();
                tail=tmp;
            }
        }

        std::string to_string(){
            std::string ret="[";
            if(head==tail){
                return ret+"]";
            }
            std::shared_ptr<ri_list_node> tmp=head->get_next();
            int i=0;
            ret+=std::to_string(i)+":"+id_util::id_to_string(tmp->get_id());
            while(tmp!=tail){
                tmp=tmp->get_next();
                i++;
                ret+=","+std::to_string(i)+":"+id_util::id_to_string(tmp->get_id());
            }
            return ret+"]";
        }
    };
};
#endif