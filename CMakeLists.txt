cmake_minimum_required(VERSION 3.13)
set(dilithium_dir ${CMAKE_CURRENT_SOURCE_DIR}/../dilithium)
set(mcl_dir ${CMAKE_CURRENT_SOURCE_DIR}/../mcl)

set(dilithium_src_dir ${CMAKE_CURRENT_SOURCE_DIR}/../dilithium/ref)
set(mcl_src_dir ${CMAKE_CURRENT_SOURCE_DIR}/../mcl/src)
set(mcl_header_dir ${CMAKE_CURRENT_SOURCE_DIR}/../mcl/include)
set(mcl_lib_dir ${CMAKE_CURRENT_SOURCE_DIR}/../mcl)

set(CMAKE_CXX_STANDARD 17)

set(CXX /usr/bin/g++)
set(CLANGXX /usr/bin/clang++)

project(adhoc_routing C CXX ASM)

add_subdirectory(adhoc)
add_subdirectory(dsr)

add_executable(dsr_node main_exp.cpp)
target_link_libraries(dsr_node PUBLIC dsr PUBLIC adhoc)

add_subdirectory(isdsr)
add_subdirectory(ecdsa)
#include_directories(${mcl_header_dir})

#add_custom_target(ecdsa_lib SOURCES libmcl)
#add_custom_command(OUTPUT libmcl cp ${mcl_lib_dir}/libmcl.a .;cp ${mcl_lib_dir}/libmclbn384_256.a .)
add_executable(ecdsa_node main_exp.cpp)
target_include_directories(ecdsa_node PRIVATE ${mcl_header_dir})
target_compile_options(ecdsa_node PRIVATE -DECDSA -static -lssl -lcrypto)
target_link_libraries(
ecdsa_node
PRIVATE ecdsa ssl crypto
PRIVATE ${mcl_lib_dir}/libmclbn384_256.a
PRIVATE ${mcl_lib_dir}/libmcl.a
PUBLIC isdsr PUBLIC dsr PUBLIC adhoc)


add_subdirectory(lattice)
add_executable(lattice_node main_exp.cpp)
target_include_directories(lattice_node PRIVATE ${dilithium_src_dir})
target_compile_options(lattice_node PRIVATE -DLATTICE)
target_link_libraries(lattice_node PRIVATE lattice PRIVATE dilithium2 PRIVATE fips202 PUBLIC isdsr PUBLIC dsr PUBLIC adhoc)


