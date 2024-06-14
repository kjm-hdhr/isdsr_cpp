#ifndef MATRIX_CALC_HPP
#define MATRIX_CALC_HPP

#include <cstdint>
#include <array>
#include <utility>
#include <string>
#include "../adhoc/adhoc_util.hpp"
namespace oit::ist::nws::adhoc_routing{

using namespace std;
std::uint64_t invmod (std::uint64_t a, std::uint64_t p);
std::int32_t multiply(std::int32_t a, std::int32_t b, int p);
class mat33_calc{
    protected:
    array<array<std::int32_t,3>,3> mat;
    std::int32_t p;
    //std::int32_t mat[3][3];
    std::int32_t mat33_det_elem(std::int32_t a, std::int32_t b, std::int32_t c, std::int32_t p);
    public:
    mat33_calc(){}
    ~mat33_calc(){}
    string to_string();
    void initialize();
    void set_array(std::int32_t v[3][3]);
    void set_array(array<array<std::int32_t,3>,3> &mat33);
    void set(std::int32_t r, std::int32_t c, std::int32_t v);
    std::int32_t get(std::int32_t r, std::int32_t c);
    std::int32_t get_det(std::int32_t p);
    array<array<std::int32_t,3>,3>* get_mat(){return &(this->mat);}
};
class mat44_calc{
    protected:
    //array<std::std::int32_t,16> mat;
    array<array<std::int32_t,4>,4> mat;
    
    std::int32_t p;
    mat33_calc mat33c;
    void make_cofactor_matrix(std::int32_t r, std::int32_t c,array<array<std::int32_t,3>,3> &mat33);
    std::int32_t get_cofactor_det(std::int32_t r, std::int32_t c, std::int32_t p);
    public:
    void set_array(std::int32_t v[4][4]);
    void set_array(array<array<std::int32_t,4>,4> &mat44);
    void set(std::int32_t r, std::int32_t c, std::int32_t v);
    std::int32_t get(std::int32_t r, std::int32_t c);
    std::int32_t get_det(std::int32_t p);
    void make_adjugate_mat44(std::int32_t p,array<array<std::int32_t,4>,4> &adj_mat);
    
    
    
// mat33[9]={11,12,13,21,22,23,31,32,33}
// (r-1)*3+(c-1)
// r1c1=0 r1c2=1 r1c3=2
// r2c1=3 r2c2=4 r2c3=5
// r3c1=6 r3c2=7 r3c3=8
//
// mat44[16]={11,12,13,14,21,22,23,24,31,32,33,34,41,42,43,44}
// (r-1)*4+(c-1)
// r1c1= 0 r1c2= 1 r1c3= 2 r1c4= 3
// r2c1= 4 r2c2= 5 r2c3= 6 r2c4= 7
// r3c1= 8 r3c2= 9 r3c3=10 r3c4=11
// r4c1=12 r4c2=13 r4c3=14 r4c4=15

void make_cofactor_matrix(
	std::int32_t mat44[16], std::int32_t mat33[9], int r, int c);

std::int32_t make_adjugate_matrix_elem(
	int r, int c, std::int32_t mat44[16], int p);

void make_adjugate_mat44(std::int32_t a[16], std::int32_t mat44[16],int p);
std::int32_t mat44_det(std::int32_t mat44[16], std::int32_t p);

void multipy_elem(int row, int col, std::int32_t a[16], std::int32_t b[16], std::int32_t c[16], int p);
void multiply_mat(std::int32_t a[16], std::int32_t b[16], std::int32_t c[16], int p);

};
};
#endif