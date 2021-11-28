# Set some common for all targets properties and compile definitions
function (_setup_targets)
    message(STATUS "Setup targets ${ARGV}")

    set_target_properties(${ARGV} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
    )

    foreach (TARGET IN LISTS ${ARGV})
        if (MSVC)
            target_compile_options(TARGET PRIVATE
                /Wall /WX
            )    
        else ()
            target_compile_options(TARGET PRIVATE
                -Wall -Wextra -pedantic -Werror
            )
        endif()
    endforeach()
endfunction()