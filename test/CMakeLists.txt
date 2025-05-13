#
# Copyright Quadrivium LLC
# All Rights Reserved
# SPDX-License-Identifier: Apache-2.0
#

function(addtest test_name)
    add_executable(${test_name} ${ARGN})
    addtest_part(${test_name} ${ARGN})
    target_link_libraries(${test_name}
            GTest::gmock_main
    )
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/xunit)
    set(xml_output "--gtest_output=xml:${CMAKE_BINARY_DIR}/xunit/xunit-${test_name}.xml")
    add_test(
            NAME ${test_name}
            COMMAND $<TARGET_FILE:${test_name}> ${xml_output} "--output-on-failure"
    )
    set_target_properties(${test_name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test_bin
            ARCHIVE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/test_lib
            LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/test_lib
    )
endfunction()

function(addtest_part test_name)
    if(POLICY CMP0076)
        cmake_policy(SET CMP0076 NEW)
    endif()
    target_sources(${test_name} PUBLIC
            ${ARGN}
    )
endfunction()

addtest(byte_vec_test
    byte_vec_test.cpp
    )
target_link_libraries(byte_vec_test
    fmt::fmt
    )

addtest(byte_vec_or_view
    byte_vec_or_view.cpp
    )

addtest(byte_arr_test
    byte_arr_test.cpp
    )

addtest(size_limited_containers_test
    size_limited_containers_test.cpp
    )
target_link_libraries(size_limited_containers_test
    fmt::fmt
    )

addtest(shared_ref_test
    shared_ref_test.cpp
)

addtest(visit_in_place_test
    visit_in_place_test.cpp
)
