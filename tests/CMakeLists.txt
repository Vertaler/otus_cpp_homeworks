add_executable(tests test_version.cpp)
target_link_libraries(tests
    gtest_main
    gtest
    helloworld
)

target_include_directories(tests 
    PRIVATE ${CMAKE_SOURCE_DIR}
)

enable_testing()
include(GoogleTest)
gtest_discover_tests(tests)