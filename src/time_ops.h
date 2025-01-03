/**
 * @file time_ops.h
 * Defines operations and conversion functions for the time-keeping structures.
 *
 * Copyright (c) 2024 Ioannis Konstantelias
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef TIME_OPS_H_
#define TIME_OPS_H_

#include <stdint.h>
#include <time.h> /* for struct timespec */
#include <sys/time.h> /* for struct timeval */
#include "ptp_datatypes.h" /* for TimeInternal */

#define USEC_IN_SEC  1000000LL
#define NSEC_IN_SEC  1000000000LL
#define NSEC_IN_USEC 1000LL

/** Type of increment passed in ti_inc(). */
enum increment_type {
	INC_SECONDS,
	INC_NANOSECONDS
};

/** Converts TimeInternal to TimeInterval
 * @note TimeInternal does not represent fractional nanoseconds
 * @param[in] from Internal time representation
 * @param[out] to Time in scaled nanoseconds according to 5.3.2
 */
void ti_to_integer64(TimeInternal from, Integer64 *to);

/** Converts TimeInterval to TimeInternal
 * @note TimeInternal does not represent fractional nanoseconds
 * @param[in] from Time in scaled nanoseconds according to 5.3.2
 * @param[out] to Internal time representation
 */
void ti_from_integer64(Integer64 from, TimeInternal *to);

/** Converts TimeInterval to Timestamp
 * @param[in] from Internal time representation
 * @param[out] to Time according to 5.3.3
 */
void ti_to_timestamp(const TimeInternal *from, Timestamp *to);

/** Converts Timestamp to TimeInterval
 * @param[in] from Time according to 5.3.3
 * @param[out] to Internal time representation
 */
void ti_from_timestamp(const Timestamp *from, TimeInternal *to);

/** Converts struct timespec to TimeInternal
 * @param[in] from Time in struct timespec
 * @param[out] to Internal time representation
 */
void ti_from_timespec(const struct timespec *from, TimeInternal *to);

/** Converts struct timespec to TimeInternal
 * @param[in] from Internal time representation
 * @param[out] to Time in struct timespec
 */
// void InternalTime_to_ts(const TimeInternal *from, struct timespec *to);
void ti_to_timespec(const TimeInternal *from, struct timespec *to);

/** Converts struct timeval to TimeInternal
 * @param[in] from Time in struct timeval
 * @param[out] to Internal time representation
 */
void ti_from_timeval(const struct timeval *from, TimeInternal *to);

/** Converts struct timeval to TimeInternal
 * @param[in] from Internal time representation
 * @param[out] to Time in struct timeval
 */
void ti_to_timeval(const TimeInternal *from, struct timeval *to);

/** Adds two TimeInternal
 * @param[out] out The result
 * @param[in] x First TimeInternal operand
 * @param[in] y Second TimeInternal operand
 */
void ti_add(TimeInternal *out, const TimeInternal *x, const TimeInternal *y);

/** Subtracts two TimeInternal
 * @param[out] out The result
 * @param[in] x First TimeInternal operand
 * @param[in] y Second TimeInternal operand
 */
void ti_sub(TimeInternal *out, const TimeInternal *x, const TimeInternal *y);

/** Divides the time given by 2
 * @note It's an in-place function
 * @param[inout] t Time to divide
 */
void ti_div2(TimeInternal *t);

/** Clears the time given
 * @param[out] t Time to set to zero
 */
void ti_clear(TimeInternal *t);

/**
 * @param[in] t Time to test
 * @returns 1 if time is negative, otherwise 0
 */
int ti_is_negative(const TimeInternal *t);

/**
 * @param[in] t Time to test
 * @returns 1 if time is zero, otherwise 0
 */
int ti_is_zero(const TimeInternal *t);

/** Compares two TimeInternal values
 * @param[in] x First time to compare
 * @param[in] y Second time to compare
 * @retval 0 if equal
 * @retval positive if x greater than y
 * @retval negative if x less than y
 */
int ti_cmp(const TimeInternal *x, const TimeInternal *y);

/** Applies an increment to a TimeInternal
 * @param[inout] t The time to be incremented
 * @param inc The increment (can be positive or negative)
 * @param type Interpretation of the increment, see enum increment_type
 */
void ti_inc(TimeInternal *t, int32_t inc, enum increment_type type);

/** Removes sign from variable
 * @param[inout] t The time to be sign neutral
 */
void ti_abs(TimeInternal *t);

/** Checks how close two times are in nanoseconds
 * @param[in] x First time to compare
 * @param[in] y Second time to compare
 * @param error_ns The accepted error in nanoseconds
 * @returns 1 if the difference of x and y is less/equal than/to error_ns, otherwise 0
 */
int ti_is_close(const TimeInternal *x, const TimeInternal *y, int error_ns);

//// NOT UNIT TESTED YET -----v

int check_timestamp_is_fresh2(const TimeInternal *timeA, const TimeInternal *timeB);

double timeInternalToDouble(const TimeInternal *p);

TimeInternal doubleToTimeInternal(double d);

/**
 * @param[in] t The time to test
 * @returns the amount of seconds contained in t
 */
int32_t ti_seconds(const TimeInternal *t);

#if 0
int check_timestamp_is_fresh(const TimeInternal * timeA);

double secondsToMidnight(void);

double getPauseAfterMidnight(Integer8 announceInterval, int pausePeriod);

/* FNV-1 hash, 32-bit, optional modulo limiter */
uint32_t fnvHash(void *input, size_t len, int modulo);

#endif /* 0 */

#endif /* TIME_OPS_H_ */
