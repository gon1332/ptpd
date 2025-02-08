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

#ifndef PTP_TIMERS_H_
#define PTP_TIMERS_H_

#include "ptpd.h"

#ifdef HAVE_POSIX_TIMER
#define LOG_MIN_INTERVAL -7
#else
/* 62.5ms tick for interval timers = 16/sec max */
#define LOG_MIN_INTERVAL -4
#endif /* HAVE_POSIX_TIMER */

/* safeguard: a week */
#define PTPTIMER_MAX_INTERVAL 604800

/**
* \brief Structure used as a timer
 */
typedef struct {
	double interval;
	Boolean expired;
	Boolean running;
	/* hook for a generic timer object that can be assigned */
	void *data;
} IntervalTimer;

/* WARNING: when updating these timers,
 * you MUST update timerSetup() in ptp_timers.c accordingly!
 * otherwise expect a segfault if there are more timers here
 * than descriptions in ptp_timers.c
 */

enum {
  PDELAYREQ_INTERVAL_TIMER=0,/**<\brief Timer handling the PdelayReq Interval*/
  DELAYREQ_INTERVAL_TIMER,/**<\brief Timer handling the delayReq Interva*/
  SYNC_INTERVAL_TIMER,/**<\brief Timer handling Interval between master sends two Syncs messages */
  ANNOUNCE_RECEIPT_TIMER,/**<\brief Timer handling announce receipt timeout*/
  ANNOUNCE_INTERVAL_TIMER, /**<\brief Timer handling interval before master sends two announce messages*/
  /* non-spec timers */
  SYNC_RECEIPT_TIMER,
  DELAY_RECEIPT_TIMER,
  UNICAST_GRANT_TIMER, /* used to age out unicast grants (sent, received) */
  OPERATOR_MESSAGES_TIMER,  /* used to limit the operator messages */
  LEAP_SECOND_PAUSE_TIMER, /* timer used for pausing updates when leap second is imminent */
  STATUSFILE_UPDATE_TIMER, /* timer used for refreshing the status file */
  PANIC_MODE_TIMER,	   /* timer used for the duration of "panic mode" */
  PERIODIC_INFO_TIMER,	   /* timer used for dumping periodic status updates */
#ifdef PTPD_STATISTICS
  STATISTICS_UPDATE_TIMER, /* online mean / std dev updare interval (non-moving statistics) */
#endif /* PTPD_STATISTICS */
  ALARM_UPDATE_TIMER,
  MASTER_NETREFRESH_TIMER,
  CALIBRATION_DELAY_TIMER,
  CLOCK_UPDATE_TIMER,
  TIMINGDOMAIN_UPDATE_TIMER,
  PTP_MAX_TIMER
};

/* functions used by 1588 only */
void timerStop(IntervalTimer *itimer);
void timerStart(IntervalTimer * itimer, double interval);
Boolean timerExpired(IntervalTimer * itimer);
Boolean timerRunning(IntervalTimer * itimer);
Boolean timerSetup(IntervalTimer *itimers);
void timerShutdown(IntervalTimer *itimers);

#endif /* PTP_TIMERS_H_ */
