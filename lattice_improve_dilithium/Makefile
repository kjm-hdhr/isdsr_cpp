CXX = g++
CC = gcc

root_dir = ..

adhoc_rt_dir = $(root_dir)/adhoc
dsr_rt_dir = $(root_dir)/dsr
isdsr_rt_dir = $(root_dir)/isdsr
lattice_rt_dir = $(root_dir)/lattice_improve_dilithium

adhoc_rt = $(adhoc_rt_dir)/adhoc_routing_fragment.cpp $(adhoc_rt_dir)/adhoc_routing_packet.cpp $(adhoc_rt_dir)/adhoc_node.cpp $(adhoc_rt_dir)/adhoc_node_exp.cpp
dsr_rt = $(dsr_rt_dir)/dsr_routing.cpp $(dsr_rt_dir)/dsr_packet.cpp
isdsr_rt = $(isdsr_rt_dir)/isdsr_packet.cpp $(isdsr_rt_dir)/isdsr_routing.cpp $(isdsr_rt_dir)/signature_scheme.cpp
#lattice_rt = $(lattice_rt_dir)/matrix_calc.cpp $(lattice_rt_dir)/polyvec.cpp  $(lattice_rt_dir)/lattice_sig.cpp 
lattice_rt = $(lattice_rt_dir)/matrix.cpp $(lattice_rt_dir)/polyvec_op.cpp  $(lattice_rt_dir)/lattice_sig.cpp 

dilithium_root_dir = ../../dilithium
dilithium_include = $(dilithium_root_dir)/ref
dilithium_src = $(dilithium_root_dir)/ref
dilithium_sources = $(dilithium_root_dir)/ref/sign.c $(dilithium_root_dir)/ref/packing.c \
				$(dilithium_root_dir)/ref/polyvec.c $(dilithium_root_dir)/ref/poly.c $(dilithium_root_dir)/ref/ntt.c \
				$(dilithium_root_dir)/ref/reduce.c $(dilithium_root_dir)/ref/rounding.c $(dilithium_root_dir)/ref/randombytes.c

dilithium_headers = $(dilithium_root_dir)/ref/config.h $(dilithium_root_dir)/ref/params.h \
					$(dilithium_root_dir)/ref/api.h $(dilithium_root_dir)/ref/sign.h \
					$(dilithium_root_dir)/ref/packing.h $(dilithium_root_dir)/ref/polyvec.h \
					$(dilithium_root_dir)/ref/poly.h $(dilithium_root_dir)/ref/ntt.h \
					$(dilithium_root_dir)/ref/reduce.h $(dilithium_root_dir)/ref/rounding.h \
					$(dilithium_root_dir)/ref/symmetric.h $(dilithium_root_dir)/ref/randombytes.h

dilithium_objects = sign.o packing.o polyvec.o poly.o ntt.o reduce.o rounding.o randombytes.o

ssl_include = /opt/homebrew/opt/openssl@3/include
dilithium_lib = .
ssl_lib = /opt/homebrew/opt/openssl@3/lib
flags = -L. -I$(dilithium_include) -ldilithium2_ref -lfips202_ref -lpthread


libfips202_ref.a: $(dilithium_src)/fips202.h $(dilithium_src)/fips202.c 
	$(CC) -O2 -c $(dilithium_src)/fips202.h $(dilithium_src)/fips202.c
	ar -r $@ fips202.o
	rm -f fips202.o

libdilithium2_ref.a: $(dilithium_headers) $(dilithium_sources) $(dilithium_root_dir)/ref/symmetric-shake.c
	$(CC) -O2 -c $(dilithium_headers) $(dilithium_sources) $(dilithium_root_dir)/ref/symmetric-shake.c
	ar -r $@ $(dilithium_objects) symmetric-shake.o
	rm -f *.o

libpqcrystals: libfips202_ref.a libdilithium2_ref.a

lattice_test: libpqcrystals $(adhoc_rt) $(dsr_rt) $(isdsr_rt) $(lattice_rt) $(lattice_rt_dir)/lattice_test.cpp
	g++ -g -O2 -Wall -o lattice_test $(adhoc_rt) $(dsr_rt) $(isdsr_rt) $(lattice_rt) $(lattice_rt_dir)/lattice_test.cpp -std=c++17 $(flags)
lattice_node: libpqcrystals $(adhoc_rt) $(dsr_rt) $(isdsr_rt) $(isdsr_rt) $(lattice_rt) $(lattice_rt_dir)/lattice_main.cpp
	g++ -O2 -Wall -g -o lattice_node $(adhoc_rt) $(dsr_rt) $(isdsr_rt) $(lattice_rt) $(lattice_rt_dir)/lattice_main.cpp -std=c++17 $(flags)

lattice_exp_rtt:libpqcrystals $(adhoc_rt) $(dsr_rt) $(isdsr_rt) $(isdsr_rt) $(lattice_rt) $(lattice_rt_dir)/lattice_main_exp_rtt.cpp
	g++  -Wall -o lattice_exp_rtt $(adhoc_rt) $(dsr_rt) $(isdsr_rt) $(lattice_rt) $(lattice_rt_dir)/lattice_main_exp_rtt.cpp -std=c++17 $(flags)

clean:
	rm *.o *.a lattice_sig_test lattice_test lattice_node lattice_exp_rtt
