add_executable(OptionTests
	OptionTableTest.cpp
)

scc_create_test(
  TARGET option
  EXECUTABLE OptionTests
  LIBS PRIVATE Option
)
