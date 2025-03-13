#
# For Debug build using Clang we building tests with enabled code coverage flags
# adding custom 'coverage' target to start llvm-profdata and llvm-cov utilities
# to analyze code coverage
#

function(create_coverage_target)

  set(options)
  set(oneValueArgs TARGET_NAME)
  set(multiValueArgs)
  cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  list(APPEND COVERAGE_FLAGS -fcoverage-mapping)
  list(APPEND COVERAGE_FLAGS -fprofile-instr-generate)

  target_compile_options(${ARG_TARGET_NAME} PRIVATE "$<$<CONFIG:DEBUG>:${COVERAGE_FLAGS}>")
  target_link_options(${ARG_TARGET_NAME} PRIVATE "$<$<CONFIG:DEBUG>:${COVERAGE_FLAGS}>")
  add_custom_target(coverage COMMAND ${ARG_TARGET_NAME} DEPENDS ${ARG_TARGET_NAME})

endfunction()

function(make_coverage_target)

  set(options)
  set(oneValueArgs TARGET_NAME)
  set(multiValueArgs)
  cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  message(STATUS "make_coverage_target: TARGET_NAME - ${ARG_TARGET_NAME}")

  if (UNIX)
    if (APPLE AND CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")  # On Mac OS for AppleClang compiler
      create_coverage_target(TARGET_NAME ${ARG_TARGET_NAME})

      find_program(XCRUN NAMES "xcrun" REQUIRED)

      add_custom_command(TARGET coverage POST_BUILD
        COMMAND ${XCRUN}
          llvm-profdata
          merge
          -sparse default.profraw
          -o ${ARG_TARGET_NAME}.profdata
      )

      add_custom_command(TARGET coverage POST_BUILD
        COMMAND ${XCRUN}
          llvm-cov
          show
          -format=html
          ${CMAKE_CURRENT_BINARY_DIR}/${ARG_TARGET_NAME}
          -instr-profile=${ARG_TARGET_NAME}.profdata
          --show-line-counts-or-regions
          -Xdemangler=c++filt
          --use-color > coverate.html
      )

      add_custom_command(TARGET coverage POST_BUILD
        COMMAND ${XCRUN}
          llvm-cov
          report
          ${CMAKE_CURRENT_BINARY_DIR}/${ARG_TARGET_NAME}
          -instr-profile=${ARG_TARGET_NAME}.profdata
          -Xdemangler=c++filt
      )

    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")  # On Unix systems for Clang compiler
      create_coverage_target(TARGET_NAME ${ARG_TARGET_NAME})

      find_program(LLVM_COV NAMES "llvm-cov" REQUIRED)
      find_program(LLVM_PROFDATA NAMES "llvm-profdata" REQUIRED)

      add_custom_command(TARGET coverage POST_BUILD
        COMMAND ${LLVM_PROFDATA}
          merge
          -sparse default.profraw
          -o ${ARG_TARGET_NAME}.profdata
      )

      add_custom_command(TARGET coverage POST_BUILD
        COMMAND ${LLVM_COV}
          show
          -format=html
          ./${ARG_TARGET_NAME}
          -instr-profile=${ARG_TARGET_NAME}.profdata
          --show-line-counts-or-regions
          -Xdemangler=c++filt
          --use-color > coverate.html
      )

      add_custom_command(TARGET coverage POST_BUILD
        COMMAND ${LLVM_COV}
          report
          ./${ARG_TARGET_NAME}
          -instr-profile=${ARG_TARGET_NAME}.profdata
          -Xdemangler=c++filt
      )

    endif()
  endif()

endfunction()
