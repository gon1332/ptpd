# SPDX-License-Identifier: BSD-2-Clause

add_library(time_ops OBJECT time_ops.c)
target_include_directories(time_ops PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(ptpd2
        arith.c
        bmc.c
        display.c
        management.c
        protocol.c
        ptp_timers.c
        ptpd.c
        signaling.c
        time_ops.c
        timingdomain.c
        dep/iniparser/dictionary.c
        dep/iniparser/iniparser.c
        dep/ntpengine/ntp_isc_md5.c
        dep/ntpengine/ntpdcontrol.c
        dep/alarms.c
        dep/configdefaults.c
        dep/daemonconfig.c
        dep/eventtimer.c
        dep/eventtimer_posix.c
        dep/ipv4_acl.c
        dep/msg.c
        dep/net.c
        dep/servo.c
        dep/snmp.c
        dep/startup.c
        dep/sys.c
        $<$<BOOL:${STATISTICS}>:
            dep/statistics.c
            dep/outlierfilter.c
        >
)
target_include_directories(ptpd2 PUBLIC
        .
        def
        dep
)
target_link_libraries(ptpd2 PUBLIC
        m
        netsnmp
        netsnmpagent
        netsnmpmibs
)
target_compile_options(ptpd2 PUBLIC
        -Wall
)
target_compile_definitions(ptpd2 PUBLIC
        DATADIR=""
        PTPD_PTIMERS
        $<$<BOOL:${STATISTICS}>:PTPD_STATISTICS>
)

add_subdirectory(unit_tests)
