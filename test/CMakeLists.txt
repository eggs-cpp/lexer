# Eggs.Lexer
#
# Copyright Agustin K-ballo Berge, Fusion Fenix 2017
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

set(_tests
  token.assign
  token.cnstr
  token.category
  token.raw
  token.tokenize)
set(_tests ${_tests}
  lexer.cnstr
  lexer.category_of
  lexer.function_call
  lexer.tokenize)
foreach (_test ${_tests})
  add_executable(test.${_test} ${_test}.cpp)
  target_link_libraries(test.${_test} Eggs::Lexer)

  add_test(NAME test.${_test} COMMAND test.${_test})
endforeach()

# Test for multiple definition errors
set(_contents "// This file is auto-generated by CMake to test for multiple definition errors.\n")

file(GLOB_RECURSE _headers
  RELATIVE "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_SOURCE_DIR}/include/*.hpp")
foreach (_header ${_headers})
  set(_contents "${_contents}#include <${_header}>\n")
endforeach()

set(_source_one "${CMAKE_CURRENT_BINARY_DIR}/multiple_definitions.one.cpp")
file(WRITE "${_source_one}" "${_contents}int main() {}\n")

set(_source_two "${CMAKE_CURRENT_BINARY_DIR}/multiple_definitions.two.cpp")
file(WRITE "${_source_two}" "${_contents}")

add_executable(test.multiple_definitions ${_source_one} ${_source_two})
target_link_libraries(test.multiple_definitions Eggs::Lexer)
add_test(NAME test.multiple_definitions COMMAND test.multiple_definitions)
