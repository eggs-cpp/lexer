# Eggs.Lexer
#
# Copyright Agustin K-ballo Berge, Fusion Fenix 2017
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.0)

project(Eggs.Lexer CXX)

if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  option(WITH_TESTS "Compile with unit tests." ON)
  set(HAVE_TESTS ${WITH_TESTS})
else()
  option(EGGS_LEXER_WITH_TESTS "Compile with unit tests." ON)
  set(HAVE_TESTS ${EGGS_LEXER_WITH_TESTS})
endif()

# Build
add_library(_eggs_lexer INTERFACE)
target_include_directories(_eggs_lexer INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
set_target_properties(_eggs_lexer
  PROPERTIES EXPORT_NAME Eggs::Lexer)

add_library(Eggs::Lexer ALIAS _eggs_lexer)

# Test
if (HAVE_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()

# Install
set(_headers
  eggs/lexer.hpp
  eggs/lexer/lexer.hpp)
foreach (_header ${_headers})
  get_filename_component(_directory "${_header}" DIRECTORY)
  install(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/include/${_header}"
    DESTINATION "include/${_directory}")
endforeach()

install(TARGETS _eggs_lexer EXPORT _targets)
install(EXPORT _targets
  DESTINATION lib/cmake/eggs.lexer
  FILE eggs.lexer-targets.cmake)

include(CMakePackageConfigHelpers)

configure_file(
  cmake/eggs.lexer-config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/eggs.lexer-config.cmake"
  @ONLY)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/eggs.lexer-config-version.cmake"
  VERSION 0.0.0
  COMPATIBILITY AnyNewerVersion)

install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/eggs.lexer-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/eggs.lexer-config-version.cmake"
  DESTINATION lib/cmake/eggs.lexer)
