add_executable(helloworld_cli main.cpp)
add_library(helloworld lib.cpp)

configure_file(version.h.in version.h)

target_link_libraries(helloworld_cli PRIVATE
    helloworld
)

target_include_directories(helloworld
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}"
)