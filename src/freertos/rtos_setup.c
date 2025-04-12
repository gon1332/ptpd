/**
 * @file rtos_setup.c
 * Implements FreeRTOS hooks.
 *
 * Copyright (c) 2025 Ioannis Konstantelias
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include <stdlib.h>

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
	printf("Stack Overflow for task %s\n", pcTaskName);
	exit(1);
}

void vApplicationMallocFailedHook(void)
{
	printf("Malloc failed\n");
	exit(1);
}