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

#include <myexceptions.h>
#include "mapson.h"

void
assert_mapson_home_dir_exists(void)
{
    struct stat    sb;
    char *         home_directory;
    char *         mapson_home_dir;
    int            rc;

    /* stat() the directory. */

    home_directory = get_home_directory();
    mapson_home_dir = fail_safe_sprintf("%s/.mapson", home_directory);
    free(home_directory);

    rc = stat(mapson_home_dir, &sb);
    if (rc == 0) {
	free(mapson_home_dir);
	if ((sb.st_mode & S_IFDIR) != 0)
	  return;		/* everything is okay */
	else {
	    syslog(LOG_ERR, "The mapSoN home directory '%s' is not a directory.",
		   mapson_home_dir);
	    THROW(MAPSON_HOME_DIR_EXCEPTION);
	}
    }


    /* See why stat() failed. */

    if (errno != ENOENT) {
	syslog(LOG_ERR, "Couldn't find my home directory '%s': %m",
	       mapson_home_dir);
	free(mapson_home_dir);
	THROW(MAPSON_HOME_DIR_EXCEPTION);
    }


    /* Directory does not exist. So we create it. */

    rc = mkdir(mapson_home_dir, 0700);
    free(mapson_home_dir);
    if (rc == -1) {
	syslog(LOG_ERR, "Failed to create my home directory '%s': %m",
	       mapson_home_dir);
	THROW(MAPSON_HOME_DIR_EXCEPTION);
    }
}
