function(msvc_set_target_options)
  set(options)
  set(oneValueArgs TARGET_NAME)
  set(multiValueArgs)
  cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  target_compile_options(${ARG_TARGET_NAME} PRIVATE "$<$<CONFIG:RELEASE>:/O2>")
  target_compile_options(${ARG_TARGET_NAME} PRIVATE "$<$<CONFIG:RELEASE>:/MD>")
  target_compile_options(${ARG_TARGET_NAME} PRIVATE "$<$<CONFIG:DEBUG>:/MDd>")

  if(DEVELOPER_MODE)
    target_compile_options(
      ${ARG_TARGET_NAME}
      PRIVATE
      /EHa
      /MP
      /permissive-
      /Zi
      /W4
      /WX
      /GR
    )
  else()
    target_compile_options(
      ${ARG_TARGET_NAME}
      PRIVATE
      /EHsc
      /MP
      /permissive-
      /Zi
      /W3
      /WX:NO
      /GR
    )
  endif()
endfunction()