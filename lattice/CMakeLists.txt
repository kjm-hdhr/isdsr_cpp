cmake_minimum_required(VERSION 3.13)
#lattice dilithium library
#set(dilithium_src_dir ../../dilithium/ref)
include_directories(${dilithium_src_dir})
set(fips202_src
${dilithium_src_dir}/fips202.c
)
set(fips202_header
${dilithium_src_dir}/fips202.h
)

set(dilithium_src
${dilithium_src_dir}/sign.c
${dilithium_src_dir}/packing.c
${dilithium_src_dir}/polyvec.c
${dilithium_src_dir}/poly.c
${dilithium_src_dir}/ntt.c
${dilithium_src_dir}/reduce.c
${dilithium_src_dir}/rounding.c
${dilithium_src_dir}/randombytes.c
${dilithium_src_dir}/symmetric-shake.c
)
set(dilithium_header
${dilithium_src_dir}/config.h
${dilithium_src_dir}/params.h
${dilithium_src_dir}/api.h
${dilithium_src_dir}/sign.h
${dilithium_src_dir}/packing.h
${dilithium_src_dir}/polyvec.h
${dilithium_src_dir}/poly.h
${dilithium_src_dir}/ntt.h
${dilithium_src_dir}/reduce.h
${dilithium_src_dir}/rounding.h
${dilithium_src_dir}/symmetric.h
${dilithium_src_dir}/randombytes.h
)
set(lattice_sig_src
lattice_sig.cpp
matrix.cpp
polyvec_op.cpp
)
set(lattice_sig_header
lattice_sig.hpp
matrix.hpp
polyvec_op.hpp
)
#add_custom_command(OUTPUT dilithium_lib COMMAND cd ${dilithium_dir}; cmake -D)
add_library(fips202 STATIC ${fips202_src} ${fips202_header})
add_library(dilithium2 STATIC ${dilithium_src} ${dilithium_header})
target_compile_options(fips202 PUBLIC -fPIC -Wall -Wextra -Wpedantic -Wmissing-prototypes -Wredundant-decls -Wshadow -Wvla -Wpointer-arith -O3 -fomit-frame-pointer)
target_compile_options(dilithium2 PUBLIC -fPIC -Wall -Wextra -Wpedantic -Wmissing-prototypes -Wredundant-decls -Wshadow -Wvla -Wpointer-arith -O3 -fomit-frame-pointer)

add_library(lattice STATIC ${lattice_sig_src} ${lattice_sig_header})
