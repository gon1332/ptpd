set(linux_sources sys_linux.c)
set(bsd_sources sys_bsd.c)
set(qnx_sources sys_qnx.c)

add_library(unix OBJECT
        impl.c
        startup.c
        sys.c
        timer_itimer.c
        $<$<BOOL:${HAVE_POSIX_TIMER}>:timer_posix.c>
)

if (${ptpd_for_linux})
    target_sources(unix PRIVATE ${linux_sources})
elseif (${ptpd_for_freebsd} OR ${ptpd_for_netbsd} OR ${ptpd_for_openbsd} OR ${ptpd_for_macos})
    target_sources(unix PRIVATE ${bsd_sources})
elseif (${ptpd_for_qnx})
    target_sources(unix PRIVATE ${qnx_sources})
endif ()

target_include_directories(unix PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        $<TARGET_PROPERTY:core,INCLUDE_DIRECTORIES>
)
target_link_libraries(unix PRIVATE core)

ptpd_add_subdirectory_if(TESTING unit_tests)
