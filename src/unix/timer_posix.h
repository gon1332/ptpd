/**
 * @file timer_posix.h
 * Implements the timer API using POSIX timers.
 *
 * Copyright (c) 2025 Ioannis Konstantelias
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef TIMER_POSIX_H_
#define TIMER_POSIX_H_

#include "timer.h"

struct tmr *tmr_posix_create(void);

#endif /* TIMER_POSIX_H_ */
