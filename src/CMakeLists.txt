cmake_minimum_required(VERSION 3.16)

include(make_static_library_sanitized_target)

set(THIS_TARGET_NAME marzbanpp)
project(${THIS_TARGET_NAME})

#
# precompiled header
#
set(PRECOMPILED_HEADER "${CMAKE_SOURCE_DIR}/include/marzbanpp/stdafx.h")

#
# deps include directories
#
list(APPEND ADDITIONAL_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/include")

#
# deps
#
list(APPEND DEPS glaze::glaze)
list(APPEND DEPS CURL::libcurl)
list(APPEND DEPS fmt::fmt)

#
# collecting sources and headers
#
file(GLOB_RECURSE TYPES_HEADERS "${CMAKE_SOURCE_DIR}/include/marzbanpp/types/*.h")
file(GLOB_RECURSE NET_HEADERS "${CMAKE_SOURCE_DIR}/include/marzbanpp/net/*.h")
file(GLOB_RECURSE HEADERS_LIST "${CMAKE_SOURCE_DIR}/include/marzbanpp/*.h")
file(GLOB_RECURSE SOURCES_LIST "${CMAKE_SOURCE_DIR}/src/*.cpp")
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

#
# creating target with sanitizers
#
if(DEVELOPER_MODE)
  make_static_library_sanitized_target(
    TARGET_NAME marzbanpp
    PRECOMPILED_HEADER ${PRECOMPILED_HEADER}
    HEADERS ${HEADERS_LIST}
    SOURCES ${SOURCES_LIST}
    DEPS ${DEPS}
  )
endif()
