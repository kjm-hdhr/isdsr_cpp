#include "matrix_calc.hpp"
#include "../adhoc/adhoc_util.hpp"
#include <iostream>
using namespace oit::ist::nws::adhoc_routing;

string mat33_calc::to_string(){
	int len=3;
	string ret="[";
	for(int i=0;i<len;i++){
		ret+="["+std::to_string(this->mat.at(i).at(0));
		for(int j=1;j<len;j++){
			ret+=","+std::to_string(this->mat.at(i).at(j));
		}
		ret+="]\n";
	}
    return ret+"]";
}
void mat33_calc::initialize(){
	for(int i=0;i<3;i++){
		this->mat.at(i).fill(0);
	}
}
void mat33_calc::set_array(int32_t v[3][3]){
	for(int i=0;i<3;i++){
		for(int j=0;j<3;j++){
			this->mat.at(i).at(j)=v[i][j];
		}
	}
}
void mat33_calc::set_array(array<array<int32_t,3>,3> &mat33){
	for(int i=0;i<3;i++){
		std::copy(mat33.at(i).begin(),mat33.at(i).end(),this->mat.at(i).begin());
	}
}
void mat33_calc::set(int32_t r, int32_t c, int32_t v){
	this->mat.at(r-1).at(c-1)=v;
}
int32_t mat33_calc::get(int32_t r, int32_t c){
	return this->mat.at(r-1).at(c-1);
}
int32_t mat33_calc::mat33_det_elem(int32_t a, int32_t b, int32_t c, int32_t p){
	//printf("%d x %d x %d = ",a,b,c);
	int64_t ret=((int64_t)a*b)%p;
	ret=(ret*c)%p;
	//printf("%d\n",ret);
	return ret;
}
int32_t mat33_calc::get_det(int32_t p){
	int64_t det=0;
	// r1c1*r2c2*r3c3
	det=mat33_det_elem(this->mat[0][0],this->mat[1][1],this->mat[2][2],p);
	// r1c2*r2c3*r3c1
	det=(det+mat33_det_elem(this->mat[0][0],this->mat[1][1],this->mat[2][2],p))%p;
	// r1c3*r2c1*r3c2
	det=(det+mat33_det_elem(this->mat[0][2],this->mat[1][0],this->mat[2][1],p))%p;
	// r1c3*r2c2*r3c1
	det=(det-mat33_det_elem(this->mat[0][2],this->mat[1][1],this->mat[2][0],p))%p;
	// r1c2*r2c1*r3c3
	det=(det-mat33_det_elem(this->mat[0][1],this->mat[1][0],this->mat[2][2],p))%p;
	// r1c1*r2c3*r3c2
	det=(det-mat33_det_elem(this->mat[0][0],this->mat[1][2],this->mat[2][1],p))%p;
	return det%p;
}
void mat44_calc::set_array(int32_t v[4][4]){
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			this->mat.at(i).at(j)=v[i][j];
		}
	}
}
void mat44_calc::set_array(array<array<int32_t,4>,4> &mat44){
	for(int i=0;i<4;i++){
		std::copy(mat44.at(i).begin(),mat44.at(i).end(),this->mat.at(i).begin());
	}
}
void mat44_calc::set(int32_t r, int32_t c, int32_t v){
	this->mat.at(r-1).at(c-1)=v;
}
int32_t mat44_calc::get(int32_t r, int32_t c){
	return this->mat.at(r-1).at(c-1);
}
void mat44_calc::make_cofactor_matrix(int32_t r, int32_t c,array<array<int32_t,3>,3> &mat33){
	
	int mat33_r=0;
	int mat33_c=0;
	for(int i=1;i<=4;i++){//row
		if(i==r){
			continue;
		}
		mat33_c=0;
		for(int j=1;j<=4;j++){//col
			if(j==c){
				continue;
			}
			//std::cout<<"mat33r:"<<std::to_string(mat33_r)<<" mat33c:"<<std::to_string(mat33_c)<<std::endl;
			mat33.at(mat33_r).at(mat33_c)=this->mat.at(i-1).at(j-1);
			mat33_c++;
		}
		mat33_r++;
	}
}
int32_t mat44_calc::get_cofactor_det(int32_t r, int32_t c, int32_t p){
	//std::cout<<"get cofactor det1"<<std::endl;
	this->make_cofactor_matrix(r,c,*(this->mat33c.get_mat()));
	//std::cout<<"get cofactor det2"<<std::endl;
	return this->mat33c.get_det(p);
}
int32_t mat44_calc::get_det(int32_t p){
	int64_t a=0;
	int64_t ret=0;
	for(int i=1;i<=4;i++){
		a=this->get(1,i);
		a=(a*this->get_cofactor_det(1,i,p))%p;
		ret=(ret+((i%2==0)?a:(-1)*a))%p;
	}
	return ret;
}
void mat44_calc::make_adjugate_mat44(int32_t p,array<array<int32_t,4>,4> &adj_mat){
	int32_t mat44_element;
	int i,j;
	for(i=1;i<=4;i++){
		for(j=1;j<=4;j++){
			mat44_element=this->get_cofactor_det(i,j,p);
			mat44_element=((i+j)%2==0)?mat44_element:mat44_element*(-1);
			adj_mat.at(i-1).at(j-1)=mat44_element;
		}
	}
}

uint64_t oit::ist::nws::adhoc_routing::invmod (uint64_t a, uint64_t p)
{
  uint64_t j = 1, i = 0, b = p, c = a, x, y;
  while (c != 0)
  {
    x = b / c;
    y = b - x * c;
    b = c; 
    c = y;
    y = j;
    j = i - j * x;
    i = y;
  }
  if ((int64_t)i < 0)
    i += p;
  return i;
}
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

int32_t oit::ist::nws::adhoc_routing::multiply(int32_t a, int32_t b, int p){
	int64_t ret=a;
	ret=(ret*b)%p;
	return ret;
}