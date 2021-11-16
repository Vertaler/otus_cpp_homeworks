add_executable(tests test_version.cpp test_ip_filter.cpp)
target_link_libraries(tests
    gtest_main
    gtest
    gmock
    helloworld
    ip_filter_lib
)

target_include_directories(tests 
    PRIVATE ${CMAKE_SOURCE_DIR}
)

enable_testing()
include(GoogleTest)
gtest_discover_tests(tests)