/**
 * @file timer.c
 * Implements the timer API.
 *
 * Copyright (c) 2025 Ioannis Konstantelias
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "timer.h"
#include <assert.h>
#include <stdlib.h>
#include "ptpd_config.h"
#include "timer_itimer.h"
#include "timer_posix.h"
#include "timer_priv.h"

struct tmr *
tmr_create(enum tmr_type type)
{
	if (type == TIMER_ITIMER) {
		return tmr_itimer_create();
	}
#ifdef HAVE_POSIX_TIMER
	if (type == TIMER_POSIX) {
		return tmr_posix_create();
	}
#endif /* HAVE_POSIX_TIMER */
	return NULL;
}

void
tmr_destroy(struct tmr *t)
{
	assert(t);
	t->destroy(t);
}

void
tmr_start(struct tmr *t, double period)
{
	assert(t);
	t->start(t, period);
}

void
tmr_stop(struct tmr *t)
{
	assert(t);
	t->stop(t);
}

bool
tmr_running(struct tmr *t)
{
	assert(t);
	return t->running(t);
}

bool
tmr_expired(struct tmr *t)
{
	assert(t);
	return t->expired(t);
}

double
tmr_interval(struct tmr *t)
{
	assert(t);
	return t->interval(t);
}
