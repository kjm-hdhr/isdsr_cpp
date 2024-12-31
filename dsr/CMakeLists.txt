cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 17)
set(dsr_src dsr_packet.cpp dsr_routing.cpp)
set(dsr_header dsr_packet.hpp dsr_routing.hpp)

add_library(dsr STATIC ${dsr_header} ${dsr_src})