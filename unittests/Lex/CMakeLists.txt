add_executable(LexTests
	FileLexerTests.cpp
)

message("crampte")
scc_create_test(
  TARGET lex
  EXECUTABLE LexTests
  LIBS PRIVATE Lex
)
