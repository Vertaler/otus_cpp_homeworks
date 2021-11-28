add_executable(join_server 
    main.cpp
    Server.h
    Storage.h
)
target_include_directories(join_server PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(join_server PRIVATE Boost::boost Boost::date_time Boost::regex)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_libraries(join_server PRIVATE pthread)
endif()

_setup_targets(join_server)

install(TARGETS join_server RUNTIME DESTINATION bin)
