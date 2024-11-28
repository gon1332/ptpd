add_library(time_ops OBJECT time_ops.c)
target_include_directories(time_ops PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(unit_tests)
