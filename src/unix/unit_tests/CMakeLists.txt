# SPDX-License-Identifier: BSD-2-Clause

add_executable(test_timer test_timer.c)
target_include_directories(test_timer PRIVATE $<TARGET_PROPERTY:unix,INCLUDE_DIRECTORIES>)
target_link_libraries(test_timer unity core unix)
add_test(timer_suite test_timer)
