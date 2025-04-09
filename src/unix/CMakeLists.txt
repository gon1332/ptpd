set(linux_sources sys_linux.c)
set(bsd_sources sys_bsd.c)
set(qnx_sources sys_qnx.c)

add_library(unix OBJECT
        impl.c
        sys.c
        timer_itimer.c
        $<$<BOOL:${HAVE_POSIX_TIMER}>:timer_posix.c>
)

if (${CMAKE_SYSTEM_NAME} STREQUAL Linux)
    target_sources(unix PRIVATE ${linux_sources})
elseif (${CMAKE_SYSTEM_NAME} STREQUAL FreeBSD)
    target_sources(unix PRIVATE ${bsd_sources})
elseif (${CMAKE_SYSTEM_NAME} STREQUAL NetBSD)
    target_sources(unix PRIVATE ${bsd_sources})
elseif (${CMAKE_SYSTEM_NAME} STREQUAL OpenBSD)
    target_sources(unix PRIVATE ${bsd_sources})
elseif (${CMAKE_SYSTEM_NAME} STREQUAL Darwin)
    target_sources(unix PRIVATE ${bsd_sources})
elseif (${CMAKE_SYSTEM_NAME} STREQUAL QNX)
    target_sources(unix PRIVATE ${qnx_sources})
endif ()

target_include_directories(unix PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(unix PRIVATE core)

if (${TESTING})
    add_subdirectory(unit_tests)
endif ()
