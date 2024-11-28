/**
 * Copyright (c) 2024 Ioannis Konstantelias
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <string.h>
#include <unity.h>
#include "time_ops.h"

#include <stdint.h>

#define NSEC_IN_SEC 1000000000LL

#define TIME_INTERVAL_MAX 0x7FFFFFFFFFFF0000LL
#define TIME_INTERVAL_MIN 0x8000000000000000LL

#define TIME_INTERNAL_MAX_SEC (0x00007FFFFFFFFFFFLL / NSEC_IN_SEC)
#define TIME_INTERNAL_MAX_NSEC (0x00007FFFFFFFFFFFLL % NSEC_IN_SEC)

#define TIME_INTERNAL_MIN_SEC (-0x0000800000000000LL / NSEC_IN_SEC)
#define TIME_INTERNAL_MIN_NSEC (-0x0000800000000000LL % NSEC_IN_SEC)

#define TEST_ASSERT_EQUAL_UINT48(expected, actual) \
	do { \
		TEST_ASSERT_GREATER_THAN_UINT(4, sizeof(actual)); \
		TEST_ASSERT_EQUAL_UINT16(((expected) >> 32) & 0xFFFF, (actual).msb); \
		TEST_ASSERT_EQUAL_UINT32((expected) & 0x0000FFFFFFFF , (actual).lsb); \
	} while (0)


int64_t to_int64_t(void *val)
{
	return *(int64_t *)val;
}

Integer64 to_Integer64(int64_t val)
{
	return *(Integer64 *)&val;
}

void setUp(void)
{}

void tearDown(void)
{}

void test_TimeInternal_to_Integer64(void)
{
	Integer64 to;
	{
		TimeInternal from = {0, 0};
		internalTime_to_integer64(from, &to);
		TEST_ASSERT_EQUAL_INT64(0LL, to_int64_t(&to));
	}
	{
		TimeInternal from = {0, 1};
		internalTime_to_integer64(from, &to);
		TEST_ASSERT_EQUAL_INT64(65536LL, to_int64_t(&to));
	}
	{
		TimeInternal from = {1, 0};
		internalTime_to_integer64(from, &to);
		TEST_ASSERT_EQUAL_INT64(65536000000000LL, to_int64_t(&to));
	}
	{
		TimeInternal from = {-1, 0};
		internalTime_to_integer64(from, &to);
		TEST_ASSERT_EQUAL_INT64(-65536000000000LL, to_int64_t(&to));
	}
	{
		TimeInternal from = {2, 2};
		internalTime_to_integer64(from, &to);
		TEST_ASSERT_EQUAL_INT64(131072000131072LL, to_int64_t(&to));
	}
	{
		TimeInternal from = {-2, 2};
		internalTime_to_integer64(from, &to);
		TEST_ASSERT_EQUAL_INT64(-131072000131072LL, to_int64_t(&to));
	}
	{
		TimeInternal from = {-1, 999999999};
		internalTime_to_integer64(from, &to);
		TEST_ASSERT_EQUAL_INT64(-131071999934464LL, to_int64_t(&to));
	}
	/* Positive largest TimeInterval */
	{
		TimeInternal from = {TIME_INTERNAL_MAX_SEC, TIME_INTERNAL_MAX_NSEC};
		internalTime_to_integer64(from, &to);
		TEST_ASSERT_EQUAL_INT64_MESSAGE(TIME_INTERVAL_MAX, to_int64_t(&to),
			"Positive largest time TimeInterval is not represented correctly by"
			"TimeInternal");
	}
	/* Negative largest TimeInterval */
	{
		TimeInternal from = {TIME_INTERNAL_MIN_SEC, TIME_INTERNAL_MIN_SEC};
		internalTime_to_integer64(from, &to);
		TEST_ASSERT_EQUAL_INT64_MESSAGE(TIME_INTERVAL_MIN, to_int64_t(&to),
			"Negative largest time TimeInterval is not represented correctly by"
			"TimeInternal");
	}
	/* Positive out of range */
	{
		TimeInternal from = {TIME_INTERNAL_MAX_SEC + 1, 0};
		internalTime_to_integer64(from, &to);
		TEST_ASSERT_EQUAL_INT64_MESSAGE(INT64_MAX, to_int64_t(&to),
			"Positive time outside the maximum range is not encoded as the largest "
			"positive value of TimeInterval");
	}
	/* Negative out of range */
	{
		TimeInternal from = {TIME_INTERNAL_MIN_SEC - 1, 0};
		internalTime_to_integer64(from, &to);
		TEST_ASSERT_EQUAL_INT64_MESSAGE(INT64_MIN, to_int64_t(&to),
			"Negative time outside the maximum range is not encoded as the largest "
			"negative value of TimeInterval");
	}
}

void test_Integer64_to_TimeInternal(void)
{
	TimeInternal to;
	{
		Integer64 from = to_Integer64(0LL);
		integer64_to_internalTime(from, &to);
		TEST_ASSERT_EQUAL_INT32(0L, to.seconds);
		TEST_ASSERT_EQUAL_INT32(0L, to.nanoseconds);
	}
	{
		Integer64 from = to_Integer64(65536LL);
		integer64_to_internalTime(from, &to);
		TEST_ASSERT_EQUAL_INT32(0L, to.seconds);
		TEST_ASSERT_EQUAL_INT32(1L, to.nanoseconds);
	}
	{
		Integer64 from = to_Integer64(65536000000000LL);
		integer64_to_internalTime(from, &to);
		TEST_ASSERT_EQUAL_INT32(1L, to.seconds);
		TEST_ASSERT_EQUAL_INT32(0L, to.nanoseconds);
	}
	{
		Integer64 from = to_Integer64(-65536000000000LL);
		integer64_to_internalTime(from, &to);
		TEST_ASSERT_EQUAL_INT32(-1L, to.seconds);
		TEST_ASSERT_EQUAL_INT32(0L, to.nanoseconds);
	}
	{
		Integer64 from = to_Integer64(131072000131072LL);
		integer64_to_internalTime(from, &to);
		TEST_ASSERT_EQUAL_INT32(2L, to.seconds);
		TEST_ASSERT_EQUAL_INT32(2L, to.nanoseconds);
	}
	{
		Integer64 from = to_Integer64(-131072000131072LL);
		integer64_to_internalTime(from, &to);
		TEST_ASSERT_EQUAL_INT32(-2L, to.seconds);
		TEST_ASSERT_EQUAL_INT32(-2L, to.nanoseconds);
	}
	/* Positive largest TimeInterval */
	{
		Integer64 from = to_Integer64(TIME_INTERVAL_MAX);
		integer64_to_internalTime(from, &to);
		TEST_ASSERT_EQUAL_INT32(TIME_INTERNAL_MAX_SEC, to.seconds);
		TEST_ASSERT_EQUAL_INT32(TIME_INTERNAL_MAX_NSEC, to.nanoseconds);
	}
	/* Negative largest TimeInterval */
	{
		Integer64 from = to_Integer64(TIME_INTERVAL_MIN);
		integer64_to_internalTime(from, &to);
		TEST_ASSERT_EQUAL_INT32(TIME_INTERNAL_MIN_SEC, to.seconds);
		TEST_ASSERT_EQUAL_INT32(TIME_INTERNAL_MIN_NSEC, to.nanoseconds);
	}
	/* Fractional nanoseconds are not considered */
	{
		Integer64 from = to_Integer64(TIME_INTERVAL_MAX + 1);
		integer64_to_internalTime(from, &to);
		TEST_ASSERT_EQUAL_INT32(TIME_INTERNAL_MAX_SEC, to.seconds);
		TEST_ASSERT_EQUAL_INT32(TIME_INTERNAL_MAX_NSEC, to.nanoseconds);
	}
	/* Fractional nanoseconds are not considered */
	{
		Integer64 from = to_Integer64(TIME_INTERVAL_MIN - 1);
		integer64_to_internalTime(from, &to);
		TEST_ASSERT_EQUAL_INT32(TIME_INTERNAL_MIN_SEC, to.seconds);
		TEST_ASSERT_EQUAL_INT32(TIME_INTERNAL_MIN_NSEC, to.nanoseconds);
	}
}

void test_TimeInternal_to_Timestamp(void)
{
	Timestamp to;
	{
		TimeInternal from = {0, 0};
		fromInternalTime(&from, &to);
		TEST_ASSERT_EQUAL_UINT48(0ULL, to.secondsField);
		TEST_ASSERT_EQUAL_UINT32(0UL, to.nanosecondsField);
	}
	{
		TimeInternal from = {0, 1};
		fromInternalTime(&from, &to);
		TEST_ASSERT_EQUAL_UINT48(0ULL, to.secondsField);
		TEST_ASSERT_EQUAL_UINT32(1UL, to.nanosecondsField);
	}
	{
		TimeInternal from = {2, 1};
		fromInternalTime(&from, &to);
		TEST_ASSERT_EQUAL_UINT48(2ULL, to.secondsField);
		TEST_ASSERT_EQUAL_UINT32(1UL, to.nanosecondsField);
	}
	{
		TimeInternal from = {2, 1};
		fromInternalTime(&from, &to);
		TEST_ASSERT_EQUAL_UINT48(2ULL, to.secondsField);
		TEST_ASSERT_EQUAL_UINT32(1UL, to.nanosecondsField);
	}
	{
		/* For this test, 'to` contains some dirty values */
		// Timestamp to = { {UINT32_MAX, UINT16_MAX}, UINT32_MAX };
		memset(&to, 0xFF, sizeof(to));
		TimeInternal from = {-1, 0};
		fromInternalTime(&from, &to);
		TEST_ASSERT_EQUAL_UINT48(UINT64_MAX, to.secondsField);
		TEST_ASSERT_EQUAL_UINT32(UINT32_MAX, to.nanosecondsField);
	}
	{
		memset(&to, 0xFF, sizeof(to));
		TimeInternal from = {1, NSEC_IN_SEC - 1};
		fromInternalTime(&from, &to);
		TEST_ASSERT_EQUAL_UINT48(1ULL, to.secondsField);
		TEST_ASSERT_EQUAL_UINT32(NSEC_IN_SEC - 1, to.nanosecondsField);
	}
	{
		TimeInternal from = {INT32_MAX, NSEC_IN_SEC - 1};
		fromInternalTime(&from, &to);
		TEST_ASSERT_EQUAL_UINT48(UINT64_MAX, to.secondsField);
		TEST_ASSERT_EQUAL_UINT32(NSEC_IN_SEC - 1, to.nanosecondsField);
	}
}

void test_Timestamp_to_TimeInternal(void)
{
	TimeInternal to;
	{
		Timestamp from = {{0L, 0}, 0L};
		toInternalTime(&to, &from);
		TEST_ASSERT_EQUAL_INT32(0L, to.seconds);
		TEST_ASSERT_EQUAL_INT32(0L, to.nanoseconds);
	}
	{
		Timestamp from = {{0L, 0}, 1L};
		toInternalTime(&to, &from);
		TEST_ASSERT_EQUAL_INT32(0L, to.seconds);
		TEST_ASSERT_EQUAL_INT32(1L, to.nanoseconds);
	}
	{
		Timestamp from = {{1L, 0}, 1L};
		toInternalTime(&to, &from);
		TEST_ASSERT_EQUAL_INT32(1L, to.seconds);
		TEST_ASSERT_EQUAL_INT32(1L, to.nanoseconds);
	}
	{
		Timestamp from = {{1L, 0}, 1L};
		toInternalTime(&to, &from);
		TEST_ASSERT_EQUAL_INT32(1L, to.seconds);
		TEST_ASSERT_EQUAL_INT32(1L, to.nanoseconds);
	}
	{
		Timestamp from = {{INT32_MAX, 0}, NSEC_IN_SEC - 1};
		toInternalTime(&to, &from);
		TEST_ASSERT_EQUAL_INT32(INT32_MAX, to.seconds);
		TEST_ASSERT_EQUAL_INT32(NSEC_IN_SEC - 1, to.nanoseconds);
	}
}

int main(void)
{
	UNITY_BEGIN();

	RUN_TEST(test_TimeInternal_to_Integer64);
	RUN_TEST(test_Integer64_to_TimeInternal);
	RUN_TEST(test_TimeInternal_to_Timestamp);
	RUN_TEST(test_Timestamp_to_TimeInternal);

	return UNITY_END();
}
