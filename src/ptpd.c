/*-
 * Copyright (c) 2015-2024 Ioannis Konstantelias,
 * Copyright (c) 2012-2015 Wojciech Owczarek,
 * Copyright (c) 2011-2012 George V. Neville-Neil,
 *                         Steven Kreuzer,
 *                         Martin Burnicki,
 *                         Jan Breuer,
 *                         Gael Mace,
 *                         Alexandre Van Kempen,
 *                         Inaqui Delgado,
 *                         Rick Ratzel,
 *                         National Instruments.
 * Copyright (c) 2009-2010 George V. Neville-Neil,
 *                         Steven Kreuzer,
 *                         Martin Burnicki,
 *                         Jan Breuer,
 *                         Gael Mace,
 *                         Alexandre Van Kempen
 * Copyright (c) 2005-2008 Kendall Correll, Aidan Williams
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

/**
 * @file   ptpd.c
 * @date   Wed Jun 23 10:13:38 2010
 *
 * @brief  The main() function for the PTP daemon
 *
 * This file contains very little code, as should be obvious,
 * and only serves to tie together the rest of the daemon.
 * All of the default options are set here, but command line
 * arguments and configuration file is processed in the
 * ptpdStartup() routine called
 * below.
 */

#include "ptpd.h"

RunTimeOpts rtOpts;			/* statically allocated run-time
					 * configuration data */

Boolean startupInProgress;

/*
 * Global variable with the main PTP port. This is used to show the current state in DBG()/message()
 * without having to pass the pointer everytime.
 *
 * if ptpd is extended to handle multiple ports (eg, to instantiate a Boundary Clock),
 * then DBG()/message() needs a per-port pointer argument
 */
PtpClock *G_ptpClock = NULL;

TimingDomain timingDomain;

int
main(int argc, char **argv)
{
	PtpClock *ptpClock;
	Integer16 ret;
	TimingService *ts;

	startupInProgress = TRUE;

	memset(&timingDomain, 0, sizeof(timingDomain));
	timingDomainSetup(&timingDomain);

	timingDomain.electionLeft = 10;

	/* Initialize run time options with command line arguments */
	if (!(ptpClock = ptpdStartup(argc, argv, &ret, &rtOpts))) {
		if (ret != 0 && !rtOpts.checkConfigOnly)
			ERROR(USER_DESCRIPTION" startup failed\n");
		return ret;
	}

	timingDomain.electionDelay = rtOpts.electionDelay;

	/* configure PTP TimeService */

	timingDomain.services[0] = &ptpClock->timingService;
	ts = timingDomain.services[0];
	strncpy(ts->id, "PTP0", TIMINGSERVICE_MAX_DESC);
	ts->dataSet.priority1 = rtOpts.preferNTP;
	ts->dataSet.type = TIMINGSERVICE_PTP;
	ts->config = &rtOpts;
	ts->controller = ptpClock;
	ts->timeout = rtOpts.idleTimeout;
	ts->updateInterval = 1;
	ts->holdTime = rtOpts.ntpOptions.failoverTimeout;
	timingDomain.serviceCount = 1;

	if (rtOpts.ntpOptions.enableEngine) {
		ntpSetup(&rtOpts, ptpClock);
	} else {
	    timingDomain.serviceCount = 1;
	    timingDomain.services[1] = NULL;
	}

	timingDomain.init(&timingDomain);
	timingDomain.updateInterval = 1;

	startupInProgress = FALSE;

	/* global variable for message(), please see comment on top of this file */
	G_ptpClock = ptpClock;

	/* do the protocol engine */
	protocol(&rtOpts, ptpClock);
	/* forever loop.. */

	/* this also calls ptpd shutdown */
	timingDomain.shutdown(&timingDomain);

	NOTIFY("Self shutdown\n");

	return 1;
}
