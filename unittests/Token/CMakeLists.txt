add_executable(TokenTests
    TokenTest.cpp
)

scc_create_test(
  TARGET token
  EXECUTABLE TokenTests
  LIBS PRIVATE Token
)
