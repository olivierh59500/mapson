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
#include <syslog.h>
#include <assert.h>

#include <myexceptions.h>
#include "mapson.h"

#define SENDMAIL_PATH "/usr/sbin/sendmail"

void
forward_mail(const char * mail, char ** receivers)
{
    FILE *         fh;
    char *         buffer;
    unsigned int   i,
	           length;

    /* Sanity checks. */

    assert(mail != NULL);
    assert(receivers != NULL);
    assert(*receivers != NULL);
    if (!mail || !receivers || !*receivers) {
	THROW(UNKNOWN_FATAL_EXCEPTION);
    }

    /* Construct the command line to call sendmail. */

    for (length = strlen(SENDMAIL_PATH) + 1, i = 0;
	 receivers[i] != NULL;
	 i++)
      length += strlen(receivers[i]) + 1;

    buffer = fail_safe_malloc(length);
    strcpy(buffer, "/usr/sbin/sendmail ");

    for (i = 0; receivers[i] != NULL; i++) {
	strcat(buffer, receivers[i]);
	if (receivers[i+1] != NULL)
	  strcat(buffer, " ");
    }

    /* Open the pipe to sendmail. */

    fh = popen(buffer, "w");
    if (fh == NULL) {
	syslog(LOG_ERR, "Failed to start command '%s': %m", buffer);
	free(buffer);
	THROW(IO_EXCEPTION);
    }
    free(buffer);

    /* Write the mail. */

    TRY {
	fprintf(fh, "From: simons@rhein.de (Peter Simons)\n");
	fprintf(fh, "To: ");
	for (i = 0; receivers[i] != NULL; i++) {
	    fprintf(fh, "%s", receivers[i]);
	    if (receivers[i+1] != NULL)
	      fprintf(fh, ", ");
	    else
	      fprintf(fh, "\n");
	}
	fprintf(fh, "Subject: [mapSoN] Please re-send your mail\n");
	fprintf(fh, "\n");
	fprintf(fh, "%s\n", mail);
    }
    OTHERWISE {
	syslog(LOG_ERR, "I/O error while piping mail to sendmail: %m");
	pclose(fh);
	PASSTHROUGH();
    }

    /* Clean up and exit. */

    pclose(fh);
}
