# i need to make better setup for this with build dependency and auto sub test but right now
# i have the crampte of doing cmake (deep inside me i think i prefer Makefile (i now that a
# skill issue because i don't now cmake)) might want to learn to do it in bazel insted

include(GoogleTest)

add_executable(sccTest 
	FileManager/FMUtils.cpp
	FileManager/FileTest.cpp
	FileManager/FileFinderTest.cpp
	FileManager/FileManagerTest.cpp

)

target_link_libraries(sccTest PRIVATE FileManager GTest::gtest_main)
gtest_discover_tests(sccTest)
