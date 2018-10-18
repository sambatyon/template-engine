cmake_minimum_required(VERSION 3.12)

set(PROJECT_PREFIX "template-engine")
project(${PROJECT_PREFIX})

# Simple overrides to prevent VS from storing files in weird locations.
set(BIN_OUTPUT_DIR ${${PROJECT_PREFIX}_BINARY_DIR}/bin)
set(LIB_OUTPUT_DIR ${${PROJECT_PREFIX}_BINARY_DIR}/lib)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR})

include(CheckCXXCompilerFlag)

# Allows cmake to choose the best standard for compilation.
# set(REQUIRED_CXX_FEATURES
#   cxx_auto_type
#   cxx_constexpr
#   cxx_deleted_functions
#   cxx_lambdas
#   cxx_nullptr
#   cxx_override
#   cxx_range_for
#   cxx_variadic_templates
#)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

ADD_DEFINITIONS(-DPACKAGE_NAME=${PROJECT_PREFIX})

# Small macro to get compiled colorized output, useful dealing with
# errors.
macro(AddCXXFlagIfSupported flag test)
  check_cxx_compiler_flag(${flag} ${test})
  if (${${test}})
    message("adding ${flag}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
  endif()
endmacro()

if ("Ninja" STREQUAL ${CMAKE_GENERATOR})
  AddCXXFlagIfSupported(-fcolor-diagnostics COMPILER_SUPPORTS_fcolor-diagnostics)
endif()

# Project sources definitions.
include_directories(include)
add_subdirectory(src)
add_subdirectory(tests)
