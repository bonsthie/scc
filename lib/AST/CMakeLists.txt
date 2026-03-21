set(AST_SOURCES
    Decl.cpp
    Stmt.cpp
    Type.cpp
)

add_library(AST STATIC)

target_sources(AST
  PRIVATE
    ${AST_SOURCES}
)

target_include_directories(AST
  PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_compile_features(AST PUBLIC cxx_std_23)
