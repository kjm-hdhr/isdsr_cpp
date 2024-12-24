#include "matrix.hpp"

using namespace oit::ist::nws::adhoc_routing;
matrix::matrix(){}
matrix::matrix(int d){this->d=d;mat=std::make_unique<int[]>(d*d);}
matrix::~matrix(){}
void matrix::clear(){
	for(int i=0;i<d*d;i++){
		this->mat[i]=0;
	}
}
void matrix::put(int r, int c, std::int32_t v){
	this->mat[(r-1)*d+(c-1)]=v;
}
std::int32_t matrix::get(int r, int c){
	return this->mat[(r-1)*d+(c-1)];
}
std::string matrix::to_string(){

	std::string ret="mat"+std::to_string(d)+std::to_string(d)+"=[\n";
	for(int i=0;i<d;i++){
		ret+=std::to_string(mat[i*d]);
		for(int j=1;j<d;j++){
			ret+=","+std::to_string(mat[i*d+j]);
		}
		ret+=std::string("\n");
		
	}
	return ret+"]\n";
}
std::int32_t matrix33::det_elem(
	std::int32_t a, std::int32_t b, std::int32_t c, std::int32_t p
){
	//printf("%d x %d x %d = ",a,b,c);
	std::int64_t ret=((std::int64_t)a*b)%p;
	ret=(ret*c)%p;
	//printf("%d\n",ret);
	return ret;
}
std::int32_t matrix33::det(std::int32_t p){
	std::int64_t det=0;
	// r1c1*r2c2*r3c3
	det=this->det_elem(this->mat[0],this->mat[4],this->mat[8],p);
	// r1c2*r2c3*r3c1
	det=(det+this->det_elem(this->mat[1],this->mat[5],this->mat[6],p))%p;
	// r1c3*r2c1*r3c2
	det=(det+this->det_elem(this->mat[2],this->mat[3],this->mat[7],p))%p;
	// r1c3*r2c2*r3c1
	det=(det-this->det_elem(this->mat[2],this->mat[4],this->mat[6],p))%p;
	// r1c2*r2c1*r3c3
	det=(det-this->det_elem(this->mat[1],this->mat[3],this->mat[8],p))%p;
	// r1c1*r2c3*r3c2
	det=(det-this->det_elem(this->mat[0],this->mat[5],this->mat[7],p))%p;
	return det%p;
}

void matrix44::cofactor_matrix(int r, int c, matrix33 &m33){
	int c33=1,r33=1;
	
	for(int r44=1;r44<=this->d;r44++){
		if(r44==r){
			continue;
		}
		for(int c44=1;c44<=this->d;c44++){
			if(c44==c){
				continue;
			}
			m33.put(r33,c33,this->get(r44,c44));
			c33++;
		}
		c33=1;
		r33++;
	}
	
}

std::int32_t matrix44::det(std::int32_t p){
	std::int64_t a;
	std::int64_t ret;
	matrix33 m33;
	this->cofactor_matrix(1,1,m33);
	a=this->get(1,1);
	a=(a*m33.det(p))%p;
	ret=a;

	this->cofactor_matrix(1,2,m33);
	a=this->get(1,2);
	a=(a*m33.det(p))%p;
	ret=(ret-a)%p;

	this->cofactor_matrix(1,3,m33);
	a=this->get(1,3);
	a=(a*m33.det(p))%p;
	ret=(ret+a)%p;

	this->cofactor_matrix(1,4,m33);
	a=this->get(1,4);
	a=(a*m33.det(p))%p;
	ret=(ret-a)%p;

	return ret;
}
void matrix44::adjugate_matrix(matrix44 &m44,std::int32_t p){
	std::int32_t adj44_elem;
	matrix33 m33;
	for(int r44=1;r44<=this->d;r44++){
		for(int c44=1;c44<=this->d;c44++){
			this->cofactor_matrix(r44,c44,m33);
			adj44_elem=((r44+c44)%2==0)?m33.det(p):m33.det(p)*(-1);
			m44.put(c44,r44,adj44_elem);
		}
	}
}