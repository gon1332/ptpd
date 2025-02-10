/**
 * @file timer_collection.h
 *
 * Copyright (c) 2025 Ioannis Konstantelias
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef TIMER_COLLECTION_H_
#define TIMER_COLLECTION_H_

#include <stdbool.h>
#include "timer.h"

enum ptp_timer_type
{
	PDELAYREQ_INTERVAL_TIMER = 0, /**< Timer handling the PdelayReq Interval*/
	DELAYREQ_INTERVAL_TIMER, /**< Timer handling the delayReq Interva*/
	SYNC_INTERVAL_TIMER, /**< Timer handling Interval between master sends two Syncs messages */
	ANNOUNCE_RECEIPT_TIMER, /**< Timer handling announce receipt timeout*/
	ANNOUNCE_INTERVAL_TIMER, /**< Timer handling interval before master sends two announce
				    messages*/
	/* non-spec timers */
	SYNC_RECEIPT_TIMER,
	DELAY_RECEIPT_TIMER,
	UNICAST_GRANT_TIMER, /* used to age out unicast grants (sent, received) */
	OPERATOR_MESSAGES_TIMER, /* used to limit the operator messages */
	LEAP_SECOND_PAUSE_TIMER, /* timer used for pausing updates when leap second is imminent */
	STATUSFILE_UPDATE_TIMER, /* timer used for refreshing the status file */
	PANIC_MODE_TIMER, /* timer used for the duration of "panic mode" */
	PERIODIC_INFO_TIMER, /* timer used for dumping periodic status updates */
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

struct tmr *tmrs_get(enum ptp_timer_type type);

/**
 * @brief Creates all the PTP timers listed in enum ptp_timer_type
 * @param type Specify the timer implementation
 * @return true if all the timers have been created
 */
bool tmrs_create(enum tmr_type type);

/**
 * @return The number of timers
 */
size_t tmrs_size(void);

/**
 * @brief Destroys all the PTP timers created by tmrs_create()
 */
void tmrs_destroy(void);

#endif /* TIMER_COLLECTION_H_ */
