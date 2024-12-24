#include <iostream>
#include "../adhoc/adhoc_node.hpp"
#include "../isdsr/isdsr_packet.hpp"
#include "../isdsr/isdsr_routing.hpp"
#include "lattice_sig.hpp"
#include <array>
#include <vector>
using namespace oit::ist::nws::adhoc_routing;

int main(int argc, char** argv){
    array<uint8_t,ADDR_SIZE> src={10,0,0,3};
    array<uint8_t,ADDR_SIZE> dest={10,0,0,10};


    int fnodes=8;
    isdsr_packet p(RREQ,src,dest);
    isdsr_routing irs[fnodes],ir_src,ir_dest;
    lattice_sig lsig[fnodes],lsig_src,lsig_dest;
    array<uint8_t,ADDR_SIZE> fid[fnodes];
    ir_src.set_id(src);
    ir_dest.set_id(dest);
    lsig_src.set_id(src);
    lsig_dest.set_id(dest);
    ir_src.set_signature_scheme(&lsig_src);
    ir_dest.set_signature_scheme(&lsig_dest);
    for(int i=0;i<fnodes;i++){
        fid[i][0]=10;
        fid[i][1]=0;
        fid[i][2]=0;
        fid[i][3]=i+100;
        irs[i].set_id(fid[i]);
        lsig[i].set_id(fid[i]);
        irs[i].set_signature_scheme(&(lsig[i]));
    }
    vector<uint8_t> buf;
    for(int i=0;i<100;i++){
        buf.clear();
        ir_src.generate_initial_request(dest,buf);
        for(int j=0;j<fnodes;j++){
            irs[j].packet_processing(buf);
        }
        ir_dest.packet_processing(buf);
    }
    /*
    for(int i=0;i<100;i++){
        for(int j=fnodes;j>0;j--){
            irs[j-1].packet_processing(buf);
        }
        ir_src.packet_processing(buf);
    }
    */
    /*
    isdsr_routing ir1,ir2,ir3;
    ir1.set_id(src);
    ir2.set_id(f1);
    ir3.set_id(f2);
    lattice_sig lattice1;
    lattice_sig lattice2;
    lattice_sig lattice3;
    lattice1.set_id(src);
    lattice2.set_id(f1);
    lattice3.set_id(f2);
    ir1.set_signature_scheme(&lattice1);
    ir2.set_signature_scheme(&lattice2);
    ir3.set_signature_scheme(&lattice3);
    vector<uint8_t> buf1;
    vector<uint8_t> buf2;
    ir1.generate_initiali_request(dest,buf1);
    ir2.packet_processing(buf1);
    ir3.packet_processing(buf1);
    */
}