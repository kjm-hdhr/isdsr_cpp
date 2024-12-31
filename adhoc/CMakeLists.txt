cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 17)
set(adhoc_src
adhoc_routing_packet.cpp
adhoc_routing_fragment.cpp
adhoc_routing.cpp
adhoc_node.cpp
)

set(adhoc_header 
adhoc_util.hpp
adhoc_routing.hpp
adhoc_routing_packet.hpp
adhoc_routing_fragment.hpp
adhoc_routing_constants.hpp
adhoc_node.hpp
)

add_library(adhoc STATIC ${adhoc_header} ${adhoc_src})