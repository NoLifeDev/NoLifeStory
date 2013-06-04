project(NoLifeNx)
cmake_minimum_required(VERSION 2.8)

# Enable C++11 support for compilers that need explicit enabling
if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  add_definitions("-std=c++11 -pedantic -Wno-switch")
endif ()

aux_source_directory(. NOLIFENX_SOURCES)

add_library(NoLifeNx ${NOLIFENX_SOURCES})