# SPDX-License-Identifier: BSD-2-Clause

add_executable(test_time_ops test_time_ops.c)
target_link_libraries(test_time_ops unity core)
add_test(time_ops_suite test_time_ops)

add_executable(test_list test_list.c)
target_link_libraries(test_list unity core)
add_test(list_suite test_list)
