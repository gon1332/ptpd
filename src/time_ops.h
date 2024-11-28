/**
 * Copyright (c) 2015-2024 Ioannis Konstantelias,
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

/** Converts TimeInternal to TimeInterval
 * @note TimeInternal does not represent fractional nanoseconds.
 * @param[in] from Time in seconds and nanoseconds
 * @param[out] to Time in scaled nanoseconds according to 5.3.2
 */
void internalTime_to_integer64(TimeInternal from, Integer64 *to);

/** Converts TimeInterval to TimeInternal
 * @note TimeInternal does not represent fractional nanoseconds.
 * @param[in] from Time in scaled nanoseconds according to 5.3.2
 * @param[out] to Time in seconds and nanoseconds
 */
void integer64_to_internalTime(Integer64 from, TimeInternal *to);

/** Converts TimeInterval to Timestamp
 * @param[in] from Time in seconds and nanoseconds
 * @param[out] to Time according to 5.3.3
 */
void fromInternalTime(const TimeInternal *from, Timestamp *to);

/** Converts Timestamp to TimeInterval
 * @param[out] to Time in seconds and nanoseconds
 * @param[in] from Time according to 5.3.3
 */
void toInternalTime(TimeInternal *to, const Timestamp *from);

#if 0
void ts_to_InternalTime(const struct timespec *a,  TimeInternal * b);
void tv_to_InternalTime(const struct timeval *a,  TimeInternal * b);
void normalizeTime(TimeInternal * r);
void addTime(TimeInternal * r, const TimeInternal * x, const TimeInternal * y);
void subTime(TimeInternal * r, const TimeInternal * x, const TimeInternal * y);

/// Divide an internal time value
///
/// @param r the time to convert
/// @param divisor
///

#if 0
/* TODO: this function could be simplified, as currently it is only called to halve the time */
void divTime(TimeInternal *r, int divisor);
#endif

void div2Time(TimeInternal *r);

/* clear an internal time value */
void clearTime(TimeInternal *r);

/* sets a time value to a certain nanoseconds */
void nano_to_Time(TimeInternal *x, int nano);

/* greater than operation */
int gtTime(const TimeInternal *x, const TimeInternal *y);

/* remove sign from variable */
void absTime(TimeInternal *r);

/* if 2 time values are close enough for X nanoseconds */
int is_Time_close(const TimeInternal *x, const TimeInternal *y, int nanos);

int check_timestamp_is_fresh2(const TimeInternal * timeA, const TimeInternal * timeB);

int check_timestamp_is_fresh(const TimeInternal * timeA);

int isTimeInternalNegative(const TimeInternal * p);

double secondsToMidnight(void);

double getPauseAfterMidnight(Integer8 announceInterval, int pausePeriod);

double timeInternalToDouble(const TimeInternal * p);

TimeInternal doubleToTimeInternal(const double d);

/* FNV-1 hash, 32-bit, optional modulo limiter */
uint32_t fnvHash(void *input, size_t len, int modulo);

#endif /* 0 */

#endif /* TIME_OPS_H_ */
