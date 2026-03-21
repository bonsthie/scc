add_executable(SemaTests
    ScopeTests.cpp
)

scc_create_test(
  TARGET sema
  EXECUTABLE SemaTests
  LIBS PRIVATE Sema
)
