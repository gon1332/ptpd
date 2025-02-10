/**
 * @file timer.h
 * Defines the timer API.
 *
 * Copyright (c) 2025 Ioannis Konstantelias
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <stdbool.h>

/**
 * Forward declaration
 */
struct tmr;

enum tmr_type
{
	TIMER_POSIX,
	TIMER_ITIMER
};

/**
 * @brief Creates a disarmed timer
 * @param type The type of timer to create
 * @returns The created timer object, otherwise NULL
 */
struct tmr *tmr_create(enum tmr_type type);

/**
 * @brief Destroys the timer
 * @param t The timer to destroy
 */
void tmr_destroy(struct tmr *t);

/**
 * @brief Starts a recurring timer
 * @param t The timer object
 * @param period Period of the timer in seconds
 */
void tmr_start(struct tmr *t, double period);

/**
 * @brief Stops the timer
 * @param t The timer to stop
 */
void tmr_stop(struct tmr *t);

/**
 * @brief Checks if the timer is running
 * @param t The timer to check
 * @returns True if the selected timer is running
 */
bool tmr_running(struct tmr *t);

/**
 * @brief Checks if the timer has expired
 * @param t The timer to check
 * @returns True if the selected timer has expired
 */
bool tmr_expired(struct tmr *t);

/**
 * @param t The timer to check
 * @returns The timer interval in seconds
 */
double tmr_interval(struct tmr *t);

#endif /* TIMER_H_ */
