/**
 * Copyright (c) 2025 Ioannis Konstantelias
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <unity.h>

#include "list.h"

struct item
{
	int datum;
	struct list list;
};

static struct list head;

void
setUp(void)
{
	list_create(&head);
	TEST_ASSERT_EQUAL_PTR(&head, head.next);
	TEST_ASSERT_EQUAL_PTR(&head, head.prev);
	TEST_ASSERT_TRUE(list_is_empty(&head));
}

void
tearDown(void)
{
}

void
test_list(void)
{
	/* [H] */
	/* [H] <=> [1] */
	struct item item1 = {1, NULL};
	list_add(&head, &item1.list);
	TEST_ASSERT_FALSE(list_is_empty(&head));
	TEST_ASSERT_EQUAL_PTR(&item1.list, head.next);
	TEST_ASSERT_EQUAL_PTR(&item1.list, head.prev);

	/* [H] <=> [2] <=> [1] */
	struct item item2 = {2, NULL};
	list_add(&head, &item2.list);
	TEST_ASSERT_FALSE(list_is_empty(&head));
	TEST_ASSERT_EQUAL_PTR(&item2.list, head.next);
	TEST_ASSERT_EQUAL_PTR(&item1.list, head.prev);
	TEST_ASSERT_EQUAL_PTR(&item1.list, item2.list.next);
	TEST_ASSERT_EQUAL_PTR(&head, item2.list.prev);

	/* [H] <=> [3] <=> [2] <=> [1] */
	struct item item3 = {3, NULL};
	list_add(&head, &item3.list);
	TEST_ASSERT_FALSE(list_is_empty(&head));
	TEST_ASSERT_EQUAL_PTR(&item3.list, head.next);
	TEST_ASSERT_EQUAL_PTR(&item1.list, head.prev);
	TEST_ASSERT_EQUAL_PTR(&item2.list, item3.list.next);
	TEST_ASSERT_EQUAL_PTR(&head, item3.list.prev);
	TEST_ASSERT_EQUAL_PTR(&item1.list, item2.list.next);
	TEST_ASSERT_EQUAL_PTR(&item3.list, item2.list.prev);

	/* [H] <=> [3] <=> [2] */
	list_del(&item1.list);
	TEST_ASSERT_FALSE(list_is_empty(&head));
	TEST_ASSERT_EQUAL_PTR(&item3.list, head.next);
	TEST_ASSERT_EQUAL_PTR(&item2.list, head.prev);
	TEST_ASSERT_EQUAL_PTR(&item2.list, item3.list.next);
	TEST_ASSERT_EQUAL_PTR(&head, item3.list.prev);
	TEST_ASSERT_EQUAL_PTR(&head, item2.list.next);
	TEST_ASSERT_EQUAL_PTR(&item3.list, item2.list.prev);

	/* [H] <=> [2] */
	list_del(&item3.list);
	TEST_ASSERT_FALSE(list_is_empty(&head));
	TEST_ASSERT_EQUAL_PTR(&item2.list, head.next);
	TEST_ASSERT_EQUAL_PTR(&item2.list, head.prev);
	TEST_ASSERT_EQUAL_PTR(&head, item2.list.next);
	TEST_ASSERT_EQUAL_PTR(&head, item2.list.prev);

	/* [H] */
	list_del(&item2.list);
	TEST_ASSERT_TRUE(list_is_empty(&head));
	TEST_ASSERT_EQUAL_PTR(&head, head.next);
	TEST_ASSERT_EQUAL_PTR(&head, head.prev);
}

int
main(void)
{
	UNITY_BEGIN();

	RUN_TEST(test_list);

	return UNITY_END();
}
