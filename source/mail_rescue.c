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
#include <string.h>
#include <errno.h>
#include <pwd.h>
#include <unistd.h>
#include <fcntl.h>
#include <syslog.h>

#include <text.h>

/* Return the path of the current user's home directory. */

char *
get_home_directory(void)
{
    char *             home_dir;
    struct passwd *    pwd;

    pwd = getpwuid(getuid());
    if (pwd == NULL)
      return NULL;

    home_dir = strdup(pwd->pw_dir);

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
    if (home_directory == NULL)
      return NULL;

    for (counter = 0; ; counter++) {
	filename = text_easy_sprintf("%s/mapson_rescue_%04d",
				   home_directory, counter);
	if (filename == NULL)
	  return NULL;
	fprintf(stderr, "DEBUG: Trying rescue file '%s'.\n", filename);

	fd = open(filename, O_WRONLY | O_CREAT | O_EXCL, 0600);
	if (fd == -1) {
	    if (errno == EEXIST)
	      continue;
	    else {
		free(filename);
		syslog(LOG_ERR, "Tried to open file '%s': %m", filename);
		return NULL;
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
