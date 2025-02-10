/**
 * @file timer_ids.h
 *
 * Copyright (c) 2025 Ioannis Konstantelias
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef TIMER_IDS_H_
#define TIMER_IDS_H_

enum
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

#endif /* TIMER_IDS_H_ */
