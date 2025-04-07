/**
 * @file timer_itimer.h
 * Implements the timer API using itimer timers.
 *
 * Copyright (c) 2025 Ioannis Konstantelias
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef TIMER_ITIMER_H_
#define TIMER_ITIMER_H_

#include "timer.h"

struct tmr *tmr_itimer_create(void);

#endif /* TIMER_ITIMER_H_ */
