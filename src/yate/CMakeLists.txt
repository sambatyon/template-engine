file(GLOB_RECURSE template_hdr *.hh)
file(GLOB_RECURSE template_src *.cc)

message(STATUS "sources ${template_src}")

source_group("Header Files" FILES ${template_hdr})
source_group("Source Files" FILES ${template_src})

include_directories(
  ../../include
)

add_library(yate STATIC
  ${template_hdr}
  ${template_src}
)

target_compile_features(yate PRIVATE ${REQUIRED_CXX_FEATURES})
