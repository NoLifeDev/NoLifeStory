project(NoLifeClient)
cmake_minimum_required(VERSION 2.8)

# Enable C++11 support.
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  add_definitions("-std=c++11 -pedantic -Wno-switch")
endif()

# Access additional CMake modules.
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake/Modules" ${CMAKE_MODULE_PATH})

include_directories(..)

# Boost
find_package(Boost COMPONENTS system filesystem regex REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

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

#aux_source_directory(. NOLIFECLIENT_SOURCES)

add_executable(NoLifeClient 
#ClassicUI.cpp
#background.cpp
config.cpp
#foothold.cpp
game.cpp
#layer.cpp
main.cpp
map.cpp
#obj.cpp
#physics.cpp
#player.cpp
#portal.cpp
#sound.cpp
sprite.cpp
#tile.cpp
time.cpp
view.cpp
window.cpp
)

target_link_libraries(NoLifeClient NoLifeNx ${Boost_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} mpg123 glfw)
