# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.21)
project(ptpd LANGUAGES C ASM)

option(STATISTICS "Enable statistics" ON)

set(PACKAGE_NAME "ptpd")
set(PACKAGE_VERSION "2.3.2")

# Compatibility Configuration
include(CheckIncludeFiles)
check_include_files(net/ethernet.h HAVE_NET_ETHERNET_H)
check_include_files(netinet/ether.h HAVE_NETINET_ETHER_H)
check_include_files(netdb.h HAVE_NETDB_H)
check_include_files(sys/socket.h HAVE_SYS_SOCKET_H)
check_include_files(sys/types.h HAVE_SYS_TYPES_H)
include(CheckFunctionExists)
check_function_exists(gethostbyname2 HAVE_GETHOSTBYNAME2)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config_cmake.h.in ${CMAKE_CURRENT_BINARY_DIR}/config_cmake.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_compile_definitions(HAVE_CONFIG_H)

include(CTest)
add_subdirectory(external/Unity)
add_subdirectory(src)
