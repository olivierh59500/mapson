/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1997 by Peter Simons, Germany
 *      All rights reserved.
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <myexceptions.h>
#include "mapson.h"

/* Escape lines starting with "From " in the body of a mail buffer. */

char *
escape_from_lines(char * buffer)
{
    char *              body,
	 *              new_buffer,
         *              p,
         *              q;
    unsigned int        new_buffer_size, i;

    /* Sanity checks. */

    assert(buffer != NULL);
    if (buffer == NULL) {
        THROW(INVALID_ARGUMENT_EXCEPTION);
    }

    /* Find the beginning of the body. */

    for (body = buffer; *body != '\0'; body++) {
	if (*body == '\n') {
	    if (body[1] == '\n') {
		body += 2;
		break;
	    }
	}
    }

    /* Do we have some work at all? */

    for (i = 0, p = body; *p != '\0'; p++) {
	if (*p == '\n' && strncmp(p+1, "From ", 5) == 0)
	  i++;
    }
    if (i == 0)
      return buffer;

    new_buffer_size = strlen(buffer) + i + 1;
    new_buffer      = fail_safe_malloc(new_buffer_size);

    i = body - buffer;
    memcpy(new_buffer, buffer, i);

    p = body;
    q = new_buffer + i;
    while (*p != '\0') {
	if (*p == '\n' && strncmp(p+1, "From ", 5) == 0) {
	    *q++ = *p++;
	    *q++ = '>';
	}
	else
	  *q++ = *p++;
    }

    return new_buffer;
}
