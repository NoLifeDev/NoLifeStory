project(NoLifeClient)

enable_cpp11()

aux_source_directory(. NOLIFECLIENT_SOURCES)

add_executable(NoLifeClient ${NOLIFECLIENT_SOURCES})

target_link_libraries(NoLifeClient NoLifeNx boost_filesystem boost_system sfml-window sfml-system GL bass GLEW)
