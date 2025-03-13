function(make_sanitize_variant)

  set(options)
  set(oneValueArgs VARIANT TARGET_NAME)
  set(multiValueArgs FLAGS DEPS)
  cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    list(APPEND SANITIZE_FLAGS ${ARG_FLAGS})

    if (ARG_PRECOMPILED_HEADER)
      target_precompile_headers(${ARG_TARGET_NAME} PRIVATE ${ARG_PRECOMPILED_HEADER})
    endif()

    message(STATUS "make_sanitized_target: TARGET_NAME - ${ARG_TARGET_NAME}")
    message(STATUS "make_sanitized_target: SOURCES - ${ARG_SOURCES}")
    message(STATUS "make_sanitized_target: HEADERS - ${ARG_HEADERS}")
    message(STATUS "make_sanitized_target: DEPS - ${ARG_DEPS}")
    message(STATUS "make_sanitized_target: PRECOMPILED_HEADER - ${ARG_PRECOMPILED_HEADER}")

    target_link_libraries(${ARG_TARGET_NAME}-${ARG_VARIANT} PRIVATE ${ARG_DEPS})
    target_compile_options(${ARG_TARGET_NAME}-${ARG_VARIANT} PRIVATE "$<$<CONFIG:DEBUG>:${SANITIZE_FLAGS}>")
    target_compile_options(${ARG_TARGET_NAME}-${ARG_VARIANT} PRIVATE "$<$<CONFIG:RELEASE>:${SANITIZE_FLAGS}>")
    target_compile_options(${ARG_TARGET_NAME}-${ARG_VARIANT} PRIVATE "$<$<CONFIG:DEBUG>:${SANITIZE_FLAGS}>")
    target_compile_options(${ARG_TARGET_NAME}-${ARG_VARIANT} PRIVATE "$<$<CONFIG:RELEASE>:${SANITIZE_FLAGS}>")
    target_link_options(${ARG_TARGET_NAME}-${ARG_VARIANT} PRIVATE "$<$<CONFIG:RELEASE>:${SANITIZE_FLAGS}>")
    target_link_options(${ARG_TARGET_NAME}-${ARG_VARIANT} PRIVATE "$<$<CONFIG:DEBUG>:${SANITIZE_FLAGS}>")
  endif()

endfunction()

function(make_sanitized_target)

  set(options)
  set(oneValueArgs TARGET_NAME PRECOMPILED_HEADER)
  set(multiValueArgs SOURCES HEADERS DEPS)
  cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  make_sanitize_variant(
    TARGET_NAME ${ARG_TARGET_NAME}
    VARIANT tsan
    FLAGS -g -fno-omit-frame-pointer -fsanitize=thread
    DEPS ${ARG_DEPS}
  )

  make_sanitize_variant(
    TARGET_NAME ${ARG_TARGET_NAME}
    VARIANT asan
    FLAGS -g -fno-omit-frame-pointer -fsanitize=address,undefined -fno-sanitize-recover=undefined
    DEPS ${ARG_DEPS}
  )

  make_sanitize_variant(
    TARGET_NAME ${ARG_TARGET_NAME}
    VARIANT ubsan
    FLAGS -g -fno-omit-frame-pointer -fsanitize=undefined
    DEPS ${ARG_DEPS}
  )

endfunction()
