/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1997 by Peter Simons, Germany
 *      All rights reserved.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <regex.h>

#include <myexceptions.h>
#include "mapson.h"

#ifndef DEBUG_DMALLOC
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

void *
fail_safe_calloc(size_t nmemb, size_t size)
{
    void *  buf;

    buf = calloc(nmemb, size);
    if (buf == NULL) {
	THROW(OUT_OF_MEMORY_EXCEPTION);
    }

    return buf;
}

void *
fail_safe_realloc(void * ptr, size_t size)
{
    void *  buf;

    buf = realloc(ptr, size);
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
#endif /* !defined(DEBUG_DMALLOC) */


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

void
fail_safe_fwrite(void * buffer, size_t size, size_t nmemb, FILE * stream)
{
    int   rc;

    /* Sanity checks. */

    assert(stream != NULL);
    assert(buffer != NULL);
    assert(size != 0 && nmemb != 0);
    if (!stream || !buffer || (size == 0 || nmemb == 0)) {
	THROW(UNKNOWN_FATAL_EXCEPTION);
    }

    rc = fwrite(buffer, size, nmemb, stream);
    if (rc < nmemb) {
	THROW(IO_EXCEPTION);
    }
}

bool
fail_safe_pattern_match(const char * buffer,
			const char * pattern)
{
    regex_t   preg;
    int       rc;

    /* Sanity checks. */

    assert(buffer != NULL);
    assert(pattern != NULL);
    if (!buffer || !pattern) {
	THROW(UNKNOWN_FATAL_EXCEPTION);
    }

    /* Compile the regular expression. */

    rc = regcomp(&preg, pattern, REG_EXTENDED | REG_ICASE | REG_NOSUB | REG_NEWLINE);
    if (rc != 0) {
	THROW(REGEX_EXCEPTION);
    }

    /* Match it. */

    rc = regexec(&preg, buffer, 0, NULL, 0);
    regfree(&preg);

    if (rc == 0)
      return TRUE;
    else
      return FALSE;
}
