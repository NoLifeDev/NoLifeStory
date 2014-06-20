project(NoLifeWzToNx CXX)
cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

find_package(Boost COMPONENTS filesystem system REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

aux_source_directory(. NOLIFEWZTONX_SOURCES)
add_executable(NoLifeWzToNx ${NOLIFEWZTONX_SOURCES})
target_link_libraries(NoLifeWzToNx z lz4 ${Boost_LIBRARIES})

install(TARGETS NoLifeWzToNx DESTINATION bin)
