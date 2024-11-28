# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.21)
project(ptpd LANGUAGES C ASM)

include(CTest)
add_subdirectory(external/Unity)
add_subdirectory(src)
