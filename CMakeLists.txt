cmake_minimum_required(VERSION 3.2)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})

project(otus_hw1 VERSION ${PROJECT_VESRION})

set(gtest_force_shared_crt ON)
add_subdirectory(googletest)

add_subdirectory(helloworld)
add_subdirectory(ip_filter)
add_subdirectory(tests)

include(cmake/utils.cmake)
_setup_targets(helloworld_cli helloworld tests ip_filter)

install(TARGETS helloworld_cli ip_filter RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT example@example.com)

include(CPack)