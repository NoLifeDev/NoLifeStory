project(NoLifeClient)

enable_cpp11()

add_executable(NoLifeClient
Game.cpp
Game.hpp
Graphics.cpp
Graphics.hpp
Layer.cpp
Layer.hpp
Log.cpp
Log.hpp
Map.cpp
Map.hpp
NoLifeClient.cpp
NoLifeClient.hpp
Obj.cpp
Obj.hpp
Sound.cpp
Sound.hpp
Tile.cpp
Tile.hpp
Time.cpp
Time.hpp
Sprite.cpp
Sprite.hpp
)

target_link_libraries(NoLifeClient NoLifeNx boost_filesystem boost_system sfml-window sfml-system GL bass GLEW)
