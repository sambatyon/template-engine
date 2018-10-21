file(GLOB_RECURSE yate_example_hdr *.hh)
file(GLOB_RECURSE yate_example_src *.cc)

source_group("Header Files" FILES ${yate_example_hdr})
source_group("Source Files" FILES ${yate_example_src})

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
  ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

add_executable(${PROJECT_PREFIX}-tests
  ${yate_example_hdr}
  ${yate_example_src}
)

target_link_libraries(${PROJECT_PREFIX}-tests yate)

target_compile_features(${PROJECT_PREFIX}-tests PRIVATE ${REQUIRED_CXX_FEATURES})

add_dependencies(${PROJECT_PREFIX}-tests yate)
add_test(${PROJECT_PREFIX}-tests ${BIN_OUTPUT_DIR}/${PROJECT_PREFIX}-tests)
