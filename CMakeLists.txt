#
# Copyright Quadrivium LLC
# All Rights Reserved
# SPDX-License-Identifier: Apache-2.0
#

cmake_minimum_required(VERSION 3.12)

option(QTILS_BUILD_TESTS "Build tests" OFF)

if (PACKAGE_MANAGER)
    if(PACKAGE_MANAGER NOT MATCHES "^(hunter|vcpkg)$")
        message(FATAL_ERROR "PACKAGE_MANAGER must be set to 'hunter', 'vcpkg' or left undefined")
    endif ()
else ()
    set(PACKAGE_MANAGER "hunter")
    if (CMAKE_TOOLCHAIN_FILE)
        get_filename_component(ACTUAL_NAME ${CMAKE_TOOLCHAIN_FILE} NAME)
        if(ACTUAL_NAME STREQUAL "vcpkg.cmake")
            message(STATUS "vcpkg will be used because vcpkg.cmake has found")
            set(PACKAGE_MANAGER "vcpkg")
        endif ()
    endif ()
endif ()
message(STATUS "Selected package manager: ${PACKAGE_MANAGER}")

if (PACKAGE_MANAGER STREQUAL "hunter")
    include(cmake/HunterGate.cmake)
    HunterGate(
        URL  https://github.com/qdrvm/hunter/archive/refs/tags/v0.25.3-qdrvm29.tar.gz
        SHA1 025920fa980ba81a150deaa534a0248dde25fd54
    )
endif ()

if(QTILS_BUILD_TESTS)
    if (PACKAGE_MANAGER STREQUAL "vcpkg")
        list(APPEND VCPKG_MANIFEST_FEATURES qtils-tests)
    endif()
endif()

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.27")
    cmake_policy(SET CMP0144 NEW)
endif()

project(qtils LANGUAGES CXX VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    option(QTILS_ENABLE_ASSERT "Enable assertions" ON)
else()
    option(QTILS_ENABLE_ASSERT "Enable assertions" OFF)
endif()

option(FORMAT_ERROR_WITH_FULLTYPE "Use the full type of the category when formatting errors" OFF)

if (PACKAGE_MANAGER STREQUAL "hunter")
    hunter_add_package(Boost)
    find_package(Boost)
else ()
    find_package(Boost REQUIRED COMPONENTS algorithm outcome)
endif ()

if (PACKAGE_MANAGER STREQUAL "hunter")
    hunter_add_package(fmt)
endif ()
find_package(fmt CONFIG REQUIRED)

add_library(qtils INTERFACE)
target_link_libraries(qtils INTERFACE
    Boost::algorithm
    Boost::outcome
    fmt::fmt
)
if (QTILS_ENABLE_ASSERT)
    target_compile_definitions(qtils INTERFACE QTILS_ENABLE_ASSERT)
endif()
if (FORMAT_ERROR_WITH_FULLTYPE)
    target_compile_definitions(qtils INTERFACE FORMAT_ERROR_WITH_FULLTYPE)
endif ()

include(GNUInstallDirs)

target_include_directories(qtils INTERFACE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(DIRECTORY src/qtils
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)
install(TARGETS qtils EXPORT qtils-config)
install(EXPORT qtils-config
    DESTINATION share/qtils
    NAMESPACE qtils::
)
install(FILES LICENSE DESTINATION ${CMAKE_INSTALL_PREFIX}/share/qtils/copyright)

if (QTILS_BUILD_TESTS)
    enable_testing()

    # since it's hard to check locally for a header-only library, generate a file that includes all headers
    # and check that it compiles

    file(GLOB_RECURSE headers RELATIVE "${PROJECT_SOURCE_DIR}/src/" CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/qtils/*.hpp") 
    set(qtils_compiles_content "// THIS FILE IS GENERATED AUTOMATICALLY\n")
    foreach(file ${headers})
        string(APPEND qtils_compiles_content "#include <${file}>\n")
    endforeach()
    string(APPEND qtils_compiles_content "int main(){}\n")

    file(WRITE "${PROJECT_SOURCE_DIR}/test/qtils_compiles.cpp" "${qtils_compiles_content}")
    
    add_executable(qtils_compiles test/qtils_compiles.cpp)
    target_link_libraries(qtils_compiles qtils)

    # Unit tests

    find_package(GTest CONFIG REQUIRED)
    set(GTEST_DEPS GTest::gtest_main)

    include_directories(src)

    add_subdirectory(test)
endif()
