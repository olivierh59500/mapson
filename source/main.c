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
#include <syslog.h>

#include <myexceptions.h>
#include "mapson.h"
#include "version.h"

int
main(int argc, char * argv[])
{
    FILE *         fh;
    char *         mail_rescue_filename,
         *         mail_buffer;
    unsigned int   mail_size;
    char           buffer[4096];
    int            rc;


    /* First of all initialize our environment. */

    openlog("mapson", LOG_CONS | LOG_PERROR | LOG_PID, LOG_MAIL);
    mail_rescue_filename = get_mail_rescue_filename();
    fprintf(stderr, "DEBUG: mail rescue file is '%s'.\n", mail_rescue_filename);


    /* Copy the mail into the rescue file. */

    fh = fopen(mail_rescue_filename, "r+");
    if (fh == NULL) {
	syslog(LOG_ERR, "Failed to open mail rescue file '%s': %m",
	       mail_rescue_filename);
	THROW(IO_EXCEPTION);
    }

    for (rc = 1, mail_size = 0; rc != 0; mail_size += rc) {
	rc = fread(buffer, 1, sizeof(buffer), stdin);
	if (rc > 0)
	  fail_safe_fwrite(buffer, 1, rc, fh);

	if (rc == 0 && !feof(stdin)) {
	    syslog(LOG_ERR, "I/O error while reading mail: %m");
	    THROW(IO_EXCEPTION);
	}
    }
    fprintf(stderr, "DEBUG: Mail is %d byte long.\n", mail_size);


    /* Now read the mail into the buffer. */

    if (fseek(fh, 0L, SEEK_SET) != 0) {
	syslog(LOG_ERR, "I/O while reading mail from rescue file '%s': %m",
	       mail_rescue_filename);
	THROW(IO_EXCEPTION);
    }
    mail_buffer = fail_safe_malloc(mail_size+1);
    rc = fread(mail_buffer, 1, mail_size, fh);
    if (rc != mail_size) {
	syslog(LOG_ERR, "I/O while reading mail from rescue file '%s': %m",
	       mail_rescue_filename);
	THROW(IO_EXCEPTION);
    }
    fclose(fh);
    mail_buffer[mail_size] = '\0';
    fprintf(stderr, "DEBUG: Mail text follows:\n%s\n", mail_buffer);


    /* Terminating gracefully. */

    remove(mail_rescue_filename);
    return 0;
}
