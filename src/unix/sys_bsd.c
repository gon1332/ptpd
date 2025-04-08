/**
 * @file sys_bsd.c
 * Defines the timer API.
 *
 * Copyright (c) 2025 Ioannis Konstantelias
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "sys.h"
#include <sys/param.h>
#include <sys/cpuset.h>
#include "ptpd_config.h"

int
set_cpu_affinity(int cpu)
{
#ifdef HAVE_SYS_CPUSET_H
	cpuset_t mask;
	CPU_ZERO(&mask);
	if (cpu >= 0) {
		CPU_SET(cpu, &mask);
	} else {
		int i;
		for (i = 0; i < CPU_SETSIZE; i++) {
			CPU_SET(i, &mask);
		}
	}
	return (cpuset_setaffinity(CPU_LEVEL_WHICH, CPU_WHICH_PID, -1, sizeof(mask), &mask));
#endif /* HAVE_SYS_CPUSET_H */

	return -1;
}
