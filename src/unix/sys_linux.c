/**
 * @file sys_linux.c
 * Defines the timer API.
 *
 * Copyright (c) 2025 Ioannis Konstantelias
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "sys.h"
#define _GNU_SOURCE
#include <sched.h>
#include "ptpd_config.h"

int
set_cpu_affinity(int cpu)
{
#ifdef HAVE_SCHED_H
	cpu_set_t mask;
	CPU_ZERO(&mask);
	if (cpu >= 0) {
		CPU_SET(cpu, &mask);
	} else {
		int i;
		for (i = 0; i < CPU_SETSIZE; i++) {
			CPU_SET(i, &mask);
		}
	}
	return sched_setaffinity(0, sizeof(mask), &mask);
#endif /* HAVE_SCHED_H */

	return -1;
}
