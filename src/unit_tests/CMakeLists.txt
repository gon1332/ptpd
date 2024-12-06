# SPDX-License-Identifier: BSD-2-Clause

add_executable(test_time_ops test_time_ops.c)
target_include_directories(test_time_ops PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(test_time_ops unity time_ops)
add_test(time_ops_suite test_time_ops)
