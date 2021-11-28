add_subdirectory(async)
add_executable(async_cli main.cpp )
target_link_libraries(async_cli PRIVATE async pthread)
_setup_targets(async_cli async)

install(TARGETS async_cli RUNTIME DESTINATION bin)
install(TARGETS async RUNTIME DESTINATION lib)