#include <iostream>
#include "adhoc_list.hpp"

using namespace oit::ist::nws::adhoc_routing;
int main(int argc, char** argv){
    ri_list rl;
    std::uint8_t id1[ADDR_SIZE]{0,0,0,1};
    std::uint8_t id2[ADDR_SIZE]{0,0,0,2};
    std::uint8_t id3[ADDR_SIZE]{0,0,0,3};
    std::uint8_t id4[ADDR_SIZE]{0,0,0,4};
    std::uint8_t id5[ADDR_SIZE]{0,0,0,5};
    std::cout<<"ri list:"<<rl.to_string()<<std::endl;
    rl.add_id(id1);
    std::cout<<"ri list:"<<rl.to_string()<<std::endl;
    rl.add_id(id2);
    std::cout<<"ri list:"<<rl.to_string()<<std::endl;
    rl.add_id(id3);
    std::cout<<"index 1:"<<id_util::id_to_string(rl.get_id(1))<<std::endl;
    std::cout<<"ri list:"<<rl.to_string()<<std::endl;
    rl.add_id(id4);
    std::cout<<"ri list:"<<rl.to_string()<<std::endl;
    rl.clear();
    std::cout<<"ri list:"<<rl.to_string()<<std::endl;

    return 0;
}