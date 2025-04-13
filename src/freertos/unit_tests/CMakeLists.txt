# SPDX-License-Identifier: BSD-2-Clause

add_executable(test_ftimer test_timer.c)
target_include_directories(test_ftimer PRIVATE $<TARGET_PROPERTY:freertos,INCLUDE_DIRECTORIES>)
target_link_libraries(test_ftimer unity core freertos)
add_test(ftimer_suite test_ftimer)
