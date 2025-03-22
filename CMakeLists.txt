cmake_minimum_required(VERSION 3.16)

#
# by default generates project to build release package
#
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

option(BUILD_TESTS "Enables building tests project" OFF)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(BUILD_TESTS ON)
endif()

project(
  "marzbanpp"
  VERSION 1.0.1
  LANGUAGES CXX
)

#
# adding another one path to find .cmake files
#
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(GNUInstallDirs)

#
# adding settings for cl/clang compiler (depends on which compiler actully used)
#
include(clang_options)
include(msvc_set_target_options)

message(STATUS "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")

#
# packages
#
find_package(GTest REQUIRED)
find_package(spdlog REQUIRED)
find_package(date CONFIG REQUIRED)
find_package(cxxopts CONFIG REQUIRED)
find_package(glaze REQUIRED)
find_package(CURL REQUIRED)

#
# C++ compiler settings
#
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_VERSION VERSION_LESS "3.7.0")
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

add_subdirectory(src)
add_subdirectory(examples)

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(install_rules)
endif()