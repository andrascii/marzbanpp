if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  add_compile_options("$<$<CONFIG:DEBUG>:-O0>")
  add_compile_options("$<$<CONFIG:RELEASE>:-O3>")
  add_compile_options("-Wall")
  add_compile_options("-Wextra")
  add_compile_options("-Werror")
  #
  # Valid only for x86-64 architecture.
  # Need to support this check and optionally disable it for arm64.
  #
  #add_compile_options("-msse4.2")
  add_compile_options("-Werror=return-type")
  add_compile_options("-Wno-implicit-fallthrough")
  add_compile_options("-Wno-unused-parameter")
  add_compile_options("-pedantic")
  add_compile_options("-Wunknown-pragmas")
  add_compile_options("-Wunused-command-line-argument")
  add_compile_options("-g")

  if (CMAKE_BUILD_TYPE STREQUAL Debug)
    add_compile_options("-O0")
  endif()
endif()
