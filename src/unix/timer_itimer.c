/**
 * @file timer_itimer.c
 * Implements the timer API using itimer timers.
 *
 * A global itimer with a fine-enough interval is used as the baseline of this implementation.
 * This global timer, counts how many times has elapsed.
 * All the individual timers, knowing the period of the global timer, calculate and update their
 * status. Updating of the status is lazy as it is done every time the user is querying the status.
 * The timers are connected with each other in a list.
 *
 * Copyright (c) 2025 Ioannis Konstantelias
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "timer_itimer.h"

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "timer_priv.h"
#include "utils.h"
#include "list.h"

#define ITIMER_INTERVAL_IN_MICROSECONDS 31250

struct tmr_itimer
{
	struct tmr timer;
	bool expired;
	bool running;
	double period;
	int interval; /**< Interval in counts of itimer expirations */
	int time_left; /**< Time left in counts of itimer expirations */

	struct list list;
};

static struct list g_head = {.next = &g_head, .prev = &g_head};
static sig_atomic_t g_elapsed = 0;

static void
tmr_handler(int sig)
{
	(void)sig;
	++g_elapsed;
}

static void
tmr_update(void)
{
	if (g_elapsed == 0) {
		return;
	}

	/*
	 * if time actually passed, then decrease every timer left
	 * the one(s) that went to zero or negative are:
	 *  a) rearmed at the original time (ignoring the time that may have passed ahead)
	 *  b) have their expiration latched until timerExpired() is called
	 */
	for (struct list *curr = g_head.next; curr != &g_head; curr = curr->next) {
		struct tmr_itimer *t_i = CONTAINER_OF(curr, struct tmr_itimer, list);
		if (t_i->interval > 0) {
			t_i->time_left -= g_elapsed;
			if (t_i->time_left <= 0) {
				t_i->time_left = t_i->interval;
				t_i->expired = true;
			}
		}
	}

	g_elapsed = 0;
}

static void
tmr_itimer_start(struct tmr *t, double period)
{
	struct tmr_itimer *t_i = CONTAINER_OF(t, struct tmr_itimer, timer);

	t_i->period = period;

	const double min_interval_in_seconds = 0.000250;
	period = max_f(period, min_interval_in_seconds);

	const unsigned interval_counts = (period * 1e6) / ITIMER_INTERVAL_IN_MICROSECONDS;
	t_i->interval = max_d(interval_counts, 1);
	t_i->time_left = t_i->interval;
	t_i->running = true;
	t_i->expired = false;
}

static void
tmr_itimer_stop(struct tmr *t)
{
	struct tmr_itimer *t_i = CONTAINER_OF(t, struct tmr_itimer, timer);

	t_i->interval = 0;
	t_i->running = false;
}

static bool
tmr_itimer_running(struct tmr *t)
{
	const struct tmr_itimer *t_i = CONTAINER_OF(t, struct tmr_itimer, timer);

	tmr_update();

	return t_i->running;
}

static bool
tmr_itimer_expired(struct tmr *t)
{
	struct tmr_itimer *t_i = CONTAINER_OF(t, struct tmr_itimer, timer);

	tmr_update();

	const bool expired = t_i->expired;
	if (expired) {
		t_i->expired = false;
	}
	return expired;
}

static double
tmr_itimer_interval(struct tmr *t)
{
	const struct tmr_itimer *t_i = CONTAINER_OF(t, struct tmr_itimer, timer);

	return t_i->period;
}

static void
tmr_itimer_destroy(struct tmr *t)
{
	struct tmr_itimer *t_i = CONTAINER_OF(t, struct tmr_itimer, timer);

	list_del(&t_i->list);

	/* The last timer, closes the door */
	if (list_is_empty(&g_head)) {
		struct sigaction sa = {0};
		sa.sa_handler = SIG_DFL;
		sigemptyset(&sa.sa_mask);
		if (sigaction(SIGALRM, &sa, NULL) == -1) {
			perror("sigaction");
			/* fall-through */
		}
	}

	free(t_i);
	t_i = NULL;
}

struct tmr *
tmr_itimer_create(void)
{
	struct tmr_itimer *t_i = calloc(1, sizeof *t_i);
	if (!t_i) {
		return NULL;
	}

	if (list_is_empty(&g_head)) {
		g_elapsed = 0;
		signal(SIGALRM, SIG_IGN);
		struct sigaction sa;
		sa.sa_handler = tmr_handler;
		sigemptyset(&sa.sa_mask);
		if (sigaction(SIGALRM, &sa, NULL) == -1) {
			perror("sigaction");
			goto fail;
		}
		signal(SIGALRM, tmr_handler);

		struct itimerval itimer;
		itimer.it_interval.tv_sec = 0;
		itimer.it_interval.tv_usec = ITIMER_INTERVAL_IN_MICROSECONDS;
		itimer.it_value = itimer.it_interval;

		if (setitimer(ITIMER_REAL, &itimer, NULL) == -1) {
			perror("setitimer");
			goto fail;
		}

		list_create(&g_head);
	}

	t_i->timer.destroy = tmr_itimer_destroy;
	t_i->timer.start = tmr_itimer_start;
	t_i->timer.stop = tmr_itimer_stop;
	t_i->timer.running = tmr_itimer_running;
	t_i->timer.expired = tmr_itimer_expired;
	t_i->timer.interval = tmr_itimer_interval;
	t_i->running = false;
	t_i->expired = false;

	list_add(&g_head, &t_i->list);

	return &t_i->timer;
fail:
	free(t_i);
	t_i = NULL;
	return NULL;
}
