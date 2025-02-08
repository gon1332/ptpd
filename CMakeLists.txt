# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.21)
project(ptpd LANGUAGES C ASM)

option(SNMP "Enable SNMP" OFF)
option(STATISTICS "Enable statistics" OFF)

set(PACKAGE_NAME "ptpd")
set(PACKAGE_VERSION "2.3.2")

if (${TESTING})
    include(CTest)
    add_subdirectory(external/Unity)
else ()
    include(scripts/utils.cmake)
    generate_configuration(${CMAKE_CURRENT_SOURCE_DIR}/ptpd_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/ptpd_config.h)
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
endif ()

add_subdirectory(src)
