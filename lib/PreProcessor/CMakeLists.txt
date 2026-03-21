file(GLOB OPTION_READER_SOURCES
	PreProcessor.cpp
)

add_library(PreProcessor STATIC)

target_sources(PreProcessor
  PRIVATE
	${OPTION_READER_SOURCES}
)

target_include_directories(PreProcessor
  PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(PreProcessor
  PUBLIC
	Error
	Token
	FileManager
	Lex
	Allocator
)

target_compile_features(PreProcessor PUBLIC cxx_std_23)
