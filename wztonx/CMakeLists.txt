project(NoLifeWzToNx CXX)
cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
include(../cmake/common.cmake)
enable_cxx11(CXX11_FLAGS)

aux_source_directory(. NOLIFEWZTONX_SOURCES)
add_executable(NoLifeWzToNx ${NOLIFEWZTONX_SOURCES})
target_link_libraries(NoLifeWzToNx c++abi z lz4 ${CXX11_FLAGS})

install(TARGETS NoLifeWzToNx DESTINATION bin)
