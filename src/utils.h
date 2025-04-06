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
 * @brief Evaluates to the maximum number of the two provided
 *
 * @param a First number to compare
 * @param b Second number to compare
 */
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#endif /* UTILS_H_ */
