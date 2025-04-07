add_library(unix OBJECT
        impl.c
        sys.c
        timer_itimer.c
        $<$<BOOL:${HAVE_POSIX_TIMER}>:timer_posix.c>
)
target_include_directories(unix PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(unix PRIVATE core)

if (${TESTING})
    add_subdirectory(unit_tests)
endif ()
