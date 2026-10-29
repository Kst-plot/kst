# SPDX-FileCopyrightText: 2026 C. Barth Netterfield <netterfield@astro.utoronto.ca>
# SPDX-License-Identifier: GPL-2.0-or-later

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS Test)

add_executable(testmatrixframecount testmatrixframecount.cpp)
target_link_libraries(testmatrixframecount PRIVATE Kst6Core Qt6::Test)
add_test(NAME testmatrixframecount COMMAND testmatrixframecount)
set_tests_properties(testmatrixframecount PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# Existing matrix tests that do not depend on datasource plugins or fixtures.
foreach(test_name testmatrix testeditablematrix testgeneratedmatrix)
    add_executable(${test_name} ../${test_name}.cpp ../${test_name}.h)
    target_link_libraries(${test_name} PRIVATE Kst6Core Qt6::Test)
    add_test(NAME ${test_name} COMMAND ${test_name})
    set_tests_properties(${test_name} PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
endforeach()

if(TARGET ssdb_rust_lib)
    # Compile the actual adapter against a deterministic C API stub, not a live DB.
    add_executable(testssdbmatrix
        testssdbmatrix.cpp
        ${PROJECT_SOURCE_DIR}/src/datasources/ssdb/ssdbsource.cpp
        ${PROJECT_SOURCE_DIR}/src/datasources/ssdb/ssdbsource.h)
    target_include_directories(testssdbmatrix PRIVATE
        ${PROJECT_SOURCE_DIR}/src/datasources/ssdb
        ${SSDB_WORKSPACE_ROOT}/telemetry_c/c)
    target_link_libraries(testssdbmatrix PRIVATE Kst6Core Qt6::Test)
    add_dependencies(testssdbmatrix ssdb_rust_lib)
    add_test(NAME testssdbmatrix COMMAND testssdbmatrix)
    set_tests_properties(testssdbmatrix PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
endif()