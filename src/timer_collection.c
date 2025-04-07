/**
 * @file timer_collection.c
 *
 * Copyright (c) 2025 Ioannis Konstantelias
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <assert.h>
#include <stdio.h>
#include "timer_collection.h"
#include "timer.h"

static const char *
to_str(enum ptp_timer_type type)
{
	assert(type >= 0 && type < PTP_MAX_TIMER);
	switch (type) {
	case PDELAYREQ_INTERVAL_TIMER:
		return "PDELAYREQ_INTERVAL_TIMER";
	case DELAYREQ_INTERVAL_TIMER:
		return "DELAYREQ_INTERVAL_TIMER";
	case SYNC_INTERVAL_TIMER:
		return "SYNC_INTERVAL_TIMER";
	case ANNOUNCE_RECEIPT_TIMER:
		return "ANNOUNCE_RECEIPT_TIMER";
	case ANNOUNCE_INTERVAL_TIMER:
		return "ANNOUNCE_INTERVAL_TIMER";
	case SYNC_RECEIPT_TIMER:
		return "SYNC_RECEIPT_TIMER";
	case DELAY_RECEIPT_TIMER:
		return "DELAY_RECEIPT_TIMER";
	case UNICAST_GRANT_TIMER:
		return "UNICAST_GRANT_TIMER";
	case OPERATOR_MESSAGES_TIMER:
		return "OPERATOR_MESSAGES_TIMER";
	case LEAP_SECOND_PAUSE_TIMER:
		return "LEAP_SECOND_PAUSE_TIMER";
	case STATUSFILE_UPDATE_TIMER:
		return "STATUSFILE_UPDATE_TIMER";
	case PANIC_MODE_TIMER:
		return "PANIC_MODE_TIMER";
	case PERIODIC_INFO_TIMER:
		return "PERIODIC_INFO_TIMER";
#ifdef PTPD_STATISTICS
	case STATISTICS_UPDATE_TIMER:
		return "STATISTICS_UPDATE_TIMER";
#endif /* PTPD_STATISTICS */
	case ALARM_UPDATE_TIMER:
		return "ALARM_UPDATE_TIMER";
	case MASTER_NETREFRESH_TIMER:
		return "MASTER_NETREFRESH_TIMER";
	case CALIBRATION_DELAY_TIMER:
		return "CALIBRATION_DELAY_TIMER";
	case CLOCK_UPDATE_TIMER:
		return "CLOCK_UPDATE_TIMER";
	case TIMINGDOMAIN_UPDATE_TIMER:
		return "TIMINGDOMAIN_UPDATE_TIMER";
	default:
		return "UNKNOWN_TIMER";
	}
}

static struct tmr *lut[PTP_MAX_TIMER];

/**
 * @return true if all timers have been created successfully
 */
static bool
tmrs_emplace(enum ptp_timer_type which, const struct impl *impl)
{
	lut[which] = tmr_create(impl);
	return lut[which] != NULL;
}

static void
destroy(unsigned up_to)
{
	for (unsigned i = 0; i < up_to; ++i) {
		if (tmrs_get(i)) {
			tmr_destroy(tmrs_get(i));
		}
	}
}

struct tmr *
tmrs_get(enum ptp_timer_type which)
{
	assert(which >= 0 && which < PTP_MAX_TIMER);
	return lut[which];
}

bool
tmrs_create(const struct impl *impl)
{
	unsigned i;
	for (i = 0; i < PTP_MAX_TIMER; i++) {
		if (!tmrs_emplace(i, impl)) {
			goto fail;
		}
	}
	return true;
fail:
	destroy(i);
	return false;
}

size_t
tmrs_size(void)
{
	return sizeof(lut) / sizeof(lut[0]);
}

void
tmrs_destroy()
{
	destroy(PTP_MAX_TIMER);
}
