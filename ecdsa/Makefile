root_dir = ..

adhoc_rt_dir = $(root_dir)/adhoc
dsr_rt_dir = $(root_dir)/dsr
isdsr_rt_dir = $(root_dir)/isdsr
ecdsa_rt_dir = $(root_dir)/ecdsa

adhoc_rt = $(adhoc_rt_dir)/adhoc_routing_fragment.cpp $(adhoc_rt_dir)/adhoc_routing_packet.cpp $(adhoc_rt_dir)/adhoc_node.cpp $(adhoc_rt_dir)/adhoc_node_exp.cpp
dsr_rt = $(dsr_rt_dir)/dsr_routing.cpp $(dsr_rt_dir)/dsr_packet.cpp
isdsr_rt = $(isdsr_rt_dir)/isdsr_packet.cpp $(isdsr_rt_dir)/isdsr_routing.cpp
ecdsa_rt = $(ecdsa_rt_dir)/ecdsa_sig.cpp 

mcl_root_dir = ../../mcl
mcl_include = $(mcl_root_dir)/include
ssl_include = /opt/homebrew/opt/openssl@3/include
mcl_lib = $(mcl_root_dir)/lib
ssl_lib = /opt/homebrew/opt/openssl@3/lib
flags = -lssl -lcrypto -lmclbn384_256 -lmcl -lpthread

ecdsa_test: $(adhoc_rt) $(dsr_rt) $(isdsr_rt) $(ecdsa_rt)
	g++ -o ecdsa_test $(adhoc_rt) $(dsr_rt) $(isdsr_rt) $(ecdsa_rt) $(ecdsa_rt_dir)/ecdsa_test.cpp -std=c++17 -I$(mcl_include) -L$(mcl_lib) $(flags)
ecdsa_node: $(adhoc_rt) $(dsr_rt) $(isdsr_rt) $(ecdsa_rt)
	g++ -o ecdsa_node $(adhoc_rt) $(dsr_rt) $(isdsr_rt) $(ecdsa_rt) $(ecdsa_rt_dir)/ecdsa_main.cpp -std=c++17 -I$(mcl_include) -L$(mcl_lib) $(flags)