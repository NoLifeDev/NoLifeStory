project(NoLifeStory)
cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
include(cmake/common.cmake)

# At the moment, we assume Clang with libc++ is used to build WzToNx
detect_system(OPERATING_SYSTEM)
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
message(STATUS "${PROJECT_NAME}: Enabling Clang C++11 support")
message(STATUS "${PROJECT_NAME}: Make sure libc++ is installed")
add_compile_options(-std=c++11 -stdlib=libc++)
if(${OPERATING_SYSTEM} MATCHES "Linux")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++ -lc++abi")
endif()
else()
message(FATAL_ERROR "Only Clang and msvc are supported")
endif()

# Modular building of subprojects:
#   To disable any of the builds in the following subprojects use
#     cmake -DBUILD_PROJECTNAME=OFF .
#   This option only needs to be run once for every CMakeCache
#   By default all subprojects will be built

option(BUILD_NX "Choose to build nx library" ON)
if(BUILD_NX)
  message(STATUS "Adding nx to build")
  add_subdirectory(src/nx)
endif()

option(BUILD_CLIENT "Choose to build client" ON)
if(BUILD_CLIENT)
  message(STATUS "Adding client to build")
  add_subdirectory(src/client)
endif()

option(BUILD_WZTONX "Choose to build wztonx or not" ON)
if(BUILD_WZTONX)
  message(STATUS "Adding wztonx to build")
  add_subdirectory(src/wztonx)
endif()

option(ALL_WARNINGS "Notify compiler to be an arse" ON)
if(ALL_WARNINGS)
  message(STATUS "All warnings enabled")
  if(UNIX)
    add_compile_options(-Wall -Wpedantic)
  endif()
endif()
