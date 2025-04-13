/**
 * Copyright (c) 2025 Ioannis Konstantelias,
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

/**
 * @file   startup.c
 * @brief  Implements startup of PTPd on FreeRTOS
 */

#include "../ptpd.h"

#include <FreeRTOS.h>
#include <task.h>
#include "timer_collection.h"

/*
 * valgrind 3.5.0 currently reports no errors (last check: 20110512)
 * valgrind 3.4.1 lacks an adjtimex handler
 *
 * to run:   sudo valgrind --show-reachable=yes --leak-check=full --track-origins=yes -- ./ptpd2 -c
 * ...
 */

/*
  to test daemon locking and startup sequence itself, try:

  function s()  { set -o pipefail ;  eval "$@" |  sed 's/^/\t/' ; echo $?;  }
  sudo killall ptpd2
  s ./ptpd2
  s sudo ./ptpd2
  s sudo ./ptpd2 -t -g
  s sudo ./ptpd2 -t -g -b eth0
  s sudo ./ptpd2 -t -g -b eth0
  ps -ef | grep ptpd2
*/

void
applyConfig(dictionary *baseConfig, RunTimeOpts *rtOpts, PtpClock *ptpClock)
{

	Boolean reloadSuccessful = TRUE;


	/* Load default config to fill in the blanks in the config file */
	RunTimeOpts tmpOpts;
	loadDefaultSettings(&tmpOpts);

	/* Check the new configuration for errors, fill in the blanks from defaults */
	if( ( rtOpts->candidateConfig = parseConfig(CFGOP_PARSE, NULL, baseConfig, &tmpOpts)) == NULL ) {
	    WARNING("Configuration has errors, reload aborted\n");
	    return;
	}

	/* Check for changes between old and new configuration */
	if(compareConfig(rtOpts->candidateConfig,rtOpts->currentConfig)) {
	    INFO("Configuration unchanged\n");
	    goto cleanup;
	}

	/*
	 * Mark which subsystems have to be restarted. Most of this will be picked up by doState()
	 * If there are errors past config correctness (such as non-existent NIC,
	 * or lock file clashes if automatic lock files used - abort the mission
	 */

	rtOpts->restartSubsystems =
	    checkSubsystemRestart(rtOpts->candidateConfig, rtOpts->currentConfig, rtOpts);

	/* If we're told to re-check lock files, do it: tmpOpts already has what rtOpts should */
	if( (rtOpts->restartSubsystems & PTPD_CHECK_LOCKS) &&
	    tmpOpts.autoLockFile && !checkOtherLocks(&tmpOpts)) {
		reloadSuccessful = FALSE;
	}

	/* If the network configuration has changed, check if the interface is OK */
	if(rtOpts->restartSubsystems & PTPD_RESTART_NETWORK) {
		INFO("Network configuration changed - checking interface(s)\n");
		if(!testInterface(tmpOpts.primaryIfaceName, &tmpOpts)) {
		    reloadSuccessful = FALSE;
		    ERROR("Error: Cannot use %s interface\n",tmpOpts.primaryIfaceName);
		}
		if(rtOpts->backupIfaceEnabled && !testInterface(tmpOpts.backupIfaceName, &tmpOpts)) {
		    rtOpts->restartSubsystems = -1;
		    ERROR("Error: Cannot use %s interface as backup\n",tmpOpts.backupIfaceName);
		}
	}
#if (defined(linux) && defined(HAVE_SCHED_H)) || defined(HAVE_SYS_CPUSET_H) || defined(__QNXNTO__)
        /* Changing the CPU affinity mask */
        if(rtOpts->restartSubsystems & PTPD_CHANGE_CPUAFFINITY) {
                NOTIFY("Applying CPU binding configuration: changing selected CPU core\n");

		if (g_impl.set_cpu_affinity(tmpOpts.cpuNumber) < 0) {
			if (tmpOpts.cpuNumber == -1) {
				ERROR("Could not unbind from CPU core %d\n", rtOpts->cpuNumber);
			} else {
				ERROR("Could bind to CPU core %d\n", tmpOpts.cpuNumber);
			}
			reloadSuccessful = FALSE;
		} else {
			if (tmpOpts.cpuNumber > -1)
				INFO("Successfully bound " PTPD_PROGNAME " to CPU core %d\n",
				     tmpOpts.cpuNumber);
			else
				INFO("Successfully unbound "PTPD_PROGNAME" from cpu core CPU core %d\n", rtOpts->cpuNumber);
		}
	}
#endif

	if(!reloadSuccessful) {
		ERROR("New configuration cannot be applied - aborting reload\n");
		rtOpts->restartSubsystems = 0;
		goto cleanup;
	}


		/**
		 * Commit changes to rtOpts and currentConfig
		 * (this should never fail as the config has already been checked if we're here)
		 * However if this DOES fail, some default has been specified out of range -
		 * this is the only situation where parse will succeed but commit not:
		 * disable quiet mode to show what went wrong, then die.
		 */
		if (rtOpts->currentConfig) {
			dictionary_del(&rtOpts->currentConfig);
		}
		if ( (rtOpts->currentConfig = parseConfig(CFGOP_PARSE_QUIET, NULL, rtOpts->candidateConfig,rtOpts)) == NULL) {
			CRITICAL("************ "PTPD_PROGNAME": parseConfig returned NULL during config commit"
				 "  - this is a BUG - report the following: \n");

			if ((rtOpts->currentConfig = parseConfig(CFGOP_PARSE, NULL, rtOpts->candidateConfig,rtOpts)) == NULL)
			    CRITICAL("*****************" PTPD_PROGNAME" shutting down **********************\n");
			/*
			 * Could be assert(), but this should be done any time this happens regardless of
			 * compile options. Anyhow, if we're here, the daemon will no doubt segfault soon anyway
			 */
			abort();
		}

	/* clean up */
	cleanup:

		dictionary_del(&rtOpts->candidateConfig);
}


/**
 * Signal handler for HUP which tells us to swap the log file
 * and reload configuration file if specified
 *
 * @param sig
 */
void
do_signal_sighup(RunTimeOpts * rtOpts, PtpClock * ptpClock)
{



	NOTIFY("SIGHUP received\n");

#ifdef RUNTIME_DEBUG
	if(rtOpts->transport == UDP_IPV4 && rtOpts->ipMode != IPMODE_UNICAST) {
		DBG("SIGHUP - running an ipv4 multicast based mode, re-sending IGMP joins\n");
		netRefreshIGMP(&ptpClock->netPath, rtOpts, ptpClock);
	}
#endif /* RUNTIME_DEBUG */


	/* if we don't have a config file specified, we're done - just reopen log files*/
	if(strlen(rtOpts->configFile) !=  0) {

	    dictionary* tmpConfig = dictionary_new(0);

	    /* Try reloading the config file */
	    NOTIFY("Reloading configuration file: %s\n",rtOpts->configFile);

            if(!loadConfigFile(&tmpConfig, rtOpts)) {

		    dictionary_del(&tmpConfig);

	    } else {
		    dictionary_merge(rtOpts->cliConfig, tmpConfig, 1, 1, "from command line");
		    applyConfig(tmpConfig, rtOpts, ptpClock);
		    dictionary_del(&tmpConfig);

	    }

	}

	/* tell the service it can perform any HUP-triggered actions */
	ptpClock->timingService.reloadRequested = TRUE;

	if(rtOpts->recordLog.logEnabled ||
	    rtOpts->eventLog.logEnabled ||
	    (rtOpts->statisticsLog.logEnabled))
		INFO("Reopening log files\n");

	restartLogging(rtOpts);

	if(rtOpts->statisticsLog.logEnabled)
		ptpClock->resetStatisticsLog = TRUE;


}

void
restartSubsystems(RunTimeOpts *rtOpts, PtpClock *ptpClock)
{
			DBG("RestartSubsystems: %d\n",rtOpts->restartSubsystems);
		    /* So far, PTP_INITIALIZING is required for both network and protocol restart */
		    if((rtOpts->restartSubsystems & PTPD_RESTART_PROTOCOL) ||
			(rtOpts->restartSubsystems & PTPD_RESTART_NETWORK)) {

			    if(rtOpts->restartSubsystems & PTPD_RESTART_NETWORK) {
				NOTIFY("Applying network configuration: going into PTP_INITIALIZING\n");
			    }

			    /* These parameters have to be passed to ptpClock before re-init */
			    ptpClock->defaultDS.clockQuality.clockClass = rtOpts->clockQuality.clockClass;
			    ptpClock->defaultDS.slaveOnly = rtOpts->slaveOnly;
			    ptpClock->disabled = rtOpts->portDisabled;

			    if(rtOpts->restartSubsystems & PTPD_RESTART_PROTOCOL) {
				INFO("Applying protocol configuration: going into %s\n",
				ptpClock->disabled ? "PTP_DISABLED" : "PTP_INITIALIZING");
			    }

			    /* Move back to primary interface only during configuration changes. */
			    ptpClock->runningBackupInterface = FALSE;
			    toState(ptpClock->disabled ? PTP_DISABLED : PTP_INITIALIZING, rtOpts, ptpClock);

		    } else {
		    /* Nothing happens here for now - SIGHUP handler does this anyway */
		    if(rtOpts->restartSubsystems & PTPD_UPDATE_DATASETS) {
				NOTIFY("Applying PTP engine configuration: updating datasets\n");
				updateDatasets(ptpClock, rtOpts);
		    }}
		    /* Nothing happens here for now - SIGHUP handler does this anyway */
		    if(rtOpts->restartSubsystems & PTPD_RESTART_LOGGING) {
				NOTIFY("Applying logging configuration: restarting logging\n");
		    }


    		if(rtOpts->restartSubsystems & PTPD_RESTART_ACLS) {
            		NOTIFY("Applying access control list configuration\n");
            		/* re-compile ACLs */
            		freeIpv4AccessList(&ptpClock->netPath.timingAcl);
            		freeIpv4AccessList(&ptpClock->netPath.managementAcl);
            		if(rtOpts->timingAclEnabled) {
                    	    ptpClock->netPath.timingAcl=createIpv4AccessList(rtOpts->timingAclPermitText,
                                rtOpts->timingAclDenyText, rtOpts->timingAclOrder);
            		}
            		if(rtOpts->managementAclEnabled) {
                    	    ptpClock->netPath.managementAcl=createIpv4AccessList(rtOpts->managementAclPermitText,
                                rtOpts->managementAclDenyText, rtOpts->managementAclOrder);
            		}
    		}

    		if(rtOpts->restartSubsystems & PTPD_RESTART_ALARMS) {
		    NOTIFY("Applying alarm configuration\n");
		    configureAlarms(ptpClock->alarms, ALRM_MAX, (void*)ptpClock);
		}

#ifdef PTPD_STATISTICS
                    /* Reinitialising the outlier filter containers */
                    if(rtOpts->restartSubsystems & PTPD_RESTART_FILTERS) {

                                NOTIFY("Applying filter configuration: re-initialising filters\n");

				freeDoubleMovingStatFilter(&ptpClock->filterMS);
				freeDoubleMovingStatFilter(&ptpClock->filterSM);

				ptpClock->oFilterMS.shutdown(&ptpClock->oFilterMS);
				ptpClock->oFilterSM.shutdown(&ptpClock->oFilterSM);

				outlierFilterSetup(&ptpClock->oFilterMS);
				outlierFilterSetup(&ptpClock->oFilterSM);

				ptpClock->oFilterMS.init(&ptpClock->oFilterMS,&rtOpts->oFilterMSConfig, "delayMS");
				ptpClock->oFilterSM.init(&ptpClock->oFilterSM,&rtOpts->oFilterSMConfig, "delaySM");


				if(rtOpts->filterMSOpts.enabled) {
					ptpClock->filterMS = createDoubleMovingStatFilter(&rtOpts->filterMSOpts,"delayMS");
				}

				if(rtOpts->filterSMOpts.enabled) {
					ptpClock->filterSM = createDoubleMovingStatFilter(&rtOpts->filterSMOpts, "delaySM");
				}

		    }
#endif /* PTPD_STATISTICS */


	    ptpClock->timingService.reloadRequested = TRUE;

            if(rtOpts->restartSubsystems & PTPD_RESTART_NTPENGINE && timingDomain.serviceCount > 1) {
		ptpClock->ntpControl.timingService.shutdown(&ptpClock->ntpControl.timingService);
	    }

	    if((rtOpts->restartSubsystems & PTPD_RESTART_NTPENGINE) ||
        	(rtOpts->restartSubsystems & PTPD_RESTART_NTPCONFIG)) {
        	ntpSetup(rtOpts, ptpClock);
    	    }
		if((rtOpts->restartSubsystems & PTPD_RESTART_NTPENGINE) && rtOpts->ntpOptions.enableEngine) {
		    timingServiceSetup(&ptpClock->ntpControl.timingService);
		    ptpClock->ntpControl.timingService.init(&ptpClock->ntpControl.timingService);
		}

		ptpClock->timingService.dataSet.priority1 = rtOpts->preferNTP;

		timingDomain.electionDelay = rtOpts->electionDelay;
		if(timingDomain.electionLeft > timingDomain.electionDelay) {
			timingDomain.electionLeft = timingDomain.electionDelay;
		}

		timingDomain.services[0]->holdTime = rtOpts->ntpOptions.failoverTimeout;

		if(timingDomain.services[0]->holdTimeLeft >
			timingDomain.services[0]->holdTime) {
			timingDomain.services[0]->holdTimeLeft =
			rtOpts->ntpOptions.failoverTimeout;
		}

		ptpClock->timingService.timeout = rtOpts->idleTimeout;

		    /* Update PI servo parameters */
		    setupPIservo(&ptpClock->servo, rtOpts);
		    /* Config changes don't require subsystem restarts - acknowledge it */
		    if(rtOpts->restartSubsystems == PTPD_RESTART_NONE) {
				NOTIFY("Applying configuration\n");
		    }

		    if(rtOpts->restartSubsystems != -1)
			    rtOpts->restartSubsystems = 0;

}

#ifdef RUNTIME_DEBUG
/* These functions are useful to temporarily enable Debug around parts of code, similar to bash's "set -x" */
void enable_runtime_debug(void )
{
	extern RunTimeOpts rtOpts;
	
	rtOpts.debug_level = max(LOG_DEBUGV, rtOpts.debug_level);
}

void disable_runtime_debug(void )
{
	extern RunTimeOpts rtOpts;
	
	rtOpts.debug_level = LOG_INFO;
}
#endif

void
checkSignals(RunTimeOpts * rtOpts, PtpClock * ptpClock)
{
}

void
ptpdShutdown(PtpClock * ptpClock)
{

	extern RunTimeOpts rtOpts;
	
	/*
         * go into DISABLED state so the FSM can call any PTP-specific shutdown actions,
	 * such as canceling unicast transmission
         */
	toState(PTP_DISABLED, &rtOpts, ptpClock);
	/* process any outstanding events before exit */
	updateAlarms(ptpClock->alarms, ALRM_MAX);
	netShutdown(&ptpClock->netPath);
	vPortFree(ptpClock->foreign);

	/* free management and signaling messages, they can have dynamic memory allocated */
	if(ptpClock->msgTmpHeader.messageType == MANAGEMENT)
		freeManagementTLV(&ptpClock->msgTmp.manage);
	freeManagementTLV(&ptpClock->outgoingManageTmp);
	if(ptpClock->msgTmpHeader.messageType == SIGNALING)
		freeSignalingTLV(&ptpClock->msgTmp.signaling);
	freeSignalingTLV(&ptpClock->outgoingSignalingTmp);

#ifdef PTPD_SNMP
	snmpShutdown();
#endif /* PTPD_SNMP */

#ifndef PTPD_STATISTICS
	/* Not running statistics code - write observed drift to driftfile if enabled, inform user */
	if(ptpClock->defaultDS.slaveOnly && !ptpClock->servo.runningMaxOutput)
		saveDrift(ptpClock, &rtOpts, FALSE);
#else
	ptpClock->oFilterMS.shutdown(&ptpClock->oFilterMS);
	ptpClock->oFilterSM.shutdown(&ptpClock->oFilterSM);
        freeDoubleMovingStatFilter(&ptpClock->filterMS);
        freeDoubleMovingStatFilter(&ptpClock->filterSM);

	/* We are running statistics code - save drift on exit only if we're not monitoring servo stability */
	if(!rtOpts.servoStabilityDetection && !ptpClock->servo.runningMaxOutput)
		saveDrift(ptpClock, &rtOpts, FALSE);
#endif /* PTPD_STATISTICS */

	if (rtOpts.currentConfig != NULL)
		dictionary_del(&rtOpts.currentConfig);
	if(rtOpts.cliConfig != NULL)
		dictionary_del(&rtOpts.cliConfig);

	tmrs_destroy();

	vPortFree(ptpClock);
	ptpClock = NULL;

	extern PtpClock* G_ptpClock;
	G_ptpClock = NULL;

	if(rtOpts.statusLog.logEnabled) {
		/* close and remove the status file */
		if(rtOpts.statusLog.logFP != NULL) {
			fclose(rtOpts.statusLog.logFP);
			rtOpts.statusLog.logFP = NULL;
		}
		unlink(rtOpts.statusLog.logPath);
	}

	stopLogging(&rtOpts);
}

void dump_command_line_parameters(int argc, char **argv)
{

	int i = 0;
	char sbuf[1000];
	char *st = sbuf;
	int len = 0;
	
	*st = '\0';
	for(i=0; i < argc; i++){
		if(strcmp(argv[i],"") == 0)
		    continue;
		len += snprintf(sbuf + len,
					     sizeof(sbuf) - len,
					     "%s ", argv[i]);
	}
	INFO("Starting %s daemon with parameters:      %s\n", PTPD_PROGNAME, sbuf);
}



PtpClock *
ptpdStartup(int argc, char **argv, Integer16 * ret, RunTimeOpts * rtOpts)
{
	/**
	 * If a required setting, such as interface name, or a setting
	 * requiring a range check is to be set via getopts_long,
	 * the respective currentConfig dictionary entry should be set,
	 * instead of just setting the rtOpts field.
	 *
	 * Config parameter evaluation priority order:
	 * 	1. Any dictionary keys set in the getopt_long loop
	 * 	2. CLI long section:key type options
	 * 	3. Any built-in config templates
	 *	4. Any templates loaded from template file
	 * 	5. Config file (parsed last), merged with 2. and 3 - will be overwritten by CLI options
	 * 	6. Defaults and any rtOpts fields set in the getopt_long loop
	**/

	/**
	 * Load defaults. Any options set here and further inside loadCommandLineOptions()
	 * by setting rtOpts fields, will be considered the defaults
	 * for config file and section:key long options.
	 */
	loadDefaultSettings(rtOpts);
	/* initialise the config dictionary */
	rtOpts->candidateConfig = dictionary_new(0);
	rtOpts->cliConfig = dictionary_new(0);

	/* parse all long section:key options and clean up argv for getopt */
	loadCommandLineKeys(rtOpts->cliConfig,argc,argv);
	/* parse the normal short and long options, exit on error */
	if (!loadCommandLineOptions(rtOpts, rtOpts->cliConfig, argc, argv, ret)) {
	    goto fail;
	}

	/* Display startup info and argv if not called with -? or -H */
		NOTIFY("%s version %s starting\n",USER_DESCRIPTION, USER_VERSION);
		dump_command_line_parameters(argc, argv);

		/* Have we got a config file? */
		if (strlen(rtOpts->configFile) > 0) {
			/* config file settings overwrite all others, except for empty strings */
			INFO("Loading configuration file: %s\n", rtOpts->configFile);
			if (loadConfigFile(&rtOpts->candidateConfig, rtOpts)) {
				dictionary_merge(rtOpts->cliConfig, rtOpts->candidateConfig, 1, 1,
						 "from command line");
			} else {
				*ret = 1;
				dictionary_merge(rtOpts->cliConfig, rtOpts->candidateConfig, 1, 1,
						 "from command line");
				goto configcheck;
			}
	} else {
		dictionary_merge(rtOpts->cliConfig, rtOpts->candidateConfig, 1, 1, "from command line");
	}
	/**
	 * This is where the final checking  of the candidate settings container happens.
	 * A dictionary is returned with only the known options, explicitly set to defaults
	 * if not present. NULL is returned on any config error - parameters missing, out of range,
	 * etc. The getopt loop in loadCommandLineOptions() only sets keys verified here.
	 */
	if( ( rtOpts->currentConfig = parseConfig(CFGOP_PARSE, NULL, rtOpts->candidateConfig,rtOpts)) == NULL ) {
	    *ret = 1;
	    dictionary_del(&rtOpts->candidateConfig);
	    goto configcheck;
	}

	/* we don't need the candidate config any more */
	dictionary_del(&rtOpts->candidateConfig);

	/* Check network before going into background */
	if(!testInterface(rtOpts->primaryIfaceName, rtOpts)) {
	    ERROR("Error: Cannot use %s interface\n",rtOpts->primaryIfaceName);
	    *ret = 1;
	    goto configcheck;
	}
	if(rtOpts->backupIfaceEnabled && !testInterface(rtOpts->backupIfaceName, rtOpts)) {
	    ERROR("Error: Cannot use %s interface as backup\n",rtOpts->backupIfaceName);
	    *ret = 1;
	    goto configcheck;
	}


configcheck:
	/* Previous errors - exit */
	if(*ret !=0)
		goto fail;

	/* Manage log files: stats, log, status and quality file */
	restartLogging(rtOpts);

	/* Allocate memory after we're done with other checks but before going into daemon */
	PtpClock *ptp_clock = pvPortCalloc(1, sizeof *ptp_clock);
	if (!ptp_clock) {
		PERROR("Error: Failed to allocate memory for protocol engine data");
		*ret = 2;
		goto fail;
	}
	DBG("allocated %d bytes for protocol engine data\n", (int)sizeof(PtpClock));

	ptp_clock->foreign = pvPortCalloc(rtOpts->max_foreign_records, sizeof *ptp_clock->foreign);
	if (!ptp_clock->foreign) {
		PERROR("failed to allocate memory for foreign "
		       "master data");
		*ret = 2;
		vPortFree(ptp_clock);
		goto fail;
	}
	DBG("allocated %d bytes for foreign master data\n",
	    (int)(rtOpts->max_foreign_records * sizeof(ForeignMasterRecord)));

	if(rtOpts->statisticsLog.logEnabled)
		ptp_clock->resetStatisticsLog = TRUE;

	/* Init to 0 net buffer */
	memset(ptp_clock->msgIbuf, 0, PACKET_SIZE);
	memset(ptp_clock->msgObuf, 0, PACKET_SIZE);

	/* Init outgoing management message */
	ptp_clock->outgoingManageTmp.tlv = NULL;

	/*  DAEMON */
	rtOpts->nonDaemon = true;

	/* set up timers */
	if (!tmrs_create(&g_impl)) {
		PERROR("failed to set up event timers");
		*ret = 2;
		vPortFree(ptp_clock->foreign);
		vPortFree(ptp_clock);
		goto fail;
	}

	ptp_clock->rtOpts = rtOpts;

	/* init alarms */
	initAlarms(ptp_clock->alarms, ALRM_MAX, ptp_clock);
	configureAlarms(ptp_clock->alarms, ALRM_MAX, ptp_clock);
	ptp_clock->alarmDelay = rtOpts->alarmInitialDelay;
	/* we're delaying alarm processing - disable alarms for now */
	if (ptp_clock->alarmDelay) {
		enableAlarms(ptp_clock->alarms, ALRM_MAX, FALSE);
	}

#if defined PTPD_SNMP
	/* Start SNMP subsystem */
	if (rtOpts->snmpEnabled)
		snmpInit(rtOpts, ptpClock);
#endif



	NOTICE(USER_DESCRIPTION" started successfully on %s using \"%s\" preset (PID %d)\n",
			    rtOpts->ifaceName,
			    (getPtpPreset(rtOpts->selectedPreset,rtOpts)).presetName,
			    getpid());
	ptp_clock->resetStatisticsLog = TRUE;

#ifdef PTPD_STATISTICS

	outlierFilterSetup(&ptpClock->oFilterMS);
	outlierFilterSetup(&ptpClock->oFilterSM);

	ptpClock->oFilterMS.init(&ptpClock->oFilterMS,&rtOpts->oFilterMSConfig, "delayMS");
	ptpClock->oFilterSM.init(&ptpClock->oFilterSM,&rtOpts->oFilterSMConfig, "delaySM");


	if(rtOpts->filterMSOpts.enabled) {
		ptpClock->filterMS = createDoubleMovingStatFilter(&rtOpts->filterMSOpts,"delayMS");
	}

	if(rtOpts->filterSMOpts.enabled) {
		ptpClock->filterSM = createDoubleMovingStatFilter(&rtOpts->filterSMOpts, "delaySM");
	}

#endif

	ptp_clock->netPath.generalSock = -1;
	ptp_clock->netPath.eventSock = -1;

	*ret = 0;

	return ptp_clock;

fail:
	dictionary_del(&rtOpts->cliConfig);
	dictionary_del(&rtOpts->candidateConfig);
	dictionary_del(&rtOpts->currentConfig);
	return 0;
}

void
ntpSetup (RunTimeOpts *rtOpts, PtpClock *ptpClock)
{
	TimingService *ts = &ptpClock->ntpControl.timingService;



	if (rtOpts->ntpOptions.enableEngine) {
	    timingDomain.services[1] = ts;
	    strncpy(ts->id, "NTP0", TIMINGSERVICE_MAX_DESC);
	    ts->dataSet.priority1 = 0;
	    ts->dataSet.type = TIMINGSERVICE_NTP;
	    ts->config = &rtOpts->ntpOptions;
	    ts->controller = &ptpClock->ntpControl;
	    /* for now, NTP is considered always active, so will never go idle */
	    ts->timeout = 60;
	    ts->updateInterval = rtOpts->ntpOptions.checkInterval;
	    timingDomain.serviceCount = 2;
	} else {
	    timingDomain.serviceCount = 1;
	    timingDomain.services[1] = NULL;
	    if(timingDomain.best == ts || timingDomain.current == ts || timingDomain.preferred == ts) {
		timingDomain.best = timingDomain.current = timingDomain.preferred = NULL;
	    }
	}
}

struct ptpd_cfg
{
	void (*log)(uint8_t p_priority, const char *p_fmt, va_list p_ap);
	int argc;
	char **argv;
};

PtpClock *G_ptpClock = NULL;
RunTimeOpts rtOpts; /* statically allocated run-time configuration data */
TimingDomain timingDomain;
Boolean startupInProgress;
struct ptpd_cfg g_config;

/*
 * PTPd task on FreeRTOS
 */
#define PTPD_TASK_STACK_SIZE (4096) /* PTPd task stack size */
#define PTPD_TASK_PRIORITY   (2) /* PTPd task priority   */

/* PTPd task Memory */
static StaticTask_t g_ptpd_task_tcb;
static StackType_t g_ptpd_task_stack[PTPD_TASK_STACK_SIZE];

static void ptpd_task(void *data);

/**
 * @param cfg
 * @return true if PTPd task has been created
 */
void
ptpd_start(int argc, char **argv)
{
	puts("ptpd_start to create task");
	g_config.argc = argc;
	g_config.argv = argv;
	TaskHandle_t task = xTaskCreateStatic(ptpd_task, "PTPd", PTPD_TASK_STACK_SIZE, NULL,
					      PTPD_TASK_PRIORITY, g_ptpd_task_stack,
					      &g_ptpd_task_tcb);
	if (task == NULL) {
		puts("ptpd_start failed to create task");
		return;
	}

	vTaskStartScheduler();
	puts("ptpd_start failed to start scheduler");
}

void
ptpd_task(void *data)
{
	(void)data;

	startupInProgress = TRUE;

	memset(&timingDomain, 0, sizeof(timingDomain));
	timingDomainSetup(&timingDomain);
	timingDomain.electionLeft = 10;

	/* Create PTPd clock */
	int16_t ret;
	G_ptpClock = ptpdStartup(g_config.argc, g_config.argv, &ret, &rtOpts);
	if (G_ptpClock == NULL) {
		ERROR("Failed to create a PTP clock (ret: %d)\n", ret);
		goto fail;
	}

	timingDomain.electionDelay = rtOpts.electionDelay;

	/* Configure PTP TimeService */
	timingDomain.services[0] = &G_ptpClock->timingService;
	TimingService *ts = timingDomain.services[0];
	strncpy(ts->id, "PTP0", TIMINGSERVICE_MAX_DESC);
	ts->dataSet.priority1 = rtOpts.preferNTP;
	ts->dataSet.type = TIMINGSERVICE_PTP;
	ts->config = &rtOpts;
	ts->controller = G_ptpClock;
	ts->timeout = rtOpts.idleTimeout;
	ts->updateInterval = 1;
	ts->holdTime = rtOpts.ntpOptions.failoverTimeout;
	timingDomain.serviceCount = 1;
	timingDomain.services[1] = NULL;

	timingDomain.init(&timingDomain);
	timingDomain.updateInterval = 1;

	startupInProgress = FALSE;

	/* Start protocol-loop */
	protocol(&rtOpts, G_ptpClock);

fail:
	NOTIFY("Self shutdown\n");
	vTaskDelete(NULL);
}
