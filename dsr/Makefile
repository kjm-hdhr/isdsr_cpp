adhoc_rt_dir = ../adhoc

adhoc_rt_node = $(adhoc_rt_dir)/adhoc_routing_fragment.cpp $(adhoc_rt_dir)/adhoc_routing_packet.cpp $(adhoc_rt_dir)/adhoc_routing.cpp $(adhoc_rt_dir)/adhoc_node.cpp $(adhoc_rt_dir)/adhoc_node_exp.cpp 
dsr_rt_node = dsr_packet.cpp dsr_routing.cpp

dsr_src = dsr_packet.cpp

dsr_pt_src = $(adhoc_rt_dir)/adhoc_routing_packet.cpp dsr_packet.cpp dsr_packet_test.cpp

dsr_pt: $(dsr_pt_src)
	g++ -o dsr_pt $(dsr_pt_src)

dsr_node: $(adhoc_rt_node) $(dsr_rt_node)  dsr_node.cpp
	g++ -o dsr_node $(adhoc_rt_node) $(dsr_rt_node) dsr_node.cpp -std=c++17 -lpthread

dsr_exp_rtt: $(adhoc_rt_node) $(dsr_rt_node)
	g++ -o dsr_exp_rtt $(adhoc_rt_node) $(dsr_rt_node)  dsr_exp_rtt.cpp -std=c++17 -lpthread

clean:
	rm dsr_node dsr_exp_rtt dsr_pt
