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
#include <string.h>
#include <syslog.h>

#include <rfc822.h>
#include <myexceptions.h>
#include "mapson.h"

#define is_keyword(string) !strncasecmp(p, (string), strlen(string))
static char ** parse_address_line(char * string);

struct Mail *
parse_mail(char * buffer)
{
    struct Mail *    mail_struct;
    char *           p,
	 *           q,
	 *           tmp,
	 *           tmp2;
    char             backup;
    unsigned int     header_size;
    int              rc;


    /* Sanity checks. */

    assert(buffer != NULL);
    if (buffer == NULL) {
	THROW(UNKNOWN_FATAL_EXCEPTION);
    }


    /* Allocate result mail structure. */

    mail_struct = fail_safe_calloc(1, sizeof(struct Mail));


    /* Fill in copy of the header. */

    p = buffer;
    while(*p != '\0') {
	if (*p == '\n' && p[1] == '\n')
	  break;
	p++;
    }

    if (*p == '\0') {
	/* Mail has no body? */
	mail_struct->header = fail_safe_strdup(buffer);
    }
    else {
	header_size = (p - buffer);
	mail_struct->header = fail_safe_malloc(header_size+1);
	memcpy(mail_struct->header, buffer, header_size);
	(mail_struct->header)[header_size] = '\0';
    }


    /* De-continue all lines in the header. */

    for (q = p = mail_struct->header; *p != '\0'; *q++ = *p++) {
	if (*p == '\n' && (p[1] == ' ' || p[1] == '\t')) {
	    p++;
	    *q++ = ' ';
	    while (*p == ' ' || *p == '\t')
	      p++;
	}
    }
    *q = '\0';


    /* Parse the addresses. */

    for (p = mail_struct->header; *p != '\0'; p = q) {

	/* Find beginning of next line first of all. */

	for (q = p; *q != '\0' && *q != '\n'; q++)
	  ;
	if (*q)
	  q++;

	/* What do we have here? */

	if (is_keyword("From ")) {
	    tmp = p + strlen("From ");
	    while (*tmp == ' ')
	      tmp++;
	    tmp2 = strchr(tmp, ' ');
	    if (tmp2 == NULL) {
		THROW(UNKNOWN_FATAL_EXCEPTION);
	    }
	    backup = *tmp2;
	    *tmp2 = '\0';
	    rc = rfc822_parse_address(tmp, &(mail_struct->envelope), NULL, NULL);
	    *tmp2 = backup;
	    if (rc != RFC822_OK) {
		THROW(INVALID_ADDRESS_EXCEPTION);
	    }
	}
	else if (is_keyword("From:")) {
	    tmp = strchr(p, ':');
	    backup = q[-1];
	    q[-1] = '\0';
	    mail_struct->from = parse_address_line(tmp+1);
	    q[-1] = backup;
	}
	else if (is_keyword("To:")) {
	    tmp = strchr(p, ':');
	    backup = q[-1];
	    q[-1] = '\0';
	    mail_struct->to = parse_address_line(tmp+1);
	    q[-1] = backup;
	}
	else if (is_keyword("Cc:")) {
	    tmp = strchr(p, ':');
	    backup = q[-1];
	    q[-1] = '\0';
	    mail_struct->cc = parse_address_line(tmp+1);
	    q[-1] = backup;
	}
	else if (is_keyword("Reply-To:")) {
	    tmp = strchr(p, ':');
	    backup = q[-1];
	    q[-1] = '\0';
	    mail_struct->reply_to = parse_address_line(tmp+1);
	    q[-1] = backup;
	}
    }

    return mail_struct;
}


static char **
parse_address_line(char * string)
{
    struct rfc822_address_sep_state   sep_state;
    array_t                           state;
    char **                           array;
    char *                            p,
	 *                            address,
	 *                            buffer;
    int                               rc;

    /* Sanity checks. */

    assert(string != NULL);
    if (string == NULL) {
	THROW(UNKNOWN_FATAL_EXCEPTION);
    }

    /* Init structures. */

    state = build_array();
    sep_state.address_line = buffer = fail_safe_strdup(string);
    sep_state.group_nest   = 0;

    /* Parse address line. */

    while((p = rfc822_address_sep(&sep_state))) {

	if (*p == '\0')
	  continue;

	rc = rfc822_parse_address(p, &address, NULL, NULL);
	if (rc == RFC822_OK) {
	    append_to_array(state, address);
	}
	else if (rc == RFC822_FATAL_ERROR) {
	    THROW(OUT_OF_MEMORY_EXCEPTION);
	}
	else {
	    syslog(LOG_WARNING, "Failed to parse address '%s'.", p);
	    THROW(INVALID_ADDRESS_EXCEPTION);
	}
    }

    free(buffer);
    array = get_array(state);
    free(state);
    return array;
}
