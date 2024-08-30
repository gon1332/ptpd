set(STATISTICS_SOURCES dep/outlierfilter.c dep/statistics.c)

add_executable(ptpd
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
        $<$<BOOL:${SNMP}>:dep/snmp.c>
        $<$<BOOL:${STATISTICS}>:${STATISTICS_SOURCES}>
        $<IF:$<BOOL:${POSIX_TIMERS}>,
            dep/eventtimer_posix.c, # true
            dep/eventtimer_itimer.c # false
        >
)
target_include_directories(ptpd PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/def
        ${CMAKE_CURRENT_SOURCE_DIR}/dep
)
target_compile_definitions(ptpd PUBLIC
        DATADIR="${CMAKE_CURRENT_SOURCE_DIR}"
        PTPD_UNICAST_MAX=16 # Use the least number supported as we do not care about unicast
        PTPD_DISABLE_SOTIMESTAMPING # Suggested to keep it disabled
        $<$<BOOL:${POSIX_TIMERS}>:PTPD_PTIMERS>
        $<$<BOOL:${RUNTIME_DEBUG}>:RUNTIME_DEBUG>
        $<$<BOOL:${SLAVE_ONLY}>:PTPD_SLAVE_ONLY>
        $<$<BOOL:${SNMP}>:PTPD_SNMP>
        $<$<BOOL:${STATISTICS}>:PTPD_STATISTICS>
)
target_compile_options(ptpd PUBLIC
        -Wall
)
target_link_libraries(ptpd PUBLIC
        m
        rt
        $<$<BOOL:${SNMP}>:${NETSNMPLIBS}>
        $<$<BOOL:${SNMP}>:${NETSNMPMIBS}>
        $<$<BOOL:${SNMP}>:${NETSNMP}>
)
