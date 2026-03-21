add_executable(PreProcessorTests
    PreProcessorTest.cpp
)

scc_create_test(
  TARGET preprocessor
  EXECUTABLE PreProcessorTests
  LIBS PRIVATE PreProcessor
)
