/**
 * @file impl.c
 *
 * Copyright (c) 2025 Ioannis Konstantelias
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "ptpd_config.h"
#include "impl.h"

#ifdef HAVE_POSIX_TIMER
#include "timer_posix.h"
#else
#include "timer_itimer.h"
#endif /* HAVE_POSIX_TIMERS */

const struct impl g_impl = {
#ifdef HAVE_POSIX_TIMER
	tmr_posix_create
#else
	tmr_itimer_create
#endif /* HAVE_POSIX_TIMERS */
};
