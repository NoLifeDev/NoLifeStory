project(NoLifeStory)
cmake_minimum_required(VERSION 2.8.12)

# Detect compiler
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(USING_GCC 1)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(USING_CLANG 1)
else()
    message(FATAL_ERROR "Your compiler is not supported by NoLifeStory")
endif()

option(BUILD_WZTONX "Build WzToNx (Experimental, only Clang with libstdc++ supported)")
if(USING_CLANG)
    option(USE_LIBCPP "Use Clang's libc++ instead of libstdc++")
endif()

# For GCC and Clang, enable C++11 support and add some other flags
if(USING_GCC OR USING_CLANG)
  add_compile_options(-std=c++11 -pedantic -Wno-switch)
endif()

if(USING_CLANG)
    add_compile_options(-Wno-nested-anon-types)
    if(USE_LIBCPP)
        add_compile_options(-stdlib=libc++)
    endif()
endif()

add_subdirectory(nx)
add_subdirectory(client)
if(BUILD_WZTONX)
    if(NOT USING_CLANG)
        message(FATAL_ERROR "At the moment, Only Clang with libstdc++ is supported for building WzToNx")
    endif()
    add_subdirectory(wztonx)
endif()
