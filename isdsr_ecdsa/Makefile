root_dir = ../..

adhoc_rt_dir = $(root_dir)/adhoc
dsr_rt_dir = $(root_dir)/dsr
isdsr_rt_dir = $(root_dir)/isdsr
ecdsa_rt_dir = $(root_dir)/isdsr/ecdsa

adhoc_rt = $(adhoc_rt_dir)/adhoc_routing_fragment.cpp $(adhoc_rt_dir)/adhoc_routing_packet.cpp $(adhoc_rt_dir)/adhoc_node.cpp
dsr_rt = $(dsr_rt_dir)/dsr_routing.cpp $(dsr_rt_dir)/dsr_packet.cpp
isdsr_rt = $(isdsr_rt_dir)/isdsr_packet.cpp $(isdsr_rt_dir)/isdsr_routing.cpp
ecdsr_rt = $(ecdsa_rt_dir)/ecdsa_cpp

mcl_include = ../../mcl/include
ssl_include = /opt/homebrew/opt/openssl@3/include
mcl_lib = ../../mcl/lib
ssl_lib = /opt/homebrew/opt/openssl@3/lib
flags = -lssl -lcrypto -lmclbn384_256 -lmcl -lpthread

flags = -lssh -lcrypto -lmcl -
dsr_node: $(adhoc_rt_node) $(dsr_rt_node)
	g++ -o dsr_node $(adhoc_rt_node) $(dsr_rt_node) -std=c++17 -lpthread