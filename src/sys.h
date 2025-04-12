/**
 * @file sys.h
 * Defines the timer API.
 *
 * Copyright (c) 2025 Ioannis Konstantelias
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef SYS_H_
#define SYS_H_

#include "ptp_datatypes.h"

void sleep_for(double duration);

int set_cpu_affinity(int cpu);

enum clock_type
{
	CLOCK_STEADY,
	CLOCK_SYSTEM
};

/**
 * @brief Returns the current time
 * If type is CLOCK_STEADY, it will return a time point of a monotonic clock that only increases.
 * If type is CLOCK_SYSTEM, it will return the current real time.
 * @param type Clock type - see enum clock_type
 * @param[out] time Time from the system
 */
void get_time(enum clock_type type, TimeInternal *time);

/**
 * @brief Sets the system (CLOCK_SYSTEM) time
 * @param[in] time Time from the system
 */
void set_time(const TimeInternal *time);

// void thread_create(void);

#endif /* SYS_H_ */
