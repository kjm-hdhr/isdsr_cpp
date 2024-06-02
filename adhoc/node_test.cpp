#include <iostream>
#include "adhoc_node.hpp"

using namespace oit::ist::nws::adhoc_routing;
using namespace std;

int main(int argc, char** argv){
    string if_name="eth0";
    adhoc_node an(if_name);
    cout<<"node "<<an.to_string()<<std::endl;

}