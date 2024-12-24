root_dir = ..

CCX = g++
adhoc_rt_dir = $(root_dir)/adhoc
dsr_rt_dir = $(root_dir)/dsr
isdsr_rt_dir = $(root_dir)/isdsr
lattice_rt_dir = $(root_dir)/lattice_improve_dilithium
dilithium_dir=$(root_dir)/dilithium_src

dilithium_src = packing.c polyvec.c poly.c ntt.c reduce.c rounding.c fips202.c symmetric-shake.c aes256ctr.c symmetric-aes.c
dilithium_headers = config.h params.h api.h sign.h packing.h polyvec.h poly.h ntt.h \
  reduce.h rounding.h symmetric.h randombytes.h
adhoc_rt = $(adhoc_rt_dir)/adhoc_routing_fragment.cpp $(adhoc_rt_dir)/adhoc_routing_packet.cpp $(adhoc_rt_dir)/adhoc_node.cpp $(adhoc_rt_dir)/adhoc_node_exp.cpp
dsr_rt = $(dsr_rt_dir)/dsr_routing.cpp $(dsr_rt_dir)/dsr_packet.cpp
isdsr_rt = $(isdsr_rt_dir)/isdsr_packet.cpp $(isdsr_rt_dir)/isdsr_routing.cpp
#lattice_rt = $(lattice_rt_dir)/matrix_calc.cpp $(lattice_rt_dir)/polyvec.cpp  $(lattice_rt_dir)/lattice_sig.cpp 
lattice_rt = $(lattice_rt_dir)/matrix.cpp $(lattice_rt_dir)/polyvec_op.cpp  $(lattice_rt_dir)/lattice_sig.cpp 

ssl_include = /opt/homebrew/opt/openssl@3/include
ssl_lib = /opt/homebrew/opt/openssl@3/lib
flags = -lpthread



lattice_test: $(adhoc_rt) $(dsr_rt) $(isdsr_rt) $(lattice_rt) $(dilithium_src) $(dilithium_headers) $(lattice_rt_dir)/lattice_test.cpp
	$(CCX) -g -O2 -std=c++17 -Wall -o lattice_test $(dilithium_headers) $(dilithium_src) $(adhoc_rt) $(dsr_rt) $(isdsr_rt) $(lattice_rt) $(lattice_rt_dir)/lattice_test.cpp -std=c++17 -I$(dilithium_include) -L$(dilithium_lib) $(flags)
lattice_node: $(dilithium_src) $(adhoc_rt) $(dsr_rt) $(isdsr_rt) $(isdsr_rt) $(lattice_rt) $(lattice_rt_dir)/lattice_main.cpp
	g++ -O2 -o lattice_node $(adhoc_rt) $(dsr_rt) $(isdsr_rt) $(lattice_rt) $(dilithium_src) $(lattice_rt_dir)/lattice_main.cpp -std=c++17 -I$(dilithium_include) -L$(dilithium_lib) $(flags)

clean:
	rm lattice_sig_test lattice_test lattice_node
