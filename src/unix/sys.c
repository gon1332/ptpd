/**
 * @file sys.c
 * Defines the timer API.
 *
 * Copyright (c) 2025 Ioannis Konstantelias
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "sys.h"
#include <errno.h>
#define __USE_GNU
#include <sched.h>
#include "ptpd_config.h"
#include "time_ops.h"

void
sleep_for(double duration)
{
	const TimeInternal duration_ti = ti_from_double(duration);
	struct timespec remaining;
	ti_to_timespec(&duration_ti, &remaining);

	while (nanosleep(&remaining, &remaining) == -1 && errno == EINTR) {
	}
}

int
set_cpu_affinity(int cpu)
{

#ifdef __QNXNTO__
	unsigned num_elements = 0;
	int *rsizep, masksize_bytes, size;
	int *rmaskp, *imaskp;
	void *my_data;
	uint32_t cpun;
	num_elements = RMSK_SIZE(_syspage_ptr->num_cpu);

	masksize_bytes = num_elements * sizeof(unsigned);

	size = sizeof(int) + 2 * masksize_bytes;
	if ((my_data = malloc(size)) == NULL) {
		return -1;
	} else {
		memset(my_data, 0x00, size);

		rsizep = (int *)my_data;
		rmaskp = rsizep + 1;
		imaskp = rmaskp + num_elements;

		*rsizep = num_elements;

		if (cpu > _syspage_ptr->num_cpu) {
			return -1;
		}

		if (cpu >= 0) {
			cpun = (uint32_t)cpu;
			RMSK_SET(cpun, rmaskp);
			RMSK_SET(cpun, imaskp);
		} else {
			for (cpun = 0; cpun < num_elements; cpun++) {
				RMSK_SET(cpun, rmaskp);
				RMSK_SET(cpun, imaskp);
			}
		}
		int ret = ThreadCtl(_NTO_TCTL_RUNMASK_GET_AND_SET_INHERIT, my_data);
		free(my_data);
		return ret;
	}

#endif

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

#if defined(linux) && defined(HAVE_SCHED_H)
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
#endif /* linux && HAVE_SCHED_H */

	return -1;
}
