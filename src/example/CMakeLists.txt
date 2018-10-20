include_directories(
  ../../include
)

add_executable(yate-example
  example_main.cc
)

target_link_libraries(yate-example yate)
add_dependencies(yate-example yate)
target_compile_features(yate PRIVATE ${REQUIRED_CXX_FEATURES})
