add_executable(ASTTests
    TypeTests.cpp
)

scc_create_test(
  TARGET AST
  EXECUTABLE ASTTests
  LIBS PRIVATE AST
)
