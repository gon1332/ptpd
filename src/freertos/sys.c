/**
 * @file sys.c
 * Implements the system functions based on the FreeRTOS API.
 *
 * Copyright (c) 2025 Ioannis Konstantelias
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "sys.h"
#include <errno.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include "ptpd_config.h"
#include "time_ops.h"
#include "utils.h"

void
sleep_for(double duration)
{
	const double min_period_sec = 1. / configTICK_RATE_HZ; /* the OS tick period */
	const double actual_period_sec = max_f(duration, min_period_sec);

	vTaskDelay(pdMS_TO_TICKS(as_ms(actual_period_sec)));
}

void
get_time(enum clock_type type, TimeInternal *time)
{
	if (type == CLOCK_MONOTONIC) {
		TimeOut_t now;
		vTaskSetTimeOutState(&now);

		/* portMAX_DELAY is the maximum value of TickType_t. */
		uint64_t ticks = (uint64_t)now.xOverflowCount * portMAX_DELAY;
		ticks += now.xTimeOnEntering;
		time->nanoseconds = (int64_t)ticks * NSEC_IN_SEC;
	} else {
#ifdef HAVE_CLOCK_GETTIME
	struct timespec ts;
	if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
		perror("clock_gettime");
		return;
	}
	ti_from_timespec(&ts, time);
#else
#pragma warning("[unimplemented] " __func__)
#endif /* _POSIX_TIMERS */
	}
}

void
set_time(const TimeInternal *time)
{
#ifdef HAVE_CLOCK_GETTIME
	struct timespec ts;
	ti_to_timespec(time, &ts);
	if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
		perror("clock_settime");
	}
#else
#pragma error("[unimplemented] " __func__)
#endif /* _POSIX_TIMERS */
}

int
set_cpu_affinity(int cpu)
{
	(void)cpu;
#pragma warning("[unimplemented] " __func__)
	return 0;
}
