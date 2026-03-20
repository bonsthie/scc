set(LEX_SOURCES
	FileLexer.cpp
)

add_library(Lex STATIC)

target_sources(Lex
  PRIVATE
	${LEX_SOURCES}
)

target_include_directories(Lex
  PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(Lex
  PUBLIC
    Token
    FileManager
)

target_compile_features(Lex PUBLIC cxx_std_23)
