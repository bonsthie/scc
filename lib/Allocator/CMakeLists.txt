file(GLOB OPTION_READER_SOURCES
	BumpAllocator.cpp
)

add_library(Allocator STATIC)

target_sources(Allocator
  PRIVATE
	${OPTION_READER_SOURCES}
)

target_include_directories(Allocator
  PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(Allocator
  PUBLIC
)

target_compile_features(Allocator PUBLIC cxx_std_23)
