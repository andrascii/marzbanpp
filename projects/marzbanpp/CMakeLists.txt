cmake_minimum_required(VERSION 3.16)

include(make_executable_sanitized_target)

set(THIS_TARGET_NAME marzbanpp)
project(${THIS_TARGET_NAME})

#
# precompiled header
#
set(PRECOMPILED_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h")

#
# deps include directories
#
list(APPEND ADDITIONAL_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/projects/marzbanpp")

#
# deps
#
list(APPEND DEPS date::date)
list(APPEND DEPS glaze::glaze)
list(APPEND DEPS CURL::libcurl)

#
# collecting sources and headers
#
file(GLOB_RECURSE TYPES_SOURCES "*.cpp")
file(GLOB_RECURSE TYPES_HEADERS "*.h")

file(GLOB_RECURSE NET_SOURCES "*.cpp")
file(GLOB_RECURSE NET_HEADERS "*.h")

file(GLOB_RECURSE SOURCES_LIST "*.cpp")
file(GLOB_RECURSE HEADERS_LIST "*.h")

list(APPEND SOURCES_LIST ${TYPES_SOURCES} ${NET_SOURCES})
list(APPEND HEADERS_LIST ${TYPES_HEADERS} ${NET_HEADERS})

#
# adding include directories to created target
#
include_directories(${ADDITIONAL_INCLUDE_DIRECTORIES})

#
# creating target
#
add_library(${THIS_TARGET_NAME} STATIC ${HEADERS_LIST} ${SOURCES_LIST})

#
# adding sources property to target
#
set_property(
  TARGET ${THIS_TARGET_NAME}
  PROPERTY SOURCES_PROPERTY ${HEADERS_LIST} ${SOURCES_LIST}
)

#
# adding precompiled header
#
target_precompile_headers(${THIS_TARGET_NAME} PRIVATE ${PRECOMPILED_HEADER})

#
# linking this target with other targets
#
target_link_libraries(${THIS_TARGET_NAME} PUBLIC ${DEPS})
