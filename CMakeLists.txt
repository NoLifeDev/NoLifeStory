project(NoLifeNx)

enable_cpp11()

aux_source_directory(. NOLIFENX_SOURCES)

add_library(NoLifeNx ${NOLIFENX_SOURCES})
