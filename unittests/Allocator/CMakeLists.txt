add_executable(AllocatorTests
    BumpAllocatorTest.cpp
    StackGrowAllocatorTest.cpp
)

scc_create_test(
  TARGET allocator
  EXECUTABLE AllocatorTests
  LIBS PRIVATE Allocator
)
