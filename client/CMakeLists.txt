include_directories(..)

# Find packages
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
find_package(glfw REQUIRED)
include_directories(${GLFW_INCLUDE_DIR})

#aux_source_directory(. NOLIFECLIENT_SOURCES)

add_executable(NoLifeClient 
#ClassicUI.cpp
background.cpp
config.cpp
foothold.cpp
game.cpp
layer.cpp
main.cpp
map.cpp
obj.cpp
#physics.cpp
#player.cpp
portal.cpp
#sound.cpp
sprite.cpp
tile.cpp
time.cpp
view.cpp
window.cpp
)

if(USE_LIBCPP)
    set_target_properties(NoLifeClient PROPERTIES LINK_FLAGS "-stdlib=libc++ -lc++ -lc++abi")
endif()
target_link_libraries(NoLifeClient NoLifeNx ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${GLFW_LIBRARY})
