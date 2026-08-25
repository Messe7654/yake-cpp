cmake_minimum_required(VERSION 3.16)

project(yake_cpp
    VERSION 0.1.0
    DESCRIPTION "C++ implementation of the YAKE unsupervised keyword extraction algorithm"
    LANGUAGES CXX
)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
include(cmake/yake-options.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Build type" FORCE)
endif()

if(PROJECT_IS_TOP_LEVEL)
    set(YAKE_BUILD_DEFAULT ON)
else()
    set(YAKE_BUILD_DEFAULT OFF)
endif()

option(YAKE_BUILD_TESTS "Build the yake-cpp unit tests" ${YAKE_BUILD_DEFAULT})
option(YAKE_BUILD_EXAMPLES "Build the yake-cpp examples" ${YAKE_BUILD_DEFAULT})
option(YAKE_BUILD_BENCHMARKS "Build the yake-cpp benchmarks" OFF)
option(YAKE_ENABLE_WARNINGS "Enable compiler warnings for yake-cpp targets" ON)
option(YAKE_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)
option(YAKE_ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)

find_program(YAKE_CLANG_FORMAT_EXECUTABLE NAMES clang-format)
if(YAKE_CLANG_FORMAT_EXECUTABLE)
    file(GLOB_RECURSE YAKE_FORMAT_FILES CONFIGURE_DEPENDS
        ${PROJECT_SOURCE_DIR}/include/*.h
        ${PROJECT_SOURCE_DIR}/src/*.h
        ${PROJECT_SOURCE_DIR}/src/*.cc
        ${PROJECT_SOURCE_DIR}/examples/*.cc
        ${PROJECT_SOURCE_DIR}/benchmarks/*.cc
        ${PROJECT_SOURCE_DIR}/tests/*.cc
    )

    add_custom_target(yake_format
        COMMAND ${YAKE_CLANG_FORMAT_EXECUTABLE} -i ${YAKE_FORMAT_FILES}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Formatting yake-cpp sources"
    )

    add_custom_target(yake_format_check
        COMMAND ${YAKE_CLANG_FORMAT_EXECUTABLE} --dry-run --Werror ${YAKE_FORMAT_FILES}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Checking yake-cpp source formatting"
    )
endif()

add_subdirectory(src)

configure_package_config_file(
    cmake/yake-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/yake-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/yake
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/yake-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    TARGETS yake
    EXPORT yake-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    DIRECTORY include/yake
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    EXPORT yake-targets
    FILE yake-targets.cmake
    NAMESPACE yake::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/yake
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/yake-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/yake-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/yake
)

install(
    FILES
        LICENSE
        NOTICE
        THIRD_PARTY_NOTICES.md
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/doc/yake
)

install(
    DIRECTORY LICENSES
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/doc/yake
)

if(YAKE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(YAKE_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

if(YAKE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
