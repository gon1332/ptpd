/**
 * Copyright (c) 2025 Ioannis Konstantelias
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdlib.h>
#include <unistd.h>
#include <unity.h>
#include <FreeRTOS.h>
#include <task.h>
#include "sys.h"
#include "timer.h"
#include "timer_freertos.h"


void
setUp(void)
{
}

void
tearDown(void)
{
}

void
test_freertos(void)
{
	const double interval_seconds = 0.5;

	const struct impl impl = {.tmr_create = tmr_freertos_create};
	struct tmr *t = tmr_create(&impl);
	TEST_ASSERT_NOT_NULL(t);
	TEST_ASSERT_FALSE(tmr_running(t));
	TEST_ASSERT_FALSE(tmr_expired(t));

	tmr_start(t, interval_seconds);
	TEST_ASSERT_TRUE(tmr_running(t));
	TEST_ASSERT_FALSE(tmr_expired(t));
	TEST_ASSERT_EQUAL(interval_seconds, tmr_interval(t));

	tmr_stop(t);
	TEST_ASSERT_FALSE(tmr_running(t));
	TEST_ASSERT_FALSE(tmr_expired(t));
	TEST_ASSERT_EQUAL(interval_seconds, tmr_interval(t));

	tmr_start(t, interval_seconds);
	TEST_ASSERT_TRUE(tmr_running(t));
	TEST_ASSERT_FALSE(tmr_expired(t));
	TEST_ASSERT_EQUAL(interval_seconds, tmr_interval(t));

	sleep_for(1);

	TEST_ASSERT_TRUE(tmr_running(t));
	TEST_ASSERT_TRUE(tmr_expired(t));
	TEST_ASSERT_EQUAL(interval_seconds, tmr_interval(t));

	tmr_start(t, interval_seconds);
	TEST_ASSERT_TRUE(tmr_running(t));
	TEST_ASSERT_FALSE(tmr_expired(t));
	TEST_ASSERT_EQUAL(interval_seconds, tmr_interval(t));

	tmr_stop(t);
	tmr_destroy(t);
	// RUN_TEST(test_freertos);
	vTaskSuspend(NULL);
}

int
main(void)
{
	UNITY_BEGIN();

	const BaseType_t task = xTaskCreate(test_freertos,
		"test_freertos",
		configMINIMAL_STACK_SIZE,
		NULL,
		tskIDLE_PRIORITY,
		NULL);
	if (task == pdFAIL) {
		return EXIT_FAILURE;
	}
	vTaskStartScheduler();


	return UNITY_END();
}
