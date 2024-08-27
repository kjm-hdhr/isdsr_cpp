#include "polyvec_op.hpp"
#include <iostream>
using namespace oit::ist::nws::adhoc_routing;

void polyvec_op::poly_initialize(poly &p){
	for(int i=0;i<N;i++){
		p.coeffs[i]=0;
	}
}
void polyvec_op::polyvecl_initialize(polyvecl &p){
	for(int i=0;i<L;i++){
		poly_initialize(p.vec[i]);
	}
}
void polyvec_op::polyveck_initialize(polyveck &p){
	for(int i=0;i<K;i++){
		poly_initialize(p.vec[i]);
	}
}
void polyvec_op::copy_polyvecl(polyvecl& dest, const polyvecl& src){
  for(int i=0;i<L;i++){
    for(int j=0;j<N;j++){
      dest.vec[i].coeffs[j]=src.vec[i].coeffs[j];
    }
  }
}

void polyvec_op::copy_polyveck(polyveck& dest, const polyveck& src){
  for(int i=0;i<K;i++){
    for(int j=0;j<N;j++){
      dest.vec[i].coeffs[j]=src.vec[i].coeffs[j];
    }
  }
}
void polyvec_op::poly_multiply(poly &c, int32_t a, poly &b, int p){
	for(int i=0;i<N;i++){
        c.coeffs[i]=(((int64_t)a)*b.coeffs[i])%p;
	}
}
void polyvec_op::poly_pointwise(poly &c, poly &a, poly &b, int p){
	int i;
	for(i=0;i<N;i++){
        c.coeffs[i]=(((int64_t)(a.coeffs[i]))*b.coeffs[i])%p;
	}
}
void polyvec_op::polyvecl_pointwise_poly(polyvecl &c, poly &a, polyvecl &b, int p){
	int i;
	for(i=0;i<L;i++){
		poly_pointwise(c.vec[i],a,b.vec[i],p);
	}
}
void polyvec_op::polyveck_pointwise_poly(polyveck &c, poly &a, polyveck &b, int p){
	int i;
	for(i=0;i<K;i++){
		poly_pointwise(c.vec[i],a,b.vec[i],p);
	}
}
void polyvec_op::polyvec_pointwise(polyveck &pk, polyvecl mat[K], polyvecl &pl){
	int i,j;
	for(i=0;i<N;i++){
		for(j=0;j<K;j++){
			pk.vec[j].coeffs[i]=(
                ((((int64_t)(mat[j].vec[0].coeffs[i]))*pl.vec[0].coeffs[i])%Q)+
                ((((int64_t)(mat[j].vec[1].coeffs[i]))*pl.vec[1].coeffs[i])%Q)+
                ((((int64_t)(mat[j].vec[2].coeffs[i]))*pl.vec[2].coeffs[i])%Q)+
                ((((int64_t)(mat[j].vec[3].coeffs[i]))*pl.vec[3].coeffs[i])%Q)
			)%Q;
		}
	}
}
void polyvec_op::poly_add_mod(poly &c, poly &a, poly &b, int p){
	int i;
	for(i=0;i<N;i++){
		c.coeffs[i]=(((int64_t)(a.coeffs[i]))+b.coeffs[i])%p;
	}
}
void polyvec_op::polyvecl_add_mod(polyvecl& c, polyvecl& a, polyvecl& b, int p){
	int i;
	for(i=0;i<L;i++){
		poly_add_mod(c.vec[i],a.vec[i],b.vec[i],p);
	}
}
void polyvec_op::polyveck_add_mod(polyveck& c, polyveck& a, polyveck& b, int p){
	int i;
	for(i=0;i<K;i++){
		poly_add_mod(c.vec[i],a.vec[i],b.vec[i],p);
	}
}
void polyvec_op::poly_sub_mod(poly &c, poly &a, poly &b, int p){
	int i;
	for(i=0;i<N;i++){
		c.coeffs[i]=(((int64_t)(a.coeffs[i]))-b.coeffs[i])%p;
	}
}
void polyvec_op::polyvecl_sub_mod(polyvecl& c, polyvecl& a, polyvecl& b, int p){
	int i;
	for(i=0;i<L;i++){
		poly_sub_mod(c.vec[i],a.vec[i],b.vec[i],p);
	}
}
void polyvec_op::polyveck_sub_mod(polyveck& c, polyveck& a, polyveck& b, int p){
	int i;
	for(i=0;i<K;i++){
		poly_sub_mod(c.vec[i],a.vec[i],b.vec[i],p);
	}
}
int polyvec_op::compare_polyveck(polyveck &a, polyveck &b){
  for(int i=0;i<K;i++){
    for(int j=0;j<N;j++){
        

      if(a.vec[i].coeffs[j]!=b.vec[i].coeffs[j]){
        std::cout<<"i:"<<std::to_string(i);
        std::cout<<" j:"<<std::to_string(j);
        std::cout<<"=("<<std::to_string(a.vec[i].coeffs[j]);
        std::cout<<","<<std::to_string(b.vec[i].coeffs[j])<<") ";
        return 0;
	  }
    }
  }
	return 1;
}

int polyvec_op::compare_polyvecl(polyvecl &a, polyvecl &b){
  for(int i=0;i<L;i++){
    for(int j=0;j<N;j++){
      if(a.vec[i].coeffs[j]!=b.vec[i].coeffs[j]){
		return 0;
      }
    }
  }
	return 1;
}