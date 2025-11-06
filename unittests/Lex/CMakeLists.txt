add_executable(LexTests
	FileLexerTests.cpp
)

scc_create_test(
  TARGET lex
  EXECUTABLE LexTests
  LIBS PRIVATE Lex
)
