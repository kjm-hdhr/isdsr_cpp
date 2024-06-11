#ifndef POLYVEC_HPP
#define POLYVEC_HPP

#include <cstdint>
#include <vector>
#ifdef __cplusplus
extern "C"{
#endif

#include <poly.h>
#include <polyvec.h>

#ifdef __cplusplus
}
#endif
#define POLY_ELEMENT_REGULAR_SIZE sizeof(int32_t)
#define POLY_ELEMENT_MODQ_SIZE 3
#define POLY_REGULAR_SIZE POLY_ELEMENT_REGULAR_SIZE * N 
#define POLY_MODQ_SIZE POLY_ELEMENT_MODQ_SIZE * N //Q is lower than 0x01000000
#define POLYVECK_REGULAR_SIZE POLY_REGULAR_SIZE * K
#define POLYVECL_REGULAR_SIZE POLY_REGULAR_SIZE * L
#define POLYVECK_MODQ_SIZE POLY_MODQ_SIZE * K
#define POLYVECL_MODQ_SIZE POLY_MODQ_SIZE * L

namespace oit::ist::nws::adhoc_routing{

class poly_operation{
    public:
    void poly_initialize(poly &p);
    void poly_pointwise(poly &c, poly &a, poly &b, std::int32_t p);
    void poly_add_mod(poly &c, poly &a, poly &b, std::int32_t p);
    void poly_sub_mod(poly &c, poly &a, poly &b, std::int32_t p);
    void poly_multiply(poly &c, std::int32_t a, poly &b, std::int32_t p);
    void serialize_poly(const poly &p, std::vector<std::uint8_t> &buf,int index);
    void deserialize_poly(poly &p, const std::vector<std::uint8_t> &buf,int index);
    void serialize_poly_modQ(const poly &p, std::vector<std::uint8_t> &buf,int index);
    void deserialize_poly_modQ(poly &p, const std::vector<std::uint8_t> &buf,int index);
};
class polyvec_operation{

    protected:
    poly_operation po;
    public:
    void polyvecl_initialize(polyvecl &p);
    void polyveck_initialize(polyveck &p);
    void copy_polyvecl(polyvecl &dest, const polyvecl &src);
    void copy_polyveck(polyveck &dest, const polyveck &src);
    void polyvecl_pointwise_poly(polyvecl &c, poly &a, polyvecl &b, std::int32_t p);
    void polyveck_pointwise_poly(polyveck &c, poly &a, polyveck &b, std::int32_t p);
    void polyvec_pointwise(polyveck &pk, polyvecl mat[K], polyvecl &pl, std::int32_t p);
    void polyvecl_add_mod(polyvecl &c, polyvecl &a, polyvecl &b, std::int32_t p);
    void polyveck_pointwise(polyveck &c, polyveck &a, polyveck &b,std::int32_t p);
    void polyvecl_pointwise(polyvecl &c, polyvecl &a, polyvecl &b,std::int32_t p);
    void polyveck_add_mod(polyveck &c, polyveck &a, polyveck &b, std::int32_t p);
    void polyvecl_sub_mod(polyvecl &c, polyvecl &a, polyvecl &b, std::int32_t p);
    void polyveck_sub_mod(polyveck &c, polyveck &a, polyveck &b, std::int32_t p);
    void polyvecl_multiply(polyvecl &c, std::int32_t a, polyveck &b, std::int32_t p);
    void polyveck_multiply(polyveck &c, std::int32_t a, polyveck &b, std::int32_t p);
    bool compare_polyvecl(polyvecl &a, polyvecl &b);
    bool compare_polyveck(polyveck &a, polyveck &b);
    void serialize_polyvecl(const polyvecl &p, std::vector<std::uint8_t> &buf,int index);
    void deserialize_polyvecl(polyvecl &p, const std::vector<std::uint8_t> &buf,int index);
    void serialize_polyveck(const polyveck &p, std::vector<std::uint8_t> &buf,int index);
    void deserialize_polyveck(polyveck &p, const std::vector<std::uint8_t> &buf,int index);

    void serialize_polyvecl_modQ(const polyvecl &p, std::vector<std::uint8_t> &buf,int index);
    void deserialize_polyvecl_modQ(polyvecl &p, const std::vector<std::uint8_t> &buf,int index);
    void serialize_polyveck_modQ(const polyveck &p, std::vector<std::uint8_t> &buf,int index);
    void deserialize_polyveck_modQ(polyveck &p, const std::vector<std::uint8_t> &buf,int index);
};
};
#endif