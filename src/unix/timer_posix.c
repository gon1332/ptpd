/**
 * @file timer_posix.c
 * Implements the timer API using POSIX timers.
 *
 * Copyright (c) 2025 Ioannis Konstantelias
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "timer_posix.h"
#include "timer_priv.h"
#include "utils.h"
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct tmr_posix
{
	struct tmr timer;
	timer_t timerid; /**< as returned from timer_create(2) */
	double period;
	bool expired;
	bool running;
};

static void
tmr_handler(int sig, siginfo_t *si, void *uc)
{
	(void)sig;
	(void)uc;
	struct tmr_posix *t_p = si->si_value.sival_ptr;
	const bool from_timer = si->si_code == SI_TIMER;

	if (from_timer && t_p) {
		t_p->expired = true;
	}
}

static void
tmr_posix_start(struct tmr *t, double period)
{
	struct tmr_posix *t_p = CONTAINER_OF(t, struct tmr_posix, timer);

	const double min_interval_in_seconds = 0.000250;

	period = MAX(period, min_interval_in_seconds);
	t_p->period = period;

	struct itimerspec its = {0};
	its.it_interval.tv_sec = (time_t)period;
	its.it_interval.tv_nsec = (long)((period - (double)its.it_interval.tv_sec) * 1e9);
	its.it_value = its.it_interval;

	if (timer_settime(t_p->timerid, 0, &its, NULL) == -1) {
		perror("timer_settime");
		return;
	}

	t_p->running = true;
	t_p->expired = false;
}

static void
tmr_posix_stop(struct tmr *t)
{
	struct tmr_posix *t_p = CONTAINER_OF(t, struct tmr_posix, timer);

	struct itimerspec its = {0};

	if (timer_settime(t_p->timerid, 0, &its, NULL) == -1) {
		perror("timer_settime");
		return;
	}

	t_p->running = false;
}

static bool
tmr_posix_running(struct tmr *t)
{
	const struct tmr_posix *t_p = CONTAINER_OF(t, struct tmr_posix, timer);

	return t_p->running;
}

static bool
tmr_posix_expired(struct tmr *t)
{
	struct tmr_posix *t_p = CONTAINER_OF(t, struct tmr_posix, timer);

	const bool expired = t_p->expired;
	if (expired) {
		t_p->expired = false;
	}
	return expired;
}

static double
tmr_posix_interval(struct tmr *t)
{
	const struct tmr_posix *t_p = CONTAINER_OF(t, struct tmr_posix, timer);

	return t_p->period;
}

static void
tmr_posix_destroy(struct tmr *t)
{
	struct tmr_posix *t_p = CONTAINER_OF(t, struct tmr_posix, timer);

	if (timer_delete(t_p->timerid) == -1) {
		perror("timer_delete");
	}

	free(t_p);
	t_p = NULL;
}

struct tmr *
tmr_posix_create(void)
{
	struct tmr_posix *t_p = calloc(1, sizeof *t_p);
	if (!t_p) {
		return NULL;
	}

	static bool handler_installed = false;
	if (!handler_installed) {
		signal(SIGALRM, SIG_IGN);
		struct sigaction sa;
		sa.sa_flags = SA_SIGINFO;
		sa.sa_sigaction = tmr_handler;
		sigemptyset(&sa.sa_mask);
		if (sigaction(SIGALRM, &sa, NULL) == -1) {
			perror("sigaction");
			goto fail;
		}
		handler_installed = true;
	}

	struct sigevent sev = {
		.sigev_notify = SIGEV_SIGNAL,
		.sigev_signo = SIGALRM,
		.sigev_value.sival_ptr = t_p,
	};

	if (timer_create(CLOCK_MONOTONIC, &sev, &t_p->timerid) == -1) {
		perror("timer_create");
		goto fail;
	}

	t_p->timer.destroy = tmr_posix_destroy;
	t_p->timer.start = tmr_posix_start;
	t_p->timer.stop = tmr_posix_stop;
	t_p->timer.running = tmr_posix_running;
	t_p->timer.expired = tmr_posix_expired;
	t_p->timer.interval = tmr_posix_interval;
	t_p->running = false;
	t_p->expired = false;

	return &t_p->timer;
fail:
	free(t_p);
	t_p = NULL;
	return NULL;
}
