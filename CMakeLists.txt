project(NoLifeClient)
cmake_minimum_required(VERSION 2.8)

# Enable C++11 support.
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  add_definitions("-std=c++11 -pedantic -Wno-switch")
endif()

# Access additional CMake modules.
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake/Modules" ${CMAKE_MODULE_PATH})

# SFML
find_package(SFML 2 COMPONENTS system window audio REQUIRED)
include_directories(${SFML_INCLUDE_DIR})

# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

# GLEW
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

# Find out if we are on X11, for X11 specific hacks
find_package(X11)

if(X11_FOUND)
    add_definitions(-DNL_X11)
endif()

aux_source_directory(. NOLIFECLIENT_SOURCES)

add_executable(NoLifeClient ${NOLIFECLIENT_SOURCES})

target_link_libraries(NoLifeClient NoLifeNx ${SFML_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} mpg123)
