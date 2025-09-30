add_executable(scc scc.cpp)

target_link_libraries(scc
	PRIVATE
	FileManager
	Option
	Lex
	Token
)

target_include_directories(scc
	PRIVATE
		${CMAKE_SOURCE_DIR}/include
)
