function (add_clang_format)

  set(options)
  set(oneValueArgs TARGET_NAME)
  set(multiValueArgs SOURCES_LIST)
  cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  message(STATUS "add_clang_format: TARGET_NAME - ${ARG_TARGET_NAME}")
  message(STATUS "add_clang_format: SOURCES_LIST - ${ARG_SOURCES_LIST}")

  if (UNIX)
    if (APPLE AND CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")  # On Mac OS for AppleClang compiler
      find_program(XCRUN NAMES "xcrun" REQUIRED)

      if (TARGET ${ARG_TARGET_NAME})
        add_custom_command(
          TARGET ${ARG_TARGET_NAME} POST_BUILD
          COMMAND ${XCRUN} clang-format --style=file -i ${ARG_SOURCES_LIST}
        )
      else()
        add_custom_target(
          ${ARG_TARGET_NAME}
          COMMAND ${XCRUN} clang-format --style=file -i ${ARG_SOURCES_LIST}
        )
      endif()

    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")  # On Unix systems for Clang compiler
      find_program(CLANG_FORMAT NAMES "clang-format" REQUIRED)

      if (TARGET ${ARG_TARGET_NAME})
        add_custom_command(
          TARGET ${ARG_TARGET_NAME} POST_BUILD
          COMMAND ${CLANG_FORMAT} --style=file -i ${ARG_SOURCES_LIST}
        )
      else()
        add_custom_target(
          ${ARG_TARGET_NAME}
          COMMAND ${CLANG_FORMAT} --style=file -i ${ARG_SOURCES_LIST}
        )
      endif()
    endif()
  endif()

endfunction()