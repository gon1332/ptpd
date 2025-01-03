/**
 * Copyright (c) 2024 Ioannis Konstantelias
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "time_ops.h"
#include <limits.h>
#include <stdlib.h>

#define TIME_INTERVAL_MAX 0x7FFFFFFFFFFF0000LL
#define TIME_INTERVAL_MIN 0x8000000000000000LL

void
ti_to_integer64(TimeInternal from, Integer64 *to)
{
	if (from.nanoseconds > ((int64_t)TIME_INTERVAL_MAX >> 16)) {
		*to = TIME_INTERVAL_MAX;
	} else if (from.nanoseconds < ((int64_t)TIME_INTERVAL_MIN >> 16)) {
		*to = TIME_INTERVAL_MIN;
	} else {
		*to = from.nanoseconds << 16;
	}
}

void
ti_from_integer64(Integer64 from, TimeInternal *to)
{
	to->nanoseconds = from >> 16;
}

void
ti_to_timestamp(const TimeInternal *from, Timestamp *to)
{
	// TODO: How to handle negative TimeInternal?
	uint64_t seconds = from->nanoseconds / NSEC_IN_SEC;
	uint64_t nanoseconds = from->nanoseconds % NSEC_IN_SEC;
	to->secondsField.msb = (seconds >> 32) & 0xFFFF;
	to->secondsField.lsb = seconds & 0xFFFFFFFF;
	to->nanosecondsField = nanoseconds;
}

void
ti_from_timestamp(const Timestamp *from, TimeInternal *to)
{
	to->nanoseconds = from->secondsField.lsb * NSEC_IN_SEC + from->nanosecondsField;
}

void
ti_from_timespec(const struct timespec *from, TimeInternal *to)
{
	to->nanoseconds = from->tv_sec * NSEC_IN_SEC + from->tv_nsec;
}

void
ti_to_timespec(const TimeInternal *from, struct timespec *to)
{
	to->tv_sec = from->nanoseconds / NSEC_IN_SEC;
	to->tv_nsec = from->nanoseconds % NSEC_IN_SEC;
}

void
ti_from_timeval(const struct timeval *from, TimeInternal *to)
{
	to->nanoseconds = from->tv_sec * NSEC_IN_SEC + from->tv_usec * NSEC_IN_USEC;
}

void
ti_to_timeval(const TimeInternal *from, struct timeval *to)
{
	to->tv_sec = from->nanoseconds / NSEC_IN_SEC;
	to->tv_usec = (from->nanoseconds % (int64_t)NSEC_IN_SEC) / NSEC_IN_USEC;
}

void
addTime(TimeInternal *out, const TimeInternal *op_a, const TimeInternal *op_b)
{
	out->nanoseconds = op_a->nanoseconds + op_b->nanoseconds;
}

void
subTime(TimeInternal *out, const TimeInternal *op_a, const TimeInternal *op_b)
{
	out->nanoseconds = op_a->nanoseconds - op_b->nanoseconds;
}

void
ti_div2(TimeInternal *t)
{
	t->nanoseconds /= 2;
}

void
ti_clear(TimeInternal *t)
{
	t->nanoseconds = 0;
}

int
ti_is_negative(const TimeInternal *t)
{
	return t->nanoseconds < 0;
}

int
ti_is_zero(const TimeInternal *t)
{
	return t->nanoseconds == 0;
}

int
ti_cmp(const TimeInternal *x, const TimeInternal *y)
{
	return x->nanoseconds - y->nanoseconds;
}

void
ti_inc(TimeInternal *t, int32_t inc, enum increment_type type)
{
	if (type == INC_SECONDS) {
		t->nanoseconds += (int64_t)inc * NSEC_IN_SEC;
	} else if (type == INC_NANOSECONDS) {
		t->nanoseconds += inc;
	}
}

void
ti_abs(TimeInternal *t)
{
	t->nanoseconds = llabs(t->nanoseconds);
}

int
ti_is_close(const TimeInternal *x, const TimeInternal *y, int error_ns)
{
	TimeInternal diff;
	TimeInternal err = {error_ns};

	subTime(&diff, x, y);
	ti_abs(&diff);

	return ti_cmp(&diff, &err) <= 0;
}

int
check_timestamp_is_fresh2(const TimeInternal *timeA, const TimeInternal *timeB)
{
	int ret;

	// maximum 1 millisecond offset
	ret = ti_is_close(timeA, timeB, 1000000);
	//	DBG2("check_timestamp_is_fresh: %d\n ", ret);
	return ret;
}

double
timeInternalToDouble(const TimeInternal *p)
{
	return ((double)p->nanoseconds) / NSEC_IN_SEC;
}

TimeInternal
doubleToTimeInternal(double d)
{
	TimeInternal t = {d * NSEC_IN_SEC};
	return t;
}

int32_t
ti_seconds(const TimeInternal *t)
{
	return t->nanoseconds / NSEC_IN_SEC;
}
