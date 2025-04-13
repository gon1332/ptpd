# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.22)
project(ptpd LANGUAGES C ASM)

option(SNMP "Enable SNMP" OFF)
option(STATISTICS "Enable statistics" OFF)

set(PACKAGE_NAME "ptpd")
set(PACKAGE_VERSION "2.3.2")

include(scripts/utils.cmake)
generate_configuration(${CMAKE_CURRENT_SOURCE_DIR}/ptpd_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/ptpd_config.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if (${TESTING})
    include(CTest)
endif ()
ptpd_add_subdirectory_if(TESTING external/Unity)

add_subdirectory(src)
