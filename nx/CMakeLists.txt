PROJECT(NoLifeNx CXX)
cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
include(../cmake/common.cmake)
enable_cxx11(CXX11_FLAGS)

aux_source_directory(. NOLIFENX_SOURCES)

setup_option_shared_lib(OFF)
if(BUILD_SHARED_LIBS)
  add_library(NoLifeNx_shared ${NOLIFENX_SOURCES})
  target_link_libraries(NoLifeNx_shared lz4)
  set_target_properties(NoLifeNx_shared PROPERTIES
    OUTPUT_NAME NoLifeNx)
  install(TARGETS NoLifeNx_shared LIBRARY DESTINATION lib)
endif()

add_library(NoLifeNx STATIC ${NOLIFENX_SOURCES})
target_link_libraries(NoLifeNx lz4 ${CXX11_FLAGS})
