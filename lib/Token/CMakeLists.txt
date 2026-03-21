set(TOKEN_SOURCES
	Token.cpp
	${CMAKE_SOURCE_DIR}/lib/Lex/DecodeChar.cpp
)

add_library(Token STATIC)

target_sources(Token
  PRIVATE
	${TOKEN_SOURCES}
)

target_include_directories(Token
  PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(Token
  PUBLIC
    FileManager
)

target_compile_features(Token PUBLIC cxx_std_23)
