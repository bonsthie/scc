file(GLOB OPTION_READER_SOURCES
	Token.cpp
)

add_library(Token STATIC)

target_sources(Token
  PRIVATE
	${OPTION_READER_SOURCES}
)

target_include_directories(Token
  PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_compile_features(Token PUBLIC cxx_std_23)
