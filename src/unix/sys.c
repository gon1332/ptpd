/**
 * @file sys.c
 * Defines the timer API.
 *
 * Copyright (c) 2025 Ioannis Konstantelias
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "sys.h"
#include <errno.h>
#include "ptpd_config.h"
#include "time_ops.h"

void
sleep_for(double duration)
{
	const TimeInternal duration_ti = ti_from_double(duration);
	struct timespec remaining;
	ti_to_timespec(&duration_ti, &remaining);

	while (nanosleep(&remaining, &remaining) == -1 && errno == EINTR) {
	}
}
