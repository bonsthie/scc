add_executable(ErrorTests
    ErrorManagerTest.cpp
)

scc_create_test(
  TARGET error
  EXECUTABLE ErrorTests
  LIBS PRIVATE Error
)
