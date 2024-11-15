/*-
 * Copyright (c) 2024      Ioannis Konstantelias,
 * Copyright (c) 2015      Wojciech Owczarek,
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

/**
 * @file   timer.c
 * @date   Wed Oct 1 00:41:26 2014
 *
 * @brief  Common code for the EventTimer object
 *
 * Creation and deletion plus maintaining a linked list of
 * all created instances.
 */

#include "../ptpd.h"

/* linked list - so that we can control all registered objects centrally */
static EventTimer *_first = NULL;
static EventTimer *_last = NULL;

EventTimer
*createEventTimer(const char* id)
{

	EventTimer *timer;

        if ( !(timer = calloc (1, sizeof(EventTimer))) ) {
            return NULL;
        }


	setupEventTimer(timer);

        strncpy(timer->id, id, EVENTTIMER_MAX_DESC);

	/* maintain the linked list */

	if(_first == NULL) {
		_first = timer;
	}

	if(_last != NULL) {
	    timer->_prev = _last;
	    timer->_prev->_next = timer;
	}

	_last = timer;

	timer->_first = _first;

	DBGV("created itimer eventtimer %s\n", timer->id);

        return timer;
}

void
freeEventTimer
(EventTimer **timer)
{
	if(timer == NULL) {
	    return;
	}

	EventTimer *ptimer = *timer;

	if(ptimer == NULL) {
	    return;
	}

	ptimer->shutdown(ptimer);

	/* maintain the linked list */

	if(ptimer->_prev != NULL) {

		if(ptimer == _last) {
			_last = ptimer->_prev;
		}

		if(ptimer->_next != NULL) {
			ptimer->_prev->_next = ptimer->_next;
		} else {
			ptimer->_prev->_next = NULL;
		}
	/* last one */
	} else if (ptimer->_next == NULL) {
		_first = NULL;
	}

	if(ptimer->_next != NULL) {

		if(ptimer == _first) {
			_first = ptimer->_next;
		}

		if(ptimer->_prev != NULL) {
			ptimer->_next->_prev = ptimer->_prev;
		} else {
			ptimer->_next->_prev = NULL;
		}

	}

	if(*timer != NULL) {
	    free(*timer);
	}

	*timer = NULL;

}
