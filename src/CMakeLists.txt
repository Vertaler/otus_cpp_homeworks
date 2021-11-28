add_executable(bulk_server 
    main.cpp
    Server.h
    bulking/BulkAggregator.h
    bulking/CommandBulk.h
    bulking/InputProcessor.h
)
target_include_directories(bulk_server PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(bulk_server PRIVATE Boost::boost Boost::date_time Boost::regex)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_libraries(bulk_server PRIVATE pthread)
endif()

_setup_targets(bulk_server)

install(TARGETS bulk_server RUNTIME DESTINATION bin)
