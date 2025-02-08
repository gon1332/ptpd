/*-
 * Copyright (c) 2024      Ioannis Konstantelias,
 * Copyright (c) 2015      Wojciech Owczarek,
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef EVENTTIMER_H_
#define EVENTTIMER_H_

#include "../ptpd.h"

#define EVENTTIMER_MAX_DESC		20
#define EVENTTIMER_MIN_INTERVAL_US	250 /* 4000/sec */

typedef struct EventTimer EventTimer;

struct EventTimer {

	/* data */
	char id[EVENTTIMER_MAX_DESC + 1];
	Boolean expired;
	Boolean running;

	/* "methods" */
	void (*start) (EventTimer* timer, double interval);
	void (*stop) (EventTimer* timer);
	void (*reset) (EventTimer* timer);
	void (*shutdown) (EventTimer* timer);
	Boolean (*isExpired) (EventTimer* timer);
	Boolean (*isRunning) (EventTimer* timer);	

	/* implementation data */
#ifdef HAVE_POSIX_TIMER
	timer_t timerId;
#else
	int32_t itimerInterval;
	int32_t itimerLeft;
#endif /* HAVE_POSIX_TIMER */

	/* linked list */
	EventTimer *_first;
	EventTimer *_next;
	EventTimer *_prev;

};

EventTimer *createEventTimer(const char *id);
void freeEventTimer(EventTimer **timer);
void setupEventTimer(EventTimer *timer);

void startEventTimers();
void shutdownEventTimers();


#endif /* EVENTTIMER_H_ */

