set(CMAKE_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/wildmatch")

write_basic_package_version_file(
    "wildmatch-config-version.cmake"
    VERSION "${wildmatch_MAJOR_VERSION}.${wildmatch_MINOR_VERSION}"
    COMPATIBILITY AnyNewerVersion)

configure_package_config_file(
    "wildmatch-config.cmake"
    "wildmatch-config.cmake"
    INSTALL_DESTINATION "${CMAKE_DIR}")

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/wildmatch-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/wildmatch-config-version.cmake"
    DESTINATION "${CMAKE_DIR}"
    COMPONENT "devel")

install(
    EXPORT wildmatch NAMESPACE wildmatch::
    FILE "wildmatch-exports.cmake"
    DESTINATION "${CMAKE_DIR}"
    COMPONENT "devel")
