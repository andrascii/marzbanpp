cmake_minimum_required(VERSION 3.16)

#
# by default generates project to build release package
#
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

option(USE_CLANG "Enables using clang as a compiler" ON)
option(BUILD_TESTS "Enables building tests project" OFF)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(BUILD_TESTS ON)
endif()

#
# by default on UNIX systems we are using clang compiler
#
if (UNIX AND NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND NOT CMAKE_CXX_COMPILER_ID MATCHES "AppleClang" AND USE_CLANG)
  set(CMAKE_C_COMPILER "clang")
  set(CMAKE_CXX_COMPILER "clang++")
endif()

project("marzbanpp" LANGUAGES C CXX)

#
# adding another one path to find .cmake files
#
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#
# adding settings for cl/clang compiler (depends on which compiler actully used)
#
include(clang_options)
include(msvc_set_target_options)

#
# function to register source files for reformatting by clang-format
#
include(add_clang_format)

message(STATUS "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")

find_package(GTest REQUIRED)
find_package(spdlog REQUIRED)
find_package(date CONFIG REQUIRED)
find_package(cxxopts CONFIG REQUIRED)
find_package(glaze REQUIRED)
find_package(CURL REQUIRED)

#
# C++ compiler settings
#
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_VERSION VERSION_LESS "3.7.0")
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

set(PROJECTS_DIR projects)
file(GLOB CHILDREN ${PROJECTS_DIR}/*)

foreach(CHILD ${CHILDREN})
  if (IS_DIRECTORY ${CHILD} AND EXISTS ${CHILD}/CMakeLists.txt)
    message(STATUS "${CHILD} is a directory and it contains CMakeLists.txt so it'll be added as a target")
    get_filename_component(TARGET_NAME ${CHILD} NAME)
    list(APPEND TARGETS ${TARGET_NAME})
  endif()
endforeach()

if (NOT TARGETS)
  message(FATAL "Not found targets")
endif()

foreach(TARGET ${TARGETS})
  add_subdirectory(${PROJECTS_DIR}/${TARGET})

  if (MSVC)
    msvc_set_target_options(TARGET_NAME ${TARGET})
  endif()

endforeach()

#
# collecting sources to register them for reformat by clang-format utility
#
foreach(MY_TARGET ${TARGETS})
  get_property(SOURCES TARGET ${MY_TARGET} PROPERTY SOURCES_PROPERTY)
  list(APPEND REFORMAT_SOURCES ${SOURCES})
endforeach()

#
# creating target 'reformat' to trigger calling clang-format utility for each source/header file in the project
#
add_clang_format(
  TARGET_NAME reformat
  SOURCES_LIST ${REFORMAT_SOURCES}
)
