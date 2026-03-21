add_executable(StringTests
    StringHelpersTest.cpp
)

scc_create_test(
  TARGET string
  EXECUTABLE StringTests
  LIBS PRIVATE Allocator
)
