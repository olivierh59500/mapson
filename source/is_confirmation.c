/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1997 by Peter Simons, Germany
 *      All rights reserved.
 */

#include <errno.h>
#include <syslog.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <regex.h>
#include <string.h>

#include <myexceptions.h>
#include "mapson.h"

#define MAX_TRANSFORM_ELEMENTS 8
#define COOKIE_PATTERN "mapSoN-Confirm-Cookie:[ \t]*(.*[^ \t])[ \t]*$"

char *
is_confirmation_mail(char * mail_buffer)
{
    regex_t       preg;
    regmatch_t    pmatch[MAX_TRANSFORM_ELEMENTS];
    char *        cookie;
    int           rc;
    int           len;


    /* Sanity checks. */

    assert(mail_buffer != NULL);
    if (mail_buffer == NULL) {
	THROW(UNKNOWN_FATAL_EXCEPTION);
    }


    /* Let's see whether we find a cookie. */

    rc = regcomp(&preg, COOKIE_PATTERN, REG_EXTENDED | REG_NEWLINE);
    if (rc != 0) {
        THROW(REGEX_EXCEPTION);
    }
    rc = regexec(&preg, mail_buffer, MAX_TRANSFORM_ELEMENTS-1, pmatch, 0);
    if (rc != 0 || preg.re_nsub <= 0) {
	regfree(&preg);
	return NULL;
    }
    len = (pmatch[1]).rm_eo - (pmatch[1]).rm_so;
    if (len <= 0) {
	regfree(&preg);
	return NULL;
    }


    /* We did. Now copy it into a new buffer and return it. */

    TRY {
	cookie = fail_safe_malloc(len+1);
    }
    HANDLE(OUT_OF_MEMORY_EXCEPTION) {
	regfree(&preg);
	PASSTHROUGH();
    }
    OTHERWISE {
	PASSTHROUGH();
    }
    memmove(cookie, mail_buffer + (pmatch[1]).rm_so, len);
    cookie[len] = '\0';
    regfree(&preg);

    return cookie;
}
