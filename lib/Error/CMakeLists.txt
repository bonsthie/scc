file(GLOB OPTION_READER_SOURCES
	Error.cpp
)

add_library(Error STATIC)

target_sources(Error
  PRIVATE
	${OPTION_READER_SOURCES}
)

target_include_directories(Error
  PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(Error
  PUBLIC
    Token
)

target_compile_features(Error PUBLIC cxx_std_23)
