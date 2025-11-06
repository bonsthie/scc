function(scc_add_check_lit_folder)
    set(options "")
    set(one_value_args LABEL DIR)
    set(multi_value_args DEPS)

    cmake_parse_arguments(
        SCC
        "${options}"
        "${one_value_args}"
        "${multi_value_args}"
        ${ARGN}
    )

    if(NOT SCC_LABEL)
        message(FATAL_ERROR "scc_add_check_lit_folder: LABEL is required")
    endif()

    if(NOT SCC_DIR)
        set(SCC_DIR "${SCC_LABEL}")
    endif()

    set(main_target "check-${SCC_LABEL}")
    set(lit_target "${main_target}-lit")

    if(NOT TARGET ${main_target})
        add_custom_target(${main_target})
    endif()

    add_custom_target(
        ${lit_target}
        COMMAND lit -sv ${CMAKE_CURRENT_SOURCE_DIR}/${SCC_DIR}
        DEPENDS ${SCC_DEPS}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        USES_TERMINAL
        VERBATIM
    )

    add_dependencies(${main_target} ${lit_target})
endfunction()


scc_add_check_lit_folder(
    LABEL lit
    DIR   .
	DEPS  scc
)

add_dependencies(check check-lit)

scc_add_check_lit_folder(
    LABEL lex
    DIR   Lex
	DEPS  scc
)

