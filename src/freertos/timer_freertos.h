/**
 * @file timer_freertos.h
 * Implements the timer API using FreeRTOS timers.
 *
 * Copyright (c) 2025 Ioannis Konstantelias
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef TIMER_FREERTOS_H_
#define TIMER_FREERTOS_H_

#include "timer.h"

struct tmr *tmr_freertos_create(void);

#endif /* TIMER_FreeRTOS_H_ */
