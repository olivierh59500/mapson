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
#include <paths.h>
#include "mapson.h"

static char default_mail_text[] = \
"Hi,\n" \
"\n" \
"a few seconds ago I have received your electronic mail. I notice that\n" \
"this is the first time you're trying to contact me via e-mail, and I\n" \
"have a little procedure set up that protects me from unwanted junk\n" \
"mail. This is why the mapSoN tool has delayed the delivery until it\n" \
"receives a confirmation from you, by which you certify that you're NOT\n" \
"sending me any unsolicted commercial stuff.\n" \
"\n" \
"To confirm this, simply reply to this automatic e-mail and make sure\n" \
"you include the following line in your reply:\n" \
"\n" \
"%s\n" \
"\n" \
"You may quote the line, if that's more comfortable for you, the mapSoN\n" \
"tool will recognize the confirmation anyway.\n" \
"\n" \
"\n" \
"I am sorry for the extra effort, but unfortunately the amount of junk\n" \
"mail I receive has made this preparative necessary. My tool keeps\n" \
"track of who has sent me e-mail before and you won't see this notice\n" \
"again.\n" \
"\n";

void
send_request_for_confirmation_mail(char * recipient, char * cookie)
{
    FILE *         fh;
    char *         buffer;
    char *         mail_text;
    char *         cookie_buffer;
    char *         home_dir;

    /* Sanity checks. */

    assert(recipient != NULL);
    assert(cookie != NULL);
    if (!recipient || !cookie) {
	THROW(UNKNOWN_FATAL_EXCEPTION);
    }

    /* Create the cookie. */

    cookie_buffer = fail_safe_sprintf("mapSoN-Confirm-Cookie: %s", cookie);

    /* Load the mail's text. */

    home_dir = get_home_directory();
    buffer = fail_safe_sprintf("%s/%s", home_dir, MAPSON_RFCTEXT_PATH);
    free(home_dir);
    TRY {
	mail_text = loadfile(buffer);
    }
    HANDLE(IO_EXCEPTION) {
	syslog(LOG_WARNING, "Failed to open file '%s': %m", buffer);
	syslog(LOG_WARNING, "Using default text.");
	mail_text = fail_safe_strdup(default_mail_text);
    }
    OTHERWISE {
	free(buffer); free(cookie_buffer);
	PASSTHROUGH();
    }
    free(buffer);

    /* Send the mail back to the originator. */

    buffer = fail_safe_sprintf("%s %s", SENDMAIL_PATH, recipient);
    fh = popen(buffer, "w");
    if (fh == NULL) {
	syslog(LOG_ERR, "Failed to start command '%s': %m", buffer);
	free(buffer); free(mail_text); free(cookie_buffer);
	THROW(IO_EXCEPTION);
    }

    fprintf(fh, "To: %s\n", recipient);
    fprintf(fh, "Subject: [mapSoN] Request for Confirmation\n");
    fprintf(fh, "Precedence: junk\n");
    fprintf(fh, "\n");
    fprintf(fh, mail_text, cookie_buffer);

    /* Clean up and exit. */

    free(cookie_buffer);
    free(buffer);
    free(mail_text);
    pclose(fh);
}
