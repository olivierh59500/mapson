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

#define SENDMAIL_PATH  "/usr/sbin/sendmail"
#define MIME_SEPARATOR "mapSoN_generated_part_separator"

void
send_request_for_confirmation_mail(char * recipient, char * cookie)
{
    FILE *         fh;
    char *         buffer;

    /* Sanity checks. */

    assert(recipient != NULL);
    assert(cookie != NULL);
    if (!recipient || !cookie) {
	THROW(UNKNOWN_FATAL_EXCEPTION);
    }

    /* Send the mail back to the originator. */

    buffer = fail_safe_sprintf("%s %s", SENDMAIL_PATH, recipient);
    fh = popen(buffer, "w");
    if (fh == NULL) {
	syslog(LOG_ERR, "Failed to start command '%s': %m", buffer);
	free(buffer);
	THROW(IO_EXCEPTION);
    }
    free(buffer);

    TRY {
	fprintf(fh, "From: simons@rhein.de (Peter Simons)\n");
	fprintf(fh, "To: %s\n", recipient);
	fprintf(fh, "Subject: [mapSoN] Request for Confirmation\n");
	fprintf(fh, "Precedence: junk\n");
	fprintf(fh, "\n");
	fprintf(fh, "Cookie: %s", cookie);
    }
    OTHERWISE {
	syslog(LOG_ERR, "I/O error while piping mail to sendmail: %m");
	pclose(fh);
	PASSTHROUGH();
    }

    /* Clean up and exit. */

    pclose(fh);
}
