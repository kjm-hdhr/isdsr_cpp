root_dir = ..

adhoc_rt_dir = $(root_dir)/adhoc
dsr_rt_dir = $(root_dir)/dsr
isdsr_rt_dir = $(root_dir)/isdsr
ecdsa_rt_dir = $(root_dir)/ecdsa

adhoc_rt = $(adhoc_rt_dir)/adhoc_routing_fragment.cpp $(adhoc_rt_dir)/adhoc_routing_packet.cpp $(adhoc_rt_dir)/adhoc_routing.cpp $(adhoc_rt_dir)/adhoc_node.cpp $(adhoc_rt_dir)/adhoc_node_exp.cpp
dsr_rt = $(dsr_rt_dir)/dsr_routing.cpp $(dsr_rt_dir)/dsr_packet.cpp
isdsr_rt = $(isdsr_rt_dir)/isdsr_packet.cpp $(isdsr_rt_dir)/isdsr_routing.cpp $(isdsr_rt_dir)/signature_scheme.cpp
ecdsa_rt = $(ecdsa_rt_dir)/ecdsa_sig.cpp 

mcl_root_dir = ../../mcl
mcl_include = $(mcl_root_dir)/include
ssl_include = /opt/homebrew/opt/openssl@3/include
mcl_lib = .
ssl_lib = /opt/homebrew/opt/openssl@3/lib
flags = -lssl -lcrypto -lmclbn384_256 -lmcl -lpthread

mcl_libs:
	clang++ -c $(mcl_root_dir)/src/fp.cpp -std=c++03 -O3 -fpic -fno-exceptions -fno-threadsafe-statics -fno-rtti -fno-stack-protector -fpic -I $(mcl_root_dir)/include -DNDEBUG -DMCL_STANDALONE -DMCL_SIZEOF_UNIT=8 -DMCL_MAX_BIT_SIZE=384 -D_FORTIFY_SOURCE=0 -DMCL_USE_LLVM=1 
	clang++ -c $(mcl_root_dir)/src/base64.ll -std=c++03 -O3 -fpic -fno-exceptions -fno-threadsafe-statics -fno-rtti -fno-stack-protector -fpic -I $(mcl_root_dir)/include -DNDEBUG -DMCL_STANDALONE -DMCL_SIZEOF_UNIT=8 -DMCL_MAX_BIT_SIZE=384 -D_FORTIFY_SOURCE=0 -DMCL_USE_LLVM=1 
	clang++ -c $(mcl_root_dir)/src/bint64.ll -std=c++03 -O3 -fpic -fno-exceptions -fno-threadsafe-statics -fno-rtti -fno-stack-protector -fpic -I $(mcl_root_dir)/include -DNDEBUG -DMCL_STANDALONE -DMCL_SIZEOF_UNIT=8 -DMCL_MAX_BIT_SIZE=384 -D_FORTIFY_SOURCE=0 -DMCL_USE_LLVM=1
	ar rv libmcl.a fp.o base64.o bint64.o
	clang++ -c $(mcl_root_dir)/src/bn_c384_256.cpp -std=c++03 -O3 -fpic -fno-exceptions -fno-threadsafe-statics -fno-rtti -fno-stack-protector -fpic -I $(mcl_root_dir)/include -DNDEBUG -DMCL_STANDALONE -DMCL_SIZEOF_UNIT=8 -DMCL_MAX_BIT_SIZE=384 -D_FORTIFY_SOURCE=0 -DMCL_USE_LLVM=1 
	ar rv libmclbn384_256.a bn_c384_256.o
	rm *.o

ecdsa_test: mcl_libs $(adhoc_rt) $(dsr_rt) $(isdsr_rt) $(ecdsa_rt)
	g++ -g -o ecdsa_test $(adhoc_rt) $(dsr_rt) $(isdsr_rt) $(ecdsa_rt) $(ecdsa_rt_dir)/ecdsa_test.cpp -std=c++17 $(flags) -I$(mcl_include) -L$(mcl_lib) 
ecdsa_node: mcl_libs $(adhoc_rt) $(dsr_rt) $(isdsr_rt) $(ecdsa_rt)
	g++ -g -O0 -o ecdsa_node $(adhoc_rt) $(dsr_rt) $(isdsr_rt) $(ecdsa_rt) $(ecdsa_rt_dir)/ecdsa_main.cpp -std=c++17 $(flags) -I$(mcl_include) -L$(mcl_lib) 
ecdsa_exp_rtt: mcl_libs $(adhoc_rt) $(dsr_rt) $(isdsr_rt) $(ecdsa_rt)
	g++ -O2 -o ecdsa_exp_rtt $(adhoc_rt) $(dsr_rt) $(isdsr_rt) $(ecdsa_rt) $(ecdsa_rt_dir)/ecdsa_main_exp_rtt.cpp -std=c++17 $(flags) -I$(mcl_include) -L$(mcl_lib) 

clean:
	rm ecdsa_test ecdsa_node ecdsa_exp_rtt libmcl.a libmclbn384_256.a