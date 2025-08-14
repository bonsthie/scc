file(GLOB BYTECODE_READER_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_library(FileManager STATIC)

target_sources(FileManager
  PRIVATE
    ${BYTECODE_READER_SOURCES}
)

target_include_directories(FileManager
  PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_compile_features(FileManager PUBLIC cxx_std_23)
