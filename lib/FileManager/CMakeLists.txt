file(GLOB FILEMANAGER_READER_SOURCES
	FileFinder.cpp
	MemoryBuffer.cpp
)

add_library(FileManager STATIC)

target_sources(FileManager
  PRIVATE
    ${FILEMANAGER_READER_SOURCES}
)

target_include_directories(FileManager
  PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_compile_features(FileManager PUBLIC cxx_std_23)
