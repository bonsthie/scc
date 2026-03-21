set(SEMA_SOURCES
    Scope.cpp
    Sema.cpp
)

add_library(Sema STATIC)

target_sources(Sema
  PRIVATE
    ${SEMA_SOURCES}
)

target_include_directories(Sema
  PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(Sema
  PUBLIC
    AST
)

target_compile_features(Sema PUBLIC cxx_std_23)
