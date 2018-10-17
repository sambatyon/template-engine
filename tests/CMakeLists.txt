include_directories(
  ../include
)

add_executable(${PROJECT_PREFIX}-tests
  test_main.cc
)

target_link_libraries(${PROJECT_PREFIX}-tests yate)

target_compile_features(${PROJECT_PREFIX}-tests PRIVATE ${REQUIRED_CXX_FEATURES})

add_dependencies(${PROJECT_PREFIX}-tests yate)
add_test(${PROJECT_PREFIX}tests ${BIN_OUTPUT_DIR}/${PROJECT_PREFIX}-tests)
