project(NoLifeStory)
cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

option(WARN_MORE "Enable additional warnings" ON)

# Option to build with libc++ if using Clang on Linux
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" AND "${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    option(USE_LIBCXX "Use libc++ instead of the default C++ standard library" OFF)
    if(USE_LIBCXX)
        add_compile_options(-stdlib=libc++)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
    endif()
endif()

# Set some things up for GCC/Clang
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    add_compile_options(-std=c++11)
    if(WARN_MORE)
        add_compile_options(-pedantic -Wall)
        if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
            # Don't warn about anonymous types in union (non-standard extension)
            add_compile_options(-Wno-nested-anon-types)
        endif()
    endif()
    if(NOT USE_LIBCXX)
        add_definitions(-DNL_NO_CODECVT)
    endif()
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
