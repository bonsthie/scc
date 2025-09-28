add_executable(FileManagerTests
	FMUtils.cpp
	FileTest.cpp
	FileFinderTest.cpp
	FileManagerTest.cpp
)

scc_create_test(
  TARGET file-manager
  EXECUTABLE FileManagerTests
  LIBS PRIVATE FileManager
)
