/**
 * @file list.h
 * Implements a double-linked circular list with a sentinel.
 *
 * Copyright (c) 2025 Ioannis Konstantelias
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef LIST_H_
#define LIST_H_

#include <stdbool.h>
#include <stdlib.h>

struct list
{
	struct list *next;
	struct list *prev;
};

static inline void
list_create(struct list *head)
{
	head->next = head;
	head->prev = head;
}

static inline void
list_add(struct list *head, struct list *new)
{
	new->next = head->next;
	new->prev = head;
	head->next->prev = new;
	head->next = new;
}

static inline void
list_del(struct list *entry)
{
	entry->prev->next = entry->next;
	entry->next->prev = entry->prev;
	entry->next = NULL;
	entry->prev = NULL;
}

static inline bool
list_is_empty(struct list *head)
{
	return head->next == head && head->prev == head;
}

#endif /* LIST_H_ */
