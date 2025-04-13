/**
 * @file impl.c
 *
 * Copyright (c) 2025 Ioannis Konstantelias
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "ptpd_config.h"
#include "impl.h"
#include "sys.h"
#include "timer_freertos.h"

const struct impl g_impl = {
	.tmr_create = tmr_freertos_create,
	.sleep_for = sleep_for,
	.set_cpu_affinity = set_cpu_affinity,
	.get_time = get_time,
	.set_time = set_time,
};
