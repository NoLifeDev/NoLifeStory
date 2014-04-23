project(NoLifeWzToNx CXX)
cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
include(../../cmake/common.cmake)

aux_source_directory(. NOLIFEWZTONX_SOURCES)
add_executable(NoLifeWzToNx ${NOLIFEWZTONX_SOURCES})
target_link_libraries(NoLifeWzToNx z lz4)

install(TARGETS NoLifeWzToNx DESTINATION bin)
