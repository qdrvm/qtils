#
# Copyright Quadrivium LLC
# All Rights Reserved
# SPDX-License-Identifier: Apache-2.0
#

cmake_minimum_required(VERSION 3.12)

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.27")
    cmake_policy(SET CMP0144 NEW)
endif()

include(cmake/HunterGate.cmake)
HunterGate(
    URL  https://github.com/qdrvm/hunter/archive/refs/tags/v0.25.3-qdrvm11.zip
    SHA1 20620ee1e2e9738b2d4561be2622d71e8c73bee9
)

project(qtils LANGUAGES CXX VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(QTILS_BUILD_TESTS "Build tests" OFF)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    option(QTILS_ENABLE_ASSERT "Enable assertions" ON)
else()
    option(QTILS_ENABLE_ASSERT "Enable assertions" OFF)
endif()

hunter_add_package(Boost)
find_package(Boost CONFIG REQUIRED)
hunter_add_package(fmt)
find_package(fmt CONFIG REQUIRED)

add_library(qtils INTERFACE)
target_link_libraries(qtils INTERFACE
    Boost::boost
    fmt::fmt
)
if (QTILS_ENABLE_ASSERT)
    target_compile_definitions(qtils INTERFACE QTILS_ENABLE_ASSERT)
endif()

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
endif()
