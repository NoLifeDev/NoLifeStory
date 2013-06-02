cmake_minimum_required(VERSION 2.8)

# Enable C++11 support for compilers that need explicit enabling
function(enable_cpp11)
    if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        add_definitions("-std=c++11 -pedantic -Wno-switch")
    endif ()
endfunction()

project(NoLifeStory)

add_subdirectory(NoLifeClient)
add_subdirectory(NoLifeNx)
# NoLifeWzToNx is not ready just yet
#add_subdirectory(NoLifeWzToNx)