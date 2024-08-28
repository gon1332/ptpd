cmake_minimum_required(VERSION 3.21)
project(ptpd C ASM)
set(CMAKE_C_STANDARD 11)

option(SNMP "Enable SNMP" OFF)
option(STATISTICS "Enable statistics" OFF)
option(POSIX_TIMERS "Enable POSIX timers" ON)

add_compile_definitions(HAVE_CONFIG_H)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(src)