add_library(ip_filter_lib INTERFACE)
target_sources(ip_filter_lib 
    INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}/ip_filter.h 
    ${CMAKE_CURRENT_SOURCE_DIR}/utils.h
)
add_executable(ip_filter main.cpp)
target_link_libraries(ip_filter
    PRIVATE ip_filter_lib
)