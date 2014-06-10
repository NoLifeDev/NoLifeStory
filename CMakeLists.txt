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
    add_compile_options(-std=c++1y)
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

option(BUILD_NX "Build nx library" ON)
if(BUILD_NX)
    add_subdirectory(src/nx)
endif()

option(BUILD_CLIENT "Build client" ON)
if(BUILD_CLIENT)
    add_subdirectory(src/client)
endif()

option(BUILD_WZTONX "Build wztonx" ON)
if(BUILD_WZTONX)
    add_subdirectory(src/wztonx)
endif()

option(BUILD_NXBENCH "Build nxbench" OFF)
if(BUILD_NXBENCH)
    add_subdirectory(src/nxbench)
endif()
