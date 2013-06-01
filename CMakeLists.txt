project(NoLifeWzToNx)

enable_cpp11()

aux_source_directory(. NOLIFEWZTONX_SOURCES)

add_executable(NoLifeWzToNx ${NOLIFEWZTONX_SOURCES))
