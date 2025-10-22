set_tests_properties(${exe} PROPERTIES
  ENVIRONMENT "GTEST_COLOR=1;GTEST_BRIEF=1"
)


function(scc_create_test)
  set(options)
  set(oneValueArgs TARGET EXECUTABLE LABEL)
  set(multiValueArgs LIBS)
  cmake_parse_arguments(SCC "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGV})

  if(NOT SCC_TARGET)
    message(FATAL_ERROR "scc_create_test: missing TARGET")
  endif()

  set(exe "${SCC_EXECUTABLE}")
  if(NOT exe)
    set(exe "${SCC_TARGET}Tests")
  endif()
  if(NOT TARGET "${exe}")
    message(FATAL_ERROR "scc_create_test: test executable '${exe}' not found")
  endif()

  set(label "${SCC_LABEL}")
  if(NOT label)
    set(label "${SCC_TARGET}")
  endif()

  if(SCC_LIBS)
    target_link_libraries("${exe}" PRIVATE ${SCC_LIBS})
  endif()
  target_link_libraries("${exe}" PRIVATE GTest::gtest_main)

  include(GoogleTest)
  gtest_discover_tests("${exe}"
    TEST_PREFIX "${label}/"
    DISCOVERY_MODE PRE_TEST
    PROPERTIES LABELS "${label}"
  )

  add_custom_target("check-${label}"
    COMMAND ${CMAKE_CTEST_COMMAND}
            --test-dir ${CMAKE_BINARY_DIR}
            -L ${label}
            --output-on-failure
			${verbose_flag}
    DEPENDS "${exe}"
    USES_TERMINAL
  )
endfunction()



add_custom_target(check
  COMMAND ${CMAKE_CTEST_COMMAND} 
		--output-on-failure ${CMAKE_BUILD_PARALLEL_LEVEL}
		${verbose_flag}
  DEPENDS
	LexTests
	FileManagerTests
	OptionTests
  USES_TERMINAL
)

add_subdirectory(FileManager)
add_subdirectory(Option)
add_subdirectory(Lex)
