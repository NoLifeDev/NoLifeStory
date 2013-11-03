project(NoLifeStory)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake/Modules" ${CMAKE_MODULE_PATH})

# For GCC and Clang, enable C++11 support and add some other flags
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  add_compile_options(-std=c++11 -pedantic -Wno-switch)
endif()

# Disable Clang warnings about nested anon types
if(CMAKE_COMPILER_IS_CLANGXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  add_compile_options(-Wno-nested-anon-types)
endif()

add_subdirectory(nx)
add_subdirectory(client)
#add_subdirectory(wztonx)
