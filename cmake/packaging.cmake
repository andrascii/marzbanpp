function(enable_packaging)
  set(options)
  set(oneValueArgs PACKAGE_NAME PACKAGE_VERSION PACKAGE_DISPLAY_NAME)
  set(multiValueArgs)

  cmake_parse_arguments(ARG "${options} ${oneValueArgs} ${multiValueArgs}" ${ARGN})

  if(NOT ARG_PACKAGE_NAME)
    message(FATAL_ERROR "PACKAGE_NAME must be set for enable_packaging")
  endif()

  if(NOT ARG_PACKAGE_VERSION)
    message(FATAL_ERROR "PACKAGE_VERSION must be set for enable_packaging")
  endif()

  include(InstallRequiredSystemLibraries)
  set(CPACK_GENERATOR "RPM")
  set(CPACK_PACKAGE_NAME ${ARG_PACKAGE_NAME})
  set(CPACK_PACKAGE_VERSION ${ARG_PACKAGE_VERSION})
  set(CPACK_COMPONENT_DISPLAY_NAME ${ARG_PACKAGE_DISPLAY_NAME})
  set(CPACK_PACKAGE_CONTACT "apugachevdev@gmail.com")
  set(CPACK_PACKAGE_VENDOR "Andrey Pugachev")
  set(CPACK_PACKAGE_DESCRIPTION "release")
  set(CPACK_PACKAGE_CHECKSUM SHA256)
  set(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_CURRENT_BINARY_DIR};CMake;ALL;/")
  set(CPACK_RPM_PACKAGE_EPOCH 0)
  set(CPACK_RPM_PACKAGE_DEBUG OFF)
  set(CPACK_RPM_PACKAGE_RELEASE 0)
  set(CPACK_RPM_PACKAGE_AUTOREQ ON)
  set(CPACK_RPM_PACKAGE_RELOCATABLE ON)

  execute_process(
    COMMAND
      "uname" "-m"
    OUTPUT_VARIABLE
      CPACK_RPM_PACKAGE_ARCHITECTURE
      OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  set(CPACK_RPM_PACKAGE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_RPM_PACKAGE_EPOCH}.${CPACK_RPM_PACKAGE_ARCHITECTURE}")
  set(CPACK_RPM_FILE_NAME ${CPACK_RPM_PACKAGE_NAME}.rpm)

  set_property(GLOBAL PROPERTY PACKAGING_PACKAGE_NAME ${ARG_PACKAGE_NAME})
  set_property(GLOBAL PROPERTY PACKAGING_PACKAGE_VERSION ${ARG_PACKAGE_VERSION})
  set_property(GLOBAL PROPERTY PACKAGING_PACKAGE_DIR ${CPACK_RPM_PACKAGE_NAME})

  file(READ ${CMAKE_CURRENT_SOURCE_DIR}/rpm/custom-preinstall.sh CUSTOM_PRE_INSTALL_CONTENTS)
  file(READ ${CMAKE_CURRENT_SOURCE_DIR}/rpm/custom-postinstall.sh CUSTOM_POST_INSTALL_CONTENTS)
  file(READ ${CMAKE_CURRENT_SOURCE_DIR}/rpm/custom-preuninstall.sh CUSTOM_PRE_UNINSTALL_CONTENTS)
  file(READ ${CMAKE_CURRENT_SOURCE_DIR}/rpm/custom-postuninstall.sh CUSTOM_POST_UNINSTALL_CONTENTS)

  set(RPM_LIBRARY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/rpm/")

  configure_file(
    ${RPM_LIBRARY_PATH}/generic.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/generic.sh
    @ONLY
    NEWLINE_STYLE UNIX
  )

  file(READ ${CMAKE_CURRENT_BINARY_DIR}/generic.sh GENERIC_PRE_AND_POST)

  configure_file(
    ${RPM_LIBRARY_PATH}/preinstall.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/preinstall.sh
    @ONLY
    NEWLINE_STYLE UNIX
  )

  set(CPACK_RPM_PRE_INSTALL_SCRIPT_FILE ${CMAKE_CURRENT_BINARY_DIR}/preinstall.sh)

  configure_file(
    ${RPM_LIBRARY_PATH}/postinstall.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/postinstall.sh
    @ONLY
    NEWLINE_STYLE UNIX
  )

  set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE ${CMAKE_CURRENT_BINARY_DIR}/postinstall.sh)

  configure_file(
    ${RPM_LIBRARY_PATH}/preuninstall.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/preuninstall.sh
    @ONLY
    NEWLINE_STYLE UNIX
  )

  set(CPACK_RPM_PRE_UNINSTALL_SCRIPT_FILE ${CMAKE_CURRENT_BINARY_DIR}/preuninstall.sh)

  configure_file(
    ${RPM_LIBRARY_PATH}/postuninstall.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/postuninstall.sh
    @ONLY
    NEWLINE_STYLE UNIX
  )

  set(CPACK_RPM_POST_UNINSTALL_SCRIPT_FILE ${CMAKE_CURRENT_BINARY_DIR}/postuninstall.sh)

  include(CPack)
endfunction()

function(package_add_target)
  set(options)
  set(oneValueArgs TARGET INSTALL_PATH)
  set(multiValueArgs)

  cmake_parse_arguments(ARG "${options} ${oneValueArgs} ${multiValueArgs}" ${ARGN})

  if(NOT ARG_TARGET)
    message(FATAL_ERROR "TARGET parameter was not set")
  endif()

  if(NOT ARG_INSTALL_PATH)
    set(INSTALL_PATH bin)
    message(STATUS "INSTALL_PATH was not set, by default it will set to 'bin'")
  else()
    set(INSTALL_PATH ${ARG_INSTALL_PATH})
  endif()

  install(TARGET ${ARG_TARGET} DESTINATION ${INSTALL_PATH})
endfunction()

function(package_add_files)
  set(options)
  set(oneValueArgs FILES INSTALL_PATH)
  set(multiValueArgs)

  cmake_parse_arguments(ARG "${options} ${oneValueArgs} ${multiValueArgs}" ${ARGN})

  if(NOT ARG_FILES)
    message(FATAL_ERROR "FILES parameter was not set")
  endif()

  if(NOT ARG_INSTALL_PATH)
    set(INSTALL_PATH .)
    message(STATUS "INSTALL_PATH was not set, by default it will set to '.'")
  else()
    set(INSTALL_PATH ${ARG_INSTALL_PATH})
  endif()

  install(
    FILES ${ARG_FILES}
    DESTINATION ${INSTALL_PATH}
    PERMISSIONS
      OWNER_EXECUTE
      OWNER_WRITE
      OWNER_READ
      GROUP_WRITE
      GROUP_READ
      GROUP_EXECUTE
  )
endfunction()
