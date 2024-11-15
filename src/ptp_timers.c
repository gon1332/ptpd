/*-
 * Copyright (c) 2015      Wojciech Owczarek,
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

/**
 * @file   timer.c
 * @date   Wed Oct 1 00:41:26 2014
 *
 * @brief  PTP timer handler code
 *
 * Glue code providing PTPd with event timers.
 * This code can be re-written to make use of any timer
 * implementation. Provided is an EventTimer which uses
 * a fixed tick interval timer, or POSIX timers, depending
 * on what is available. So the options are:
 * - write another EventTimer implementation,
 *   using the existing framework,
 * - write something different
 */

#include "ptpd.h"

void
timerStop(IntervalTimer * itimer)
{
	if (itimer == NULL)
		return;
	EventTimer *timer = (EventTimer *)(itimer->data);

	timer->stop(timer);
}

void
timerStart(IntervalTimer * itimer, double interval)
{
	if (itimer == NULL)
		return;

        if(interval > PTPTIMER_MAX_INTERVAL) {
            interval = PTPTIMER_MAX_INTERVAL;
        }

	itimer->interval = interval;
	EventTimer* timer = (EventTimer *)(itimer->data);

	timer->start(timer, interval);
}

Boolean
timerExpired(IntervalTimer * itimer)
{

	if (itimer == NULL)
		return FALSE;

	EventTimer *timer = (EventTimer *)(itimer->data);

	return timer->isExpired(timer);
}

Boolean
timerRunning(IntervalTimer * itimer)
{

	if (itimer==NULL)
		return FALSE;

	EventTimer *timer = (EventTimer *)(itimer->data);

	return timer->isRunning(timer);
}

Boolean timerSetup(IntervalTimer *itimers)
{

    Boolean ret = TRUE;

/* WARNING: these descriptions MUST be in the same order,
 * and in the same number as the enum in ptp_timers.h
 */

    static const char* timerDesc[PTP_MAX_TIMER] = {
  "PDELAYREQ_INTERVAL",
  "DELAYREQ_INTERVAL",
  "SYNC_INTERVAL",
  "ANNOUNCE_RECEIPT",
  "ANNOUNCE_INTERVAL",
  "SYNC_RECEIPT",
  "DELAY_RECEIPT",
  "UNICAST_GRANT",
  "OPERATOR_MESSAGES",
  "LEAP_SECOND_PAUSE",
  "STATUSFILE_UPDATE",
  "PANIC_MODE",
  "PERIODIC_INFO_TIMER",
#ifdef PTPD_STATISTICS
  "STATISTICS_UPDATE",
#endif /* PTPD_STATISTICS */
  "ALARM_UPDATE",
  "MASTER_NETREFRESH",
  "CALIBRATION_DELAY",
  "CLOCK_UPDATE",
  "TIMINGDOMAIN_UPDATE"
    };

    int i = 0;

    startEventTimers();

    for(i=0; i<PTP_MAX_TIMER; i++) {

	itimers[i].data = NULL;
	itimers[i].data = (void *)(createEventTimer(timerDesc[i]));
	if(itimers[i].data == NULL) {
	    ret = FALSE;
	}
    }

    return ret;

}

void timerShutdown(IntervalTimer *itimers)
{

    int i = 0;
    EventTimer *timer = NULL;


    for(i=0; i<PTP_MAX_TIMER; i++) {
	    timer = (EventTimer*)(itimers[i].data);
	    freeEventTimer(&timer);
    }

    shutdownEventTimers();

}
