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
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#include <myexceptions.h>

void *
fail_safe_malloc(size_t size)
{
    void *  buf;

    buf = malloc(size);
    if (buf == NULL) {
	THROW(OUT_OF_MEMORY_EXCEPTION);
    }

    return buf;
}

char *
fail_safe_strdup(char * string)
{
    char *  buf;

    buf = strdup(string);
    if (buf == NULL) {
	THROW(OUT_OF_MEMORY_EXCEPTION);
    }

    return buf;
}

char *
fail_safe_sprintf(const char * fmt, ...)
{
    char *    buffer,
	 *    result;
    size_t    buffer_size;
    va_list   ap;
    int       rc;

    /* Sanity checks. */

    assert(fmt != NULL);
    if (fmt == NULL) {
	THROW(UNKNOWN_FATAL_EXCEPTION);
    }

    /* Do the work. */

    va_start(ap, fmt);
    for (buffer_size = 0, result = NULL; 1 == 1; free(buffer)) { /* forever */

	/* Allocate the internal buffer. */

	buffer_size += 8 * 1024;
	buffer = malloc(buffer_size);
	if (buffer == NULL) {
	    THROW(OUT_OF_MEMORY_EXCEPTION);
	}

	/* Format the string into it. */

	rc = vsnprintf(buffer, buffer_size, fmt, ap);

	/* Success? */

	if (rc < buffer_size) {
	    /* Yes. */

	    result = fail_safe_strdup(buffer);
	    free(buffer);
	    goto leave;
	}

	/* No. */
    }

leave:
    va_end(ap);
    return result;
}
