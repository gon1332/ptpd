/**
 * @file utils.h
 *
 * Copyright (c) 2025 Ioannis Konstantelias
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <stddef.h>

/**
 * @brief Finds the starting address of the container of a member
 *
 * @param ptr Pointer to the member of the struct
 * @param type The type of the container struct
 * @param member The name of the struct member the ptr is provided for
 */
#define CONTAINER_OF(ptr, type, member) ((type *)((char *)(ptr) - offsetof(type, member)))

/**
 * @param a First number to compare
 * @param b Second number to compare
 * @returns The maximum floating number of the two provided
 */
static inline double max_f(double a, double b) { return a > b ? a : b; }

/**
 * @param a First number to compare
 * @param b Second number to compare
 * @returns The maximum unsigned integer of the two provided
 */
static inline uint32_t max_d(uint32_t a, uint32_t b) { return a > b ? a : b; }

static inline double as_ms(double v) { return v * 1e3; }
static inline double as_ns(double v) { return v * 1e9; }
static inline double ms(double v) { return v * 1e-3; }
static inline double us(double v) { return v * 1e-6; }
static inline double ns(double v) { return v * 1e-9; }

#endif /* UTILS_H_ */
