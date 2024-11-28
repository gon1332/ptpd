/**
 * Copyright (c) 2024 Ioannis Konstantelias
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "time_ops.h"
#include <limits.h>

void
internalTime_to_integer64(TimeInternal internal, Integer64 *interval)
{
	int64_t scaledNanoseconds;

	scaledNanoseconds = internal.seconds;
	scaledNanoseconds *= 1000000000;
	scaledNanoseconds += internal.nanoseconds;
	scaledNanoseconds <<= 16;

	interval->msb = (scaledNanoseconds >> 32) & 0x00000000ffffffff;
	interval->lsb = scaledNanoseconds & 0x00000000ffffffff;
}

void
integer64_to_internalTime(Integer64 bigint, TimeInternal * internal)
{
	int sign;
	int64_t scaledNanoseconds;

	scaledNanoseconds = bigint.msb;
	scaledNanoseconds <<=32;
	scaledNanoseconds += bigint.lsb;

	/*determine sign of result big integer number*/

	if (scaledNanoseconds < 0) {
		scaledNanoseconds = -scaledNanoseconds;
		sign = -1;
	} else {
		sign = 1;
	}

	/*fractional nanoseconds are excluded (see 5.3.2)*/
	scaledNanoseconds >>= 16;
	internal->seconds = sign * (scaledNanoseconds / 1000000000);
	internal->nanoseconds = sign * (scaledNanoseconds % 1000000000);
}


void
fromInternalTime(const TimeInternal * internal, Timestamp * external)
{

	/*
	 * fromInternalTime is only used to convert time given by the system
	 * to a timestamp.  As a consequence, no negative value can normally
	 * be found in (internal)
	 *
	 * Note that offsets are also represented with TimeInternal structure,
	 * and can be negative, but offset are never convert into Timestamp
	 * so there is no problem here.
	 */

	if ((internal->seconds & ~INT_MAX) ||
	    (internal->nanoseconds & ~INT_MAX)) {
//		DBG("Negative value canno't be converted into timestamp \n");
		return;
	} else {
		external->secondsField.lsb = internal->seconds;
		external->nanosecondsField = internal->nanoseconds;
		external->secondsField.msb = 0;
	}
}

void
toInternalTime(TimeInternal * internal, const Timestamp * external)
{

	/* Program will not run after 2038... */
	if (external->secondsField.lsb < INT_MAX) {
		internal->seconds = external->secondsField.lsb;
		internal->nanoseconds = external->nanosecondsField;
	} else {
//		DBG("Clock servo canno't be executed : "
//		    "seconds field is higher than signed integer (32bits) \n");
		return;
	}
}
