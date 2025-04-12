# SPDX-License-Identifier: BSD-2-Clause

add_library(core OBJECT
        arith.c
        bmc.c
        display.c
        dep/alarms.c
        dep/configdefaults.c
        dep/daemonconfig.c
        dep/iniparser/dictionary.c
        dep/iniparser/iniparser.c
        dep/ipv4_acl.c
        dep/msg.c
        dep/net.c
        dep/ntpengine/ntp_isc_md5.c
        dep/ntpengine/ntpdcontrol.c
        dep/servo.c
        $<$<BOOL:${SNMP}>:dep/snmp.c>
        $<$<BOOL:${STATISTICS}>:
        dep/statistics.c
        dep/outlierfilter.c
        >
        management.c
        protocol.c
        signaling.c
        time_ops.c
        timer.c
        timer_collection.c
        timingdomain.c
)
target_include_directories(core
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib
)
target_compile_definitions(core PUBLIC
        DATADIR=""
)

add_subdirectory(freertos)
add_subdirectory(unix)

add_executable(ptpd2
        ptpd.c
        dep/sys.c
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
        $<$<BOOL:${STATISTICS}>:PTPD_STATISTICS>
        $<$<BOOL:${SNMP}>:PTPD_SNMP>
)

target_compile_features(ptpd2 PUBLIC c_std_99)

add_executable(ptpd2_freertos
        ptpd.c
        dep/sys.c
)
target_include_directories(ptpd2_freertos PUBLIC
        .
        def
        dep
)
target_link_libraries(ptpd2_freertos PUBLIC
        core
        m
        "$<$<BOOL:${SNMP}>:netsnmp;netsnmpagent;netsnmpmibs>"
        freertos
)
target_compile_options(ptpd2_freertos PUBLIC
        -Wall
        -gdwarf-4
        "$<$<BOOL:${SNMP}>:`net-snmp-config --base-cflags`>"
)
target_compile_definitions(ptpd2_freertos PUBLIC
        $<$<BOOL:${STATISTICS}>:PTPD_STATISTICS>
        $<$<BOOL:${SNMP}>:PTPD_SNMP>
)

target_compile_features(ptpd2_freertos PUBLIC c_std_99)

if (${TESTING})
    add_subdirectory(unit_tests)
endif ()
