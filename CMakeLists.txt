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
    URL  https://github.com/qdrvm/hunter/archive/refs/tags/v0.23.257-qdrvm11.tar.gz
    SHA1 b2a69ae501bdc99006fb86e55930640004468556
)

project(qtils LANGUAGES CXX VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(GNUInstallDirs)

hunter_add_package(Boost)
find_package(Boost CONFIG REQUIRED)
hunter_add_package(fmt)
find_package(fmt CONFIG REQUIRED)

add_library(qtils INTERFACE)
target_link_libraries(qtils INTERFACE
    Boost::boost
    fmt::fmt
)
target_include_directories(qtils INTERFACE
    $<BUILD_INTERFACE:src/qtils>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(DIRECTORY src/qtils
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)
install(TARGETS qtils EXPORT qtils-config)
install(EXPORT qtils-config
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/qtils
    NAMESPACE qtils::
)
