/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1997 by Peter Simons, Germany
 *      All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <myexceptions.h>
#include "mapson.h"

array_t
build_array(void)
{
    array_t   state;

    /* Initialize state structure and array. */

    state             = fail_safe_malloc(sizeof(struct array_state));
    state->array_size = 8;
    state->array_pos  = 0;
    state->array      = fail_safe_malloc(state->array_size * sizeof(char *));
    (state->array)[0] = NULL;

    return state;
}


void
append_to_array(array_t state, char * string)
{
    /* Sanity checks. */

    assert(state != NULL);
    if (state == NULL) {
	THROW(UNKNOWN_FATAL_EXCEPTION);
    }

    /* Resize the array if necessary. */

    if (state->array_size <= state->array_pos) {
	state->array_size += 8;
	state->array = fail_safe_realloc(state->array, state->array_size * sizeof( char *));
    }

    /* Store a copy of the string. */

    (state->array)[state->array_pos++] = string;
    (state->array)[state->array_pos]   = NULL;
}

char **
get_array(array_t state)
{
    /* Sanity checks. */

    assert(state != NULL);
    if (state == NULL) {
	THROW(UNKNOWN_FATAL_EXCEPTION);
    }

    return state->array;
}

void
free_array(char ** array)
{
    unsigned int   i;

    /* Sanity checks. */

    assert(array != NULL);
    if (array == NULL) {
	THROW(UNKNOWN_FATAL_EXCEPTION);
    }

    for (i = 0; array[i] != NULL; i++)
      free(array[i]);

    free(array);
}
