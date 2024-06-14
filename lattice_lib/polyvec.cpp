#include "polyvec.hpp"

using namespace oit::ist::nws::adhoc_routing;

static std::uint32_t multiply( std::int32_t a, std::int32_t b, std::int32_t p){
    std::int64_t ret=a;
	ret=(ret*b)%p;
	return ret;
}
void poly_operation::poly_initialize(poly &p){
	int i;
	for(i=0;i<N;i++){
		p.coeffs[i]=0;
	}
}
void poly_operation::poly_pointwise(poly &c, poly &a, poly &b, std::int32_t p){
    int i;
	for(i=0;i<N;i++){
        c.coeffs[i]=multiply(a.coeffs[i],b.coeffs[i],p);
	}
}
void poly_operation::poly_add_mod(poly &c, poly &a, poly &b, std::int32_t p){
    int i;
	for(i=0;i<N;i++){
		c.coeffs[i]=(a.coeffs[i]+b.coeffs[i])%p;
	}
}
void poly_operation::poly_sub_mod(poly &c, poly &a, poly &b, std::int32_t p){
    int i;
	for(i=0;i<N;i++){
		c.coeffs[i]=(a.coeffs[i]-b.coeffs[i])%p;
	}
}
void poly_operation::poly_multiply(poly &c, std::int32_t a, poly &b, std::int32_t p){
    int i;
	for(i=0;i<N;i++){
        c.coeffs[i]=multiply(a,b.coeffs[i],p);
	}
}

void poly_operation::serialize_poly(const poly &p, std::vector<std::uint8_t> &buf,int index){
	if(buf.size()<POLY_REGULAR_SIZE){
		buf.resize(POLY_REGULAR_SIZE);
	}
	std::uint8_t *tmp=(uint8_t*)(p.coeffs);
	std::copy(((std::uint8_t*)(p.coeffs)),((std::uint8_t*)(p.coeffs))+POLY_REGULAR_SIZE,buf.begin()+index);
}
void poly_operation::deserialize_poly(poly &p, const std::vector<std::uint8_t> &buf,int index){
	std::uint8_t *tmp=(std::uint8_t*)(p.coeffs);
	std::copy(buf.begin()+index,buf.begin()+index+POLY_REGULAR_SIZE,((std::uint8_t*)(p.coeffs)));
}
void poly_operation::serialize_poly_modQ(const poly &p, std::vector<std::uint8_t> &buf,int index){
	if(buf.size()<POLY_MODQ_SIZE){
		buf.resize(POLY_MODQ_SIZE);
	}
	for(int i=0;i<N;i++){
		buf[i*3+0+index]=p.coeffs[i];
		buf[i*3+1+index]=p.coeffs[i]>>8;
		buf[i*3+2+index]=p.coeffs[i]>>16;
		//printf("pack[%x %x %x]=coeffs[%d] %x\n",pack[i*3+0],pack[i*3+1],pack[i*3+2],i,p->coeffs[i]);
	}
}
void poly_operation::deserialize_poly_modQ(poly &p, const std::vector<std::uint8_t> &buf,int index){

	for(int i=0;i<N;i++){
		p.coeffs[i]=buf[i*3+0+index];
		p.coeffs[i]|=((std::int32_t)buf[i*3+1+index])<<8;
		p.coeffs[i]|=(((std::int32_t)buf[i*3+2+index])<<24)>>8;
		//printf("unpack[%x %x %x]=coeffs[%d] %x\n",pack[i*3+0],pack[i*3+1],pack[i*3+2],i,p->coeffs[i]);
	}
}

void poly_operation::serialize_poly_modQ(const poly &p, std::uint8_t *byte_array,int index){
	for(int i=0;i<N;i++){
		byte_array[i*3+0+index]=p.coeffs[i];
		byte_array[i*3+1+index]=p.coeffs[i]>>8;
		byte_array[i*3+2+index]=p.coeffs[i]>>16;
		//printf("pack[%x %x %x]=coeffs[%d] %x\n",pack[i*3+0],pack[i*3+1],pack[i*3+2],i,p->coeffs[i]);
	}
}
void poly_operation::deserialize_poly_modQ(poly &p, const std::uint8_t *byte_array,int index){

	for(int i=0;i<N;i++){
		p.coeffs[i]=byte_array[i*3+0+index];
		p.coeffs[i]|=((std::int32_t)byte_array[i*3+1+index])<<8;
		p.coeffs[i]|=(((std::int32_t)byte_array[i*3+2+index])<<24)>>8;
		//printf("unpack[%x %x %x]=coeffs[%d] %x\n",pack[i*3+0],pack[i*3+1],pack[i*3+2],i,p->coeffs[i]);
	}
}

void polyvec_operation::polyvecl_initialize(polyvecl &p){
    int i;
	for(i=0;i<L;i++){
		this->po.poly_initialize(p.vec[i]);
	}
}
void polyvec_operation::polyveck_initialize(polyveck &p){
    int i;
	for(i=0;i<K;i++){
		this->po.poly_initialize(p.vec[i]);
	}
}
void polyvec_operation::copy_polyvecl(polyvecl &dest, const polyvecl &src){
    int i,j;
    for(i=0;i<L;i++){
        for(j=0;j<N;j++){
        dest.vec[i].coeffs[j]=src.vec[i].coeffs[j];
        }
    }
}
void polyvec_operation::copy_polyveck(polyveck &dest, const polyveck &src){
    int i,j;
    for(i=0;i<K;i++){
        for(j=0;j<N;j++){
        dest.vec[i].coeffs[j]=src.vec[i].coeffs[j];
        }
    }
}
void polyvec_operation::polyvecl_pointwise_poly(polyvecl &c, poly &a, polyvecl &b, std::int32_t p){
    int i;
	for(i=0;i<L;i++){
		this->po.poly_pointwise(c.vec[i],a,b.vec[i],p);
	}
}
void polyvec_operation::polyveck_pointwise_poly(polyveck &c, poly &a, polyveck &b, std::int32_t p){
    int i;
	for(i=0;i<K;i++){
		this->po.poly_pointwise(c.vec[i],a,b.vec[i],p);
	}
}
void polyvec_operation::polyvec_pointwise(polyveck &pk, polyvecl mat[K], polyvecl &pl,std::int32_t p){
	for(int i=0;i<N;i++){
		for(int j=0;j<K;j++){
			pk.vec[j].coeffs[i]=(
				multiply(mat[j].vec[0].coeffs[i],pl.vec[0].coeffs[i],p)+
				multiply(mat[j].vec[1].coeffs[i],pl.vec[1].coeffs[i],p)+
				multiply(mat[j].vec[2].coeffs[i],pl.vec[2].coeffs[i],p)+
				multiply(mat[j].vec[3].coeffs[i],pl.vec[3].coeffs[i],p)
			)%Q;
		}
	}
}

void polyvec_operation::polyveck_pointwise(polyveck &c, polyveck &a, polyveck &b,std::int32_t p){
	for(int i=0;i<K;i++){
		this->po.poly_pointwise(c.vec[i],a.vec[i],b.vec[i],p);
	}
}
void polyvec_operation::polyvecl_pointwise(polyvecl &c, polyvecl &a, polyvecl &b,std::int32_t p){
	for(int i=0;i<L;i++){
		this->po.poly_pointwise(c.vec[i],a.vec[i],b.vec[i],p);
	}
}
void polyvec_operation::polyvecl_add_mod(polyvecl &c, polyvecl &a, polyvecl &b, std::int32_t p){
    int i;
	for(i=0;i<L;i++){
		this->po.poly_add_mod(c.vec[i],a.vec[i],b.vec[i],p);
	}
}
void polyvec_operation::polyveck_add_mod(polyveck &c, polyveck &a, polyveck &b, std::int32_t p){
    int i;
	for(i=0;i<K;i++){
		this->po.poly_add_mod(c.vec[i],a.vec[i],b.vec[i],p);
	}
}
void polyvec_operation::polyvecl_sub_mod(polyvecl &c, polyvecl &a, polyvecl &b, std::int32_t p){
    int i;
	for(i=0;i<L;i++){
		this->po.poly_sub_mod(c.vec[i],a.vec[i],b.vec[i],p);
	}
}
void polyvec_operation::polyveck_sub_mod(polyveck &c, polyveck &a, polyveck &b, std::int32_t p){
    int i;
	for(i=0;i<K;i++){
		this->po.poly_sub_mod(c.vec[i],a.vec[i],b.vec[i],p);
	}
}

void polyvec_operation::polyvecl_multiply(polyvecl &c, std::int32_t a, polyveck &b, std::int32_t p){
	for(int i=0;i<L;i++){
		this->po.poly_multiply(c.vec[i],a,b.vec[i],p);
	}
}
void polyvec_operation::polyveck_multiply(polyveck &c, std::int32_t a, polyveck &b, std::int32_t p){
	for(int i=0;i<K;i++){
		this->po.poly_multiply(c.vec[i],a,b.vec[i],p);
	}
}
bool polyvec_operation::compare_polyvecl(polyvecl &a, polyvecl &b){
    for(int i=0;i<L;i++){
        for(int j=0;j<N;j++){
            if(a.vec[i].coeffs[j]!=b.vec[i].coeffs[j]){
                return false;
            }
        }
    }
	return true;
}
bool polyvec_operation::compare_polyveck(polyveck &a, polyveck &b){
    for(int i=0;i<K;i++){
        for(int j=0;j<N;j++){
            if(a.vec[i].coeffs[j]!=b.vec[i].coeffs[j]){
                return false;
            }
        }
    }
	return true;
}
void polyvec_operation::serialize_polyvecl(const polyvecl &p, std::vector<std::uint8_t> &buf,int index){
	if(buf.size()<POLYVECL_REGULAR_SIZE){
		buf.resize(POLYVECL_REGULAR_SIZE);
	}
	for(int i=0;i<L;i++){
		this->po.serialize_poly(p.vec[i],buf,index+(i*POLY_REGULAR_SIZE));
	}
}
void polyvec_operation::deserialize_polyvecl(polyvecl &p, const std::vector<std::uint8_t> &buf,int index){
	for(int i=0;i<L;i++){
		this->po.deserialize_poly(p.vec[i],buf,index+(i*POLY_REGULAR_SIZE));
	}
}
void polyvec_operation::serialize_polyveck(const polyveck &p, std::vector<std::uint8_t> &buf,int index){
	if(buf.size()<POLYVECK_REGULAR_SIZE){
		buf.resize(POLYVECK_REGULAR_SIZE);
	}
	for(int i=0;i<K;i++){
		this->po.serialize_poly(p.vec[i],buf,index+(i*POLY_REGULAR_SIZE));
	}
}
void polyvec_operation::deserialize_polyveck(polyveck &p, const std::vector<std::uint8_t> &buf,int index){
	for(int i=0;i<K;i++){
		this->po.deserialize_poly(p.vec[i],buf,index+(i*POLY_REGULAR_SIZE));
	}
}


void polyvec_operation::serialize_polyvecl_modQ(const polyvecl &p, std::vector<std::uint8_t> &buf,int index){
	if(buf.size()<POLYVECL_MODQ_SIZE){
		buf.resize(POLYVECL_MODQ_SIZE);
	}
	for(int i=0;i<L;i++){
		this->po.serialize_poly_modQ(p.vec[i],buf,index+(i*POLY_MODQ_SIZE));
	}
}
void polyvec_operation::deserialize_polyvecl_modQ(polyvecl &p, const std::vector<std::uint8_t> &buf,int index){
	for(int i=0;i<L;i++){
		this->po.deserialize_poly_modQ(p.vec[i],buf,index+(i*POLY_MODQ_SIZE));
	}
}
void polyvec_operation::serialize_polyveck_modQ(const polyveck &p, std::vector<std::uint8_t> &buf,int index){
	if(buf.size()<POLYVECK_MODQ_SIZE){
		buf.resize(POLYVECK_MODQ_SIZE);
	}
	for(int i=0;i<K;i++){
		this->po.serialize_poly_modQ(p.vec[i],buf,index+(i*POLY_MODQ_SIZE));
	}
}
void polyvec_operation::deserialize_polyveck_modQ(polyveck &p, const std::vector<std::uint8_t> &buf,int index){
	for(int i=0;i<K;i++){
		this->po.deserialize_poly_modQ(p.vec[i],buf,index+(i*POLY_MODQ_SIZE));
	}
}


void polyvec_operation::serialize_polyvecl_modQ(const polyvecl &p, std::uint8_t *byte_array,int index){
	
	for(int i=0;i<L;i++){
		this->po.serialize_poly_modQ(p.vec[i],byte_array,index+(i*POLY_MODQ_SIZE));
	}
}
void polyvec_operation::deserialize_polyvecl_modQ(polyvecl &p, const std::uint8_t *byte_array,int index){
	for(int i=0;i<L;i++){
		this->po.deserialize_poly_modQ(p.vec[i],byte_array,index+(i*POLY_MODQ_SIZE));
	}
}
void polyvec_operation::serialize_polyveck_modQ(const polyveck &p, std::uint8_t *byte_array,int index){
	
	for(int i=0;i<K;i++){
		this->po.serialize_poly_modQ(p.vec[i],byte_array,index+(i*POLY_MODQ_SIZE));
	}
}
void polyvec_operation::deserialize_polyveck_modQ(polyveck &p, const std::uint8_t *byte_array,int index){
	for(int i=0;i<K;i++){
		this->po.deserialize_poly_modQ(p.vec[i],byte_array,index+(i*POLY_MODQ_SIZE));
	}
}