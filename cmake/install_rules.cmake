# Project is configured with no languages, so tell GNUInstallDirs the lib dir
set(CMAKE_INSTALL_LIBDIR lib CACHE PATH "")

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# find_package(<package>) call for consumers to find this project
set(package marzbanpp)

install(
    DIRECTORY include
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT marzbanpp_Development
)

install(
    TARGETS marzbanpp
    EXPORT marzbanppTargets
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

write_basic_package_version_file(
    "${package}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
    ARCH_INDEPENDENT
)

# Allow package maintainers to freely override the path for the configs
set(
    marzbanpp_INSTALL_CMAKEDIR "${CMAKE_INSTALL_DATADIR}/${package}"
    CACHE PATH "CMake package config location relative to the install prefix"
)

mark_as_advanced(marzbanpp_INSTALL_CMAKEDIR)

install(
    FILES cmake/install_config.cmake
    DESTINATION "${marzbanpp_INSTALL_CMAKEDIR}"
    RENAME "${package}Config.cmake"
    COMPONENT marzbanpp_Development
)

install(
    FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
    DESTINATION "${marzbanpp_INSTALL_CMAKEDIR}"
    COMPONENT marzbanpp_Development
)

install(
    EXPORT marzbanppTargets
    NAMESPACE marzbanpp::
    DESTINATION "${marzbanpp_INSTALL_CMAKEDIR}"
    COMPONENT marzbanpp_Development
)