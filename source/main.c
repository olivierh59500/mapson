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
#include <syslog.h>

#include "proto.h"
#include "version.h"

int
main(int argc, char * argv[])
{
    FILE *         fh;
    char *         mail_rescue_filename;
    unsigned int   mail_size;
    char           buffer[4096];
    int            rc;

    /* First of all initialize our environment. */

    openlog("mapson", LOG_CONS | LOG_PERROR | LOG_PID, LOG_MAIL);
    mail_rescue_filename = get_mail_rescue_filename();
    if (mail_rescue_filename == NULL)
      exit(1);
    else
      fprintf(stderr, "DEBUG: mail rescue file is '%s'.\n", mail_rescue_filename);


    /* Copy the mail into the rescue file. */

    fh = fopen(mail_rescue_filename, "r+");
    if (fh == NULL) {
	syslog(LOG_ERR, "Failed to open mail rescue file '%s': %m",
	       mail_rescue_filename);
	exit(1);
    }

    for (rc = 1, mail_size = 0; rc != 0; mail_size += rc) {
	rc = fread(buffer, 1, sizeof(buffer), stdin);
	fprintf(stderr, "DEBUG: fread() returned '%d'.\n", rc);
	if (rc > 0)
	  fwrite(buffer, 1, rc, fh);

	if (rc == 0 && !feof(stdin)) {
	    syslog(LOG_ERR, "I/O error while reading mail: %m");
	    exit(1);
	}
    }
    fclose(fh);
    fprintf(stderr, "Mail is %d byte long.\n", mail_size);

    return 0;
}
