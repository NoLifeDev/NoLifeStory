include_directories(..)

# Find packages
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

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

target_link_libraries(NoLifeClient NoLifeNx ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} mpg123 glfw)
