set(PTPD2_SRCS
        arith.c
        bmc.c
        dep/alarms.c
        dep/eventtimer.c
        dep/iniparser/dictionary.c
        dep/iniparser/iniparser.c
        dep/ipv4_acl.c
        dep/configdefaults.c
        dep/daemonconfig.c
        dep/msg.c
        dep/net.c
        dep/ntpengine/ntp_isc_md5.c
        dep/ntpengine/ntpdcontrol.c
        dep/servo.c
        dep/startup.c
        dep/sys.c
        display.c
        protocol.c
        ptp_timers.c
        ptpd.c
        management.c
        signaling.c
        timingdomain.c
)

add_executable(ptpd ${PTPD2_SRCS})

target_include_directories(ptpd PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/def
        ${CMAKE_CURRENT_SOURCE_DIR}/dep
)

if (SNMP)
    target_sources(ptpd PUBLIC dep/snmp.c)
endif ()

if (STATISTICS)
    target_sources(ptpd PUBLIC
            dep/statistics.c
            dep/outlierfilter.c
    )
endif ()

if (POSIX_TIMERS)
    target_sources(ptpd PUBLIC dep/eventtimer_posix.c)
else ()
    target_sources(ptpd PUBLIC dep/eventtimer_itimer.c)
endif ()

target_compile_definitions(ptpd PUBLIC
        DATADIR="${CMAKE_CURRENT_SOURCE_DIR}"
        $<$<BOOL:POSIX_TIMERS>:PTPD_PTIMERS>
)

target_link_libraries(ptpd PUBLIC
        m
)