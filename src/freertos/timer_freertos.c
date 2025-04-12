/**
 * @file timer_freertos.c
 * Implements the timer API using FreeRTOS timers.
 *
 * Copyright (c) 2025 Ioannis Konstantelias
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "timer_freertos.h"
#include "timer_priv.h"
#include "utils.h"
#include <stdbool.h>
#include <FreeRTOS.h>
#include <timers.h>

struct tmr_freertos
{
	struct tmr timer;

	TimerHandle_t timerid; /** as returned from xTimerCreateStatic() */
	StaticTimer_t buffer;
	TickType_t period_ticks;
	UBaseType_t cb_invocations; /**< number of times the callback has been invoked */

	double period;
	bool expired;
};

static void
tmr_handler(TimerHandle_t tmr)
{
	struct tmr_freertos *t_f = pvTimerGetTimerID(tmr);
	if (t_f != NULL) {
		t_f->expired = true;
	}
}

static void
tmr_freertos_start(struct tmr *t, double period)
{
	struct tmr_freertos *t_f = CONTAINER_OF(t, struct tmr_freertos, timer);

	const double min_period_sec = 1. / configTICK_RATE_HZ; /* the OS tick period */
	const double actual_period_sec = max_f(period, min_period_sec);

	t_f->period = actual_period_sec;
	t_f->period_ticks = pdMS_TO_TICKS(as_ms(t_f->period));

	/* Stop an active timer */
	if (xTimerIsTimerActive(t_f->timerid)) {
		tmr_stop(t);
	}

	/* Send a request to change the timer period and wait until the timer is active */
	const BaseType_t change_ok = xTimerChangePeriod(t_f->timerid, t_f->period_ticks, 0);
	while (change_ok == pdTRUE && xTimerIsTimerActive(t_f->timerid) == pdFALSE) {
		vTaskDelay(1);
	}

	t_f->expired = false;
}

static void
tmr_freertos_stop(struct tmr *t)
{
	struct tmr_freertos *t_f = CONTAINER_OF(t, struct tmr_freertos, timer);

	xTimerStop(t_f->timerid, 0);
	while (xTimerIsTimerActive(t_f->timerid) == pdFALSE) {
		vTaskDelay(1);
	}
}

static bool
tmr_freertos_running(struct tmr *t)
{
	const struct tmr_freertos *t_f = CONTAINER_OF(t, struct tmr_freertos, timer);
	return xTimerIsTimerActive(t_f->timerid);
}

static bool
tmr_freertos_expired(struct tmr *t)
{
	struct tmr_freertos *t_p = CONTAINER_OF(t, struct tmr_freertos, timer);

	const bool expired = t_p->expired;
	if (expired) {
		t_p->expired = false;
	}
	return expired;
}

static double
tmr_freertos_interval(struct tmr *t)
{
	const struct tmr_freertos *t_p = CONTAINER_OF(t, struct tmr_freertos, timer);

	return t_p->period;
}

static void
tmr_freertos_destroy(struct tmr *t)
{
	struct tmr_freertos *t_f = CONTAINER_OF(t, struct tmr_freertos, timer);

	tmr_stop(t);

	vPortFree(t_f);
	t_f = NULL;
}

struct tmr *
tmr_freertos_create(void)
{
	struct tmr_freertos *t_f = pvPortCalloc(1, sizeof *t_f);
	if (!t_f) {
		return NULL;
	}

	/* New timer is dormant.
	 * The following call will not fail because the memory has already been allocated with
	 * pvPortCalloc()
	 */
	t_f->timerid = xTimerCreateStatic("timer",
		portMAX_DELAY, /* It will change in tmr_start */
		pdTRUE, /* Auto reloading is what we do */
		t_f, /* To be retrieved by the callback */
		tmr_handler,
		&t_f->buffer);

	t_f->timer.destroy = tmr_freertos_destroy;
	t_f->timer.start = tmr_freertos_start;
	t_f->timer.stop = tmr_freertos_stop;
	t_f->timer.running = tmr_freertos_running;
	t_f->timer.expired = tmr_freertos_expired;
	t_f->timer.interval = tmr_freertos_interval;
	t_f->period_ticks = 0;
	t_f->period = 0;
	t_f->expired = false;

	return &t_f->timer;
}
