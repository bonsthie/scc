file(GLOB OPTION_READER_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_library(Option STATIC)

target_sources(Option
  PRIVATE
	${OPTION_READER_SOURCES}
)

target_include_directories(Option
  PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(Option
  PUBLIC
	Error
)

target_compile_features(Option PUBLIC cxx_std_23)
