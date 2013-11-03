aux_source_directory(. NOLIFEWZTONX_SOURCES)
# At the moment, we assume Clang with libc++ is used to build WzToNx
add_compile_options(-stdlib=libc++)
add_executable(NoLifeWzToNx ${NOLIFEWZTONX_SOURCES})
set_target_properties(NoLifeWzToNx PROPERTIES LINK_FLAGS -stdlib=libc++)
target_link_libraries(NoLifeWzToNx c++abi)
