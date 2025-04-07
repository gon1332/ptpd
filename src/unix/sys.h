/**
 * @file sys.h
 * Defines the timer API.
 *
 * Copyright (c) 2025 Ioannis Konstantelias
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef SYS_H_
#define SYS_H_

void sleep_for(double duration);

int set_cpu_affinity(int cpu);

#endif /* SYS_H_ */
