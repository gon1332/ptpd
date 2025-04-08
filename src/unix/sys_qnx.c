/**
 * @file sys_qnx.c
 * Defines the timer API.
 *
 * Copyright (c) 2025 Ioannis Konstantelias
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "sys.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/neutrino.h>

int
set_cpu_affinity(int cpu)
{
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
	}
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
