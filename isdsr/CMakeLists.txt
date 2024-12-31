cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 17)
set(isdsr_src isdsr_packet.cpp isdsr_routing.cpp signature_scheme.cpp)
set(isdsr_header isdsr_packet.hpp isdsr_routing.hpp signature_scheme.hpp)

add_library(isdsr STATIC ${isdsr_header} ${isdsr_src})