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
#include <fcntl.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <errno.h>
#include <syslog.h>

#include <myexceptions.h>
#include "mapson.h"

/* Return the path of the current user's home directory. */

char *
get_home_directory(void)
{
    char *             home_dir;
    struct passwd *    pwd;

    pwd = getpwuid(getuid());
    if (pwd == NULL) {
	THROW(UNKNOWN_FATAL_EXCEPTION);
    }
    home_dir = fail_safe_strdup(pwd->pw_dir);
    endpwent();
    return home_dir;
}


/* Determine the filename for the rescue file. */

char *
get_mail_rescue_filename(void)
{
    char *   filename;
    char *   home_directory;
    int      counter;
    int      fd;

    home_directory = get_home_directory();

    for (counter = 0; ; counter++) {
	filename = fail_safe_sprintf("%s/mapson_rescue_%04d", home_directory, counter);

	fd = open(filename, O_WRONLY | O_CREAT | O_EXCL, 0600);
	if (fd == -1) {
	    if (errno == EEXIST)
	      continue;
	    else {
		syslog(LOG_ERR, "Tried to open file '%s': %m", filename);
		free(filename);
		THROW(IO_EXCEPTION);
	    }
	    free(filename);
	}
	else
	  break;

    }
    close(fd);
#if 0
    remove(filename);		/* leave the file there, so that no
				   other process will grab it. */
#endif

    return filename;
}
