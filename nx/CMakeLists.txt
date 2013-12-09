aux_source_directory(. NOLIFENX_SOURCES)
add_library(NoLifeNx ${NOLIFENX_SOURCES})
target_link_libraries(NoLifeNx lz4)
