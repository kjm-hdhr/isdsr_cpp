#ifndef POLYVEC_OP_HPP
#define POLYVEC_OP_HPP
#ifdef __cplusplus
extern "C"{
#endif
#include <params.h>
#include <poly.h>
#include <polyvec.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <randombytes.h>
#include <sign.h>
#include <packing.h>
#include <fips202.h>
#include <time.h>
#include <reduce.h>
#ifdef __cplusplus
}
#endif

#include <cstdint>
#include <cstring>

namespace oit::ist::nws::adhoc_routing{

    class polyvec_op{
        public:
		void poly_initialize(poly &p);
		void polyvecl_initialize(polyvecl &p);
		void polyveck_initialize(polyveck &p);
		void copy_polyvecl(polyvecl& dest, const polyvecl& src);
		void copy_polyveck(polyveck& dest, const polyveck& src);
		void poly_pointwise(poly &c, poly &a, poly &b, int p);
		void polyvecl_pointwise_poly(polyvecl &c, poly &a, polyvecl &b, int p);
		void polyveck_pointwise_poly(polyveck &c, poly &a, polyveck &b, int p);
		void polyvec_pointwise(polyveck &pk, polyvecl mat[K], polyvecl &pl);
		void poly_add_mod(poly &c, poly &a, poly &b, int p);
		void polyvecl_add_mod(polyvecl& c, polyvecl& a, polyvecl& b, int p);
		void polyveck_add_mod(polyveck& c, polyveck& a, polyveck& b, int p);
		void poly_sub_mod(poly &c, poly &a, poly &b, int p);
		void polyvecl_sub_mod(polyvecl& c, polyvecl& a, polyvecl& b, int p);
		void polyveck_sub_mod(polyveck& c, polyveck& a, polyveck& b, int p);
		void poly_multiply(poly &c, int32_t a, poly &b, int p);
		int compare_polyvecl(polyvecl &a, polyvecl &b);
		int compare_polyveck(polyveck &a, polyveck &b);
};
};
#endif