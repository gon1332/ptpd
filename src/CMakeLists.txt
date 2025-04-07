# SPDX-License-Identifier: BSD-2-Clause

add_library(core OBJECT
        time_ops.c
        timer.c
        timer_collection.c
)
target_include_directories(core
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

add_subdirectory(unix)

add_executable(ptpd2
        arith.c
        bmc.c
        display.c
        management.c
        protocol.c
        ptpd.c
        signaling.c
        timingdomain.c
        dep/iniparser/dictionary.c
        dep/iniparser/iniparser.c
        dep/ntpengine/ntp_isc_md5.c
        dep/ntpengine/ntpdcontrol.c
        dep/alarms.c
        dep/configdefaults.c
        dep/daemonconfig.c
        dep/ipv4_acl.c
        dep/msg.c
        dep/net.c
        dep/servo.c
        dep/startup.c
        dep/sys.c
        $<$<BOOL:${SNMP}>:dep/snmp.c>
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
        core
        m
        "$<$<BOOL:${SNMP}>:netsnmp;netsnmpagent;netsnmpmibs>"
        unix
)
target_compile_options(ptpd2 PUBLIC
        -Wall
        -gdwarf-4
        "$<$<BOOL:${SNMP}>:`net-snmp-config --base-cflags`>"
)
target_compile_definitions(ptpd2 PUBLIC
        DATADIR=""
        $<$<BOOL:${STATISTICS}>:PTPD_STATISTICS>
        $<$<BOOL:${SNMP}>:PTPD_SNMP>
)

target_compile_features(ptpd2 PUBLIC c_std_99)


if (${TESTING})
    add_subdirectory(unit_tests)
endif ()
