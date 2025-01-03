/**
 * Copyright (c) 2024 Ioannis Konstantelias
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <string.h>
#include <unity.h>
#include <time.h>
#include <sys/time.h>
#include "time_ops.h"
#include "ptp_datatypes.h"

#include <stdint.h>

#define TIME_INTERVAL_MAX 0x7FFFFFFFFFFF0000LL
#define TIME_INTERVAL_MIN 0x8000000000000000LL

#define TIME_INTERNAL_MAX_SEC  (0x00007FFFFFFFFFFFLL / NSEC_IN_SEC)
#define TIME_INTERNAL_MAX_NSEC (0x00007FFFFFFFFFFFLL % NSEC_IN_SEC)

#define TIME_INTERNAL_MIN_SEC  (-0x0000800000000000LL / NSEC_IN_SEC)
#define TIME_INTERNAL_MIN_NSEC (-0x0000800000000000LL % NSEC_IN_SEC)

#define TEST_ASSERT_EQUAL_UINT48(expected, actual)                                     \
	do {                                                                           \
		TEST_ASSERT_GREATER_THAN_UINT(4, sizeof(actual));                      \
		TEST_ASSERT_EQUAL_UINT16(((expected) >> 32) & 0xFFFF, (actual).msb);   \
		TEST_ASSERT_EQUAL_UINT32((expected) & (0x0000FFFFFFFF), (actual).lsb); \
	} while (0)

int64_t
to_int64_t(void *val)
{
	return *(int64_t *)val;
}

void
setUp(void)
{
}

void
tearDown(void)
{
}

void
test_ti_to_time_interval(void)
{
	Integer64 to;
	{
		TimeInternal from = {0};
		ti_to_integer64(from, &to);
		TEST_ASSERT_EQUAL_INT64(0LL, to_int64_t(&to));
	}
	{
		TimeInternal from = {1};
		ti_to_integer64(from, &to);
		TEST_ASSERT_EQUAL_INT64(65536LL, to_int64_t(&to));
	}
	{
		TimeInternal from = {NSEC_IN_SEC};
		ti_to_integer64(from, &to);
		TEST_ASSERT_EQUAL_INT64(65536000000000LL, to_int64_t(&to));
	}
	{
		TimeInternal from = {-NSEC_IN_SEC};
		ti_to_integer64(from, &to);
		TEST_ASSERT_EQUAL_INT64(-65536000000000LL, to_int64_t(&to));
	}
	{
		TimeInternal from = {2 * NSEC_IN_SEC + 2};
		ti_to_integer64(from, &to);
		TEST_ASSERT_EQUAL_INT64(131072000131072LL, to_int64_t(&to));
	}
	/* Positive largest TimeInterval */
	{
		TimeInternal from = {TIME_INTERNAL_MAX_SEC * NSEC_IN_SEC + TIME_INTERNAL_MAX_NSEC};
		ti_to_integer64(from, &to);
		TEST_ASSERT_EQUAL_INT64_MESSAGE(TIME_INTERVAL_MAX, to_int64_t(&to),
						"Positive largest time TimeInterval is not "
						"represented correctly by TimeInternal");
	}
	/* Negative largest TimeInterval */
	{
		TimeInternal from = {TIME_INTERNAL_MIN_SEC * NSEC_IN_SEC + TIME_INTERNAL_MIN_NSEC};
		ti_to_integer64(from, &to);
		TEST_ASSERT_EQUAL_INT64_MESSAGE(TIME_INTERVAL_MIN, to_int64_t(&to),
						"Negative largest time TimeInterval is not "
						"represented correctly by TimeInternal");
	}
	/* Positive out of range */
	{
		TimeInternal from = {(TIME_INTERNAL_MAX_SEC + 1) * NSEC_IN_SEC};
		ti_to_integer64(from, &to);
		TEST_ASSERT_EQUAL_INT64_MESSAGE(TIME_INTERVAL_MAX, to_int64_t(&to),
						"Positive time outside the maximum range is not "
						"encoded as the largest positive value of "
						"TimeInterval");
	}
	/* Negative out of range */
	{
		TimeInternal from = {(TIME_INTERNAL_MIN_SEC - 1) * NSEC_IN_SEC};
		ti_to_integer64(from, &to);
		TEST_ASSERT_EQUAL_INT64_MESSAGE(TIME_INTERVAL_MIN, to_int64_t(&to),
						"Negative time outside the maximum range is not "
						"encoded as the largest negative value of "
						"TimeInterval");
	}
}

void
test_ti_from_time_interval(void)
{
	TimeInternal to;
	{
		const Integer64 from = 0LL;
		ti_from_integer64(from, &to);
		TEST_ASSERT_EQUAL_INT64(0LL, to.nanoseconds);
	}
	{
		const Integer64 from = 65536LL;
		ti_from_integer64(from, &to);
		TEST_ASSERT_EQUAL_INT64(1LL, to.nanoseconds);
	}
	{
		const Integer64 from = 65536000000000LL;
		ti_from_integer64(from, &to);
		TEST_ASSERT_EQUAL_INT64(NSEC_IN_SEC, to.nanoseconds);
	}
	{
		const Integer64 from = -65536000000000LL;
		ti_from_integer64(from, &to);
		TEST_ASSERT_EQUAL_INT64(-NSEC_IN_SEC, to.nanoseconds);
	}
	{
		const Integer64 from = 131072000131072LL;
		ti_from_integer64(from, &to);
		TEST_ASSERT_EQUAL_INT64(2 * NSEC_IN_SEC + 2, to.nanoseconds);
	}
	/* Positive largest TimeInterval */
	{
		const Integer64 from = TIME_INTERVAL_MAX;
		ti_from_integer64(from, &to);
		TEST_ASSERT_EQUAL_INT64(TIME_INTERNAL_MAX_SEC * NSEC_IN_SEC +
						TIME_INTERNAL_MAX_NSEC,
					to.nanoseconds);
	}
	/* Negative largest TimeInterval */
	{
		const Integer64 from = TIME_INTERVAL_MIN;
		ti_from_integer64(from, &to);
		TEST_ASSERT_EQUAL_INT64(TIME_INTERNAL_MIN_SEC * NSEC_IN_SEC +
						TIME_INTERNAL_MIN_NSEC,
					to.nanoseconds);
	}
	/* Fractional nanoseconds are not considered */
	{
		const Integer64 from = TIME_INTERVAL_MAX + 1;
		ti_from_integer64(from, &to);
		TEST_ASSERT_EQUAL_INT64(TIME_INTERNAL_MAX_SEC * NSEC_IN_SEC +
						TIME_INTERNAL_MAX_NSEC,
					to.nanoseconds);
	}
	/* Fractional nanoseconds are not considered */
	{
		const Integer64 from = TIME_INTERVAL_MIN + 1;
		ti_from_integer64(from, &to);
		TEST_ASSERT_EQUAL_INT64(TIME_INTERNAL_MIN_SEC * NSEC_IN_SEC +
						TIME_INTERNAL_MIN_NSEC,
					to.nanoseconds);
	}
}

void
test_ti_to_timestamp(void)
{
	Timestamp to;
	{
		TimeInternal from = {0};
		ti_to_timestamp(&from, &to);
		TEST_ASSERT_EQUAL_UINT48(0ULL, to.secondsField);
		TEST_ASSERT_EQUAL_UINT32(0UL, to.nanosecondsField);
	}
	{
		const TimeInternal from = {1};
		ti_to_timestamp(&from, &to);
		TEST_ASSERT_EQUAL_UINT48(0ULL, to.secondsField);
		TEST_ASSERT_EQUAL_UINT32(1UL, to.nanosecondsField);
	}
	{
		const TimeInternal from = {2 * NSEC_IN_SEC + 1};
		ti_to_timestamp(&from, &to);
		TEST_ASSERT_EQUAL_UINT48(2ULL, to.secondsField);
		TEST_ASSERT_EQUAL_UINT32(1UL, to.nanosecondsField);
	}
	{
		memset(&to, 0xFF, sizeof(to));
		const TimeInternal from = {1 * NSEC_IN_SEC + NSEC_IN_SEC - 1};
		ti_to_timestamp(&from, &to);
		TEST_ASSERT_EQUAL_UINT48(1ULL, to.secondsField);
		TEST_ASSERT_EQUAL_UINT32(NSEC_IN_SEC - 1, to.nanosecondsField);
	}
}

void
test_ti_from_timestamp(void)
{
	TimeInternal to;
	{
		const Timestamp from = {{0L, 0}, 0L};
		ti_from_timestamp(&from, &to);
		TEST_ASSERT_EQUAL_INT64(0LL, to.nanoseconds);
	}
	{
		const Timestamp from = {{0L, 0}, 1L};
		ti_from_timestamp(&from, &to);
		TEST_ASSERT_EQUAL_INT64(1LL, to.nanoseconds);
	}
	{
		const Timestamp from = {{1L, 0}, 1L};
		ti_from_timestamp(&from, &to);
		TEST_ASSERT_EQUAL_INT64(1 * NSEC_IN_SEC + 1, to.nanoseconds);
	}
	{
		const Timestamp from = {{INT32_MAX, 0}, NSEC_IN_SEC - 1};
		ti_from_timestamp(&from, &to);
		TEST_ASSERT_EQUAL_INT64(INT32_MAX * NSEC_IN_SEC + NSEC_IN_SEC - 1, to.nanoseconds);
	}
}

void
test_ti_from_timespec(void)
{
	TimeInternal to;
	{
		const struct timespec from = {0, 0};
		ti_from_timespec(&from, &to);
		TEST_ASSERT_EQUAL_INT64(0LL, to.nanoseconds);
	}
	{
		const struct timespec from = {0, 1};
		ti_from_timespec(&from, &to);
		TEST_ASSERT_EQUAL_INT64(1LL, to.nanoseconds);
	}
	{
		/* GNU Libc generates struct timespec with tv_nsec less than 1000000000.
		 * No need to test more than that.
		 */
		const struct timespec from = {INT32_MAX, NSEC_IN_SEC - 1};
		ti_from_timespec(&from, &to);
		TEST_ASSERT_EQUAL_INT64(INT32_MAX * NSEC_IN_SEC + NSEC_IN_SEC - 1, to.nanoseconds);
	}
}

void
test_ti_from_timeval(void)
{
	TimeInternal to;
	{
		const struct timeval from = {0, 0};
		ti_from_timeval(&from, &to);
		TEST_ASSERT_EQUAL_INT64(0LL, to.nanoseconds);
	}
	{
		const struct timeval from = {0, 1};
		ti_from_timeval(&from, &to);
		TEST_ASSERT_EQUAL_INT64(1000LL, to.nanoseconds);
	}
	{
		/* GNU Libc generates struct timeval with tv_usec less than 1000000.
		 * No need to test more than that.
		 */
		const struct timeval from = {INT32_MAX, USEC_IN_SEC - 1};
		ti_from_timeval(&from, &to);
		TEST_ASSERT_EQUAL_INT64(INT32_MAX * NSEC_IN_SEC + NSEC_IN_SEC - 1000,
					to.nanoseconds);
	}
}

void
test_addTime(void)
{
	TimeInternal result;
	{
		TimeInternal time1 = {0};
		TimeInternal time2 = {0};
		addTime(&result, &time1, &time2);
		TEST_ASSERT_EQUAL_INT64(0LL, result.nanoseconds);
	}
	{
		TimeInternal time1 = {10};
		TimeInternal time2 = {20000};
		addTime(&result, &time1, &time2);
		TEST_ASSERT_EQUAL_INT64(20010LL, result.nanoseconds);
	}
	{
		TimeInternal time1 = {10};
		TimeInternal time2 = {-1};
		addTime(&result, &time1, &time2);
		TEST_ASSERT_EQUAL_INT64(9LL, result.nanoseconds);
	}
}

void
test_subTime(void)
{
	TimeInternal result;
	{
		TimeInternal time1 = {0};
		TimeInternal time2 = {0};
		subTime(&result, &time1, &time2);
		TEST_ASSERT_EQUAL_INT64(0LL, result.nanoseconds);
	}
	{
		TimeInternal time1 = {10};
		TimeInternal time2 = {20};
		subTime(&result, &time1, &time2);
		TEST_ASSERT_EQUAL_INT64(-10LL, result.nanoseconds);
	}
	{
		TimeInternal time1 = {10};
		TimeInternal time2 = {10};
		subTime(&result, &time1, &time2);
		TEST_ASSERT_EQUAL_INT64(0LL, result.nanoseconds);
	}
}

void
test_ti_div2(void)
{
	{
		TimeInternal time = {0};
		ti_div2(&time);
		TEST_ASSERT_EQUAL_INT64(0LL, time.nanoseconds);
	}
	{
		TimeInternal time = {NSEC_IN_SEC};
		ti_div2(&time);
		TEST_ASSERT_EQUAL_INT64(500000000LL, time.nanoseconds);
	}
	{
		TimeInternal time = {2 * NSEC_IN_SEC};
		ti_div2(&time);
		TEST_ASSERT_EQUAL_INT64(NSEC_IN_SEC, time.nanoseconds);
	}
}

void
test_ti_clear(void)
{
	{
		TimeInternal time = {0};
		ti_clear(&time);
		TEST_ASSERT_EQUAL_INT64(0LL, time.nanoseconds);
	}
	{
		TimeInternal time = {1};
		ti_clear(&time);
		TEST_ASSERT_EQUAL_INT64(0LL, time.nanoseconds);
	}
}

void
test_ti_is_negative(void)
{
	{
		TimeInternal time = {0};
		TEST_ASSERT_EQUAL_INT(0, ti_is_negative(&time));
	}
	{
		TimeInternal time = {1};
		TEST_ASSERT_EQUAL_INT(0, ti_is_negative(&time));
	}
	{
		TimeInternal time = {-1};
		TEST_ASSERT_EQUAL_INT(1, ti_is_negative(&time));
	}
}

void
test_ti_cmp(void)
{
	{
		TimeInternal time1 = {0};
		TimeInternal time2 = {0};
		TEST_ASSERT_EQUAL(0, ti_cmp(&time1, &time2));
	}
	{
		TimeInternal time1 = {1};
		TimeInternal time2 = {0};
		TEST_ASSERT_GREATER_THAN_INT(0, ti_cmp(&time1, &time2));
	}
	{
		TimeInternal time1 = {0};
		TimeInternal time2 = {1};
		TEST_ASSERT_LESS_THAN_INT(0, ti_cmp(&time1, &time2));
	}
	{
		TimeInternal time1 = {0};
		TimeInternal time2 = {-1};
		TEST_ASSERT_GREATER_THAN_INT(0, ti_cmp(&time1, &time2));
	}
}

void
test_ti_inc(void)
{
	{
		TimeInternal time = {0};
		ti_inc(&time, 5, INC_NANOSECONDS);
		TEST_ASSERT_EQUAL_INT64(5, time.nanoseconds);
	}
	{
		TimeInternal time = {0};
		ti_inc(&time, -5, INC_NANOSECONDS);
		TEST_ASSERT_EQUAL_INT64(-5, time.nanoseconds);
	}
	{
		TimeInternal time = {0};
		ti_inc(&time, 5, INC_SECONDS);
		TEST_ASSERT_EQUAL_INT64(5 * NSEC_IN_SEC, time.nanoseconds);
	}
	{
		TimeInternal time = {0};
		ti_inc(&time, -5, INC_SECONDS);
		TEST_ASSERT_EQUAL_INT64(-5 * NSEC_IN_SEC, time.nanoseconds);
	}
}

void
test_ti_abs(void)
{
	{
		TimeInternal time = {0};
		ti_abs(&time);
		TEST_ASSERT_EQUAL_INT64(0LL, time.nanoseconds);
	}
	{
		TimeInternal time = {NSEC_IN_SEC};
		ti_abs(&time);
		TEST_ASSERT_EQUAL_INT64(NSEC_IN_SEC, time.nanoseconds);
	}
	{
		TimeInternal time = {-NSEC_IN_SEC};
		ti_abs(&time);
		TEST_ASSERT_EQUAL_INT64(NSEC_IN_SEC, time.nanoseconds);
	}
}

void
test_ti_is_close(void)
{
	{
		TimeInternal time1 = {0};
		TimeInternal time2 = {0};
		TEST_ASSERT_EQUAL_INT(1LL, ti_is_close(&time1, &time2, 1));
	}
	{
		TimeInternal time1 = {1};
		TimeInternal time2 = {2};
		TEST_ASSERT_EQUAL_INT(1LL, ti_is_close(&time1, &time2, 1));
	}
	{
		TimeInternal time1 = {2};
		TimeInternal time2 = {1};
		TEST_ASSERT_EQUAL_INT(1LL, ti_is_close(&time1, &time2, 1));
	}
	{
		TimeInternal time1 = {1};
		TimeInternal time2 = {3};
		TEST_ASSERT_EQUAL_INT(0LL, ti_is_close(&time1, &time2, 1));
	}
	{
		TimeInternal time1 = {3};
		TimeInternal time2 = {1};
		TEST_ASSERT_EQUAL_INT(0LL, ti_is_close(&time1, &time2, 1));
	}
}

int
main(void)
{
	UNITY_BEGIN();

	RUN_TEST(test_ti_to_time_interval);
	RUN_TEST(test_ti_from_time_interval);
	RUN_TEST(test_ti_to_timestamp);
	RUN_TEST(test_ti_from_timestamp);
	RUN_TEST(test_ti_from_timespec);
	RUN_TEST(test_ti_from_timeval);
	RUN_TEST(test_addTime);
	RUN_TEST(test_subTime);
	RUN_TEST(test_ti_div2);
	RUN_TEST(test_ti_clear);
	RUN_TEST(test_ti_is_negative);
	RUN_TEST(test_ti_cmp);
	RUN_TEST(test_ti_inc);
	RUN_TEST(test_ti_abs);
	RUN_TEST(test_ti_is_close);

	return UNITY_END();
}
