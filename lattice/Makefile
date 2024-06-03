dsr_dir = ../dsr
dsr_src = $(dsr_dir)/dsr.c $(dsr_dir)/dsr_packet.c
isdsr_dir = ../isdsr
isdsr_src = $(isdsr_dir)/isdsr_packet.c $(isdsr_dir)/isdsr.c
include_dir = ../../dilithium/ref
lib_dir = ../../dilithium/build/ref
flags = -ldilithium2_ref -lfips202_ref -lpthread

isdsr_lattice = sign_agg.c packing_agg.c matrix.c isdsr_lattice.c
isdsr_node = $(dsr_src) $(isdsr_src) $(isdsr_lattice) $(isdsr_dir)/isdsr_node.c
isdsr_node_test = $(dsr_src) $(isdsr_src) $(isdsr_lattice) isdsr_node_test.c
isdsr_lattice_test = $(dsr_src) $(isdsr_src) $(isdsr_lattice) isdsr_lattice_test.c

isdsr_rtt_sender = $(dsr_src) $(isdsr_src) $(isdsr_lattice) $(isdsr_dir)/isdsr_node_rtt_sender.c

isdsr_rtt_rcv = $(dsr_src) $(isdsr_src) $(isdsr_lattice) $(isdsr_dir)/isdsr_node_rtt.c

node: $(isdsr_node) 
	gcc -Wall -o isdsr_node -I$(include_dir) -L$(lib_dir) $(isdsr_node) $(flags)

node_test: $(isdsr_node_test)
	gcc -Wall -o isdsr_node_test $(isdsr_node_test) $(flags)

lattice_test:
	gcc -Wall -o isdsr_lattice_test -I$(include_dir) -L$(lib_dir)  $(isdsr_lattice_test) $(flags)

rtt_receiver: $(isdsr_rtt_rcv)
	gcc -Wall -o isdsr_rtt_rcv -I$(include_dir) -L$(lib_dir)  $(isdsr_rtt_rcv) $(flags)

rtt_client: $(isdsr_rtt_sender)
	gcc -g -Wall -o isdsr_node_test_rtt -I$(include_dir) -L$(lib_dir)  $(isdsr_rtt_sender) $(flags)
