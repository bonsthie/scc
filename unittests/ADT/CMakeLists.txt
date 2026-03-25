add_executable(ADTTests
    SmallVectorTest.cpp
)

target_include_directories(ADTTests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

scc_create_test(
  TARGET ADT
  EXECUTABLE ADTTests
  LIBS PRIVATE Allocator
)
