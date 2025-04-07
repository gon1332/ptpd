/**
 * @file timer_priv.h
 * Defines the private bits of timer API.
 *
 * Copyright (c) 2025 Ioannis Konstantelias
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef TIMER_PRIV_H_
#define TIMER_PRIV_H_

#include <stdbool.h>

struct tmr
{
	void (*start)(struct tmr *, double);
	void (*stop)(struct tmr *);
	bool (*running)(struct tmr *);
	bool (*expired)(struct tmr *);
	double (*interval)(struct tmr *t);
	void (*destroy)(struct tmr *);
};

#endif /* TIMER_PRIV_H_ */
