cmake_minimum_required(VERSION 3.21)
project(ptpd C ASM)
set(CMAKE_C_STANDARD 11)

option(POSIX_TIMERS "Enable POSIX timers" ON)
option(SLAVE_ONLY "Build a slave-only version incapable of running as a PTP master" OFF)
option(SNMP "Enable SNMP (Not supported by Camlin)" OFF)
option(STATISTICS "Enable statistics" OFF)

if (SNMP)
    # Provided with libsnmp-dev on Ubuntu
    message(WARNING "SNMP build has not been tested")
    find_library(NETSNMPAGENT "netsnmpagent" REQUIRED)
    find_library(NETSNMPMIBS "netsnmpmibs" REQUIRED)
    find_library(NETSNMP "netsnmp" REQUIRED)
endif ()

add_compile_definitions(HAVE_CONFIG_H)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(src)
