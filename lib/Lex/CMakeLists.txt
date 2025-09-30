file(GLOB OPTION_READER_SOURCES
	FileLexer.cpp
)

add_library(Lex STATIC)

target_sources(Lex
  PRIVATE
	${OPTION_READER_SOURCES}
)

target_include_directories(Lex
  PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_compile_features(Lex PUBLIC cxx_std_23)
