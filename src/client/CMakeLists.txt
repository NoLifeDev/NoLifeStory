project(NoLifeClient)
cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
include_directories(..)
include(../../cmake/common.cmake)

# Find packages
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
find_package(glfw REQUIRED)
include_directories(${GLFW_INCLUDE_DIR})

FILE(GLOB NOLIFE_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

list(REMOVE_ITEM NOLIFE_SOURCES
  ClassicUI.cpp)

add_executable(NoLifeClient ${NOLIFE_SOURCES})

target_link_libraries(NoLifeClient lz4 NoLifeNx ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${GLFW_LIBRARY} portaudio mpg123 pthread)
