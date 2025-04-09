/**
 * Copyright (c) 2025 Ioannis Konstantelias
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <time.h>
#include <unistd.h>
#include <unity.h>
#include "sys.h"
#include "timer.h"
#include "timer_itimer.h"
#include "timer_posix.h"

void
setUp(void)
{
}

void
tearDown(void)
{
}

void
test_posix(void)
{
	const double interval_seconds = 0.5;

	const struct impl impl = {.tmr_create = tmr_posix_create};
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
}

void
test_itimer(void)
{
	const double interval_seconds = 0.5;

	const struct impl impl = {.tmr_create = tmr_itimer_create};
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
}

int
main(void)
{
	UNITY_BEGIN();

	RUN_TEST(test_posix);
	RUN_TEST(test_itimer);

	return UNITY_END();
}
