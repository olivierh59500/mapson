/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1997 by Peter Simons, Germany
 *      All rights reserved.
 */

#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <myexceptions.h>
#include <paths.h>
#include "mapson.h"

void
assert_mapson_spool_dir_exists(void)
{
    struct stat    sb;
    char *         home_directory;
    char *         mapson_spool_dir;
    int            rc;

    /* stat() the directory. */

    home_directory = get_home_directory();
    mapson_spool_dir = fail_safe_sprintf("%s/%s", home_directory,
					 MAPSON_SPOOL_DIR_PATH);
    free(home_directory);

    rc = stat(mapson_spool_dir, &sb);
    if (rc == 0) {
	free(mapson_spool_dir);
	if ((sb.st_mode & S_IFDIR) != 0)
	  return;		/* everything is okay */
	else {
	    log("The mapSoN spool directory '%s' is not a directory.",
		   mapson_spool_dir);
	    THROW(MAPSON_SPOOL_DIR_EXCEPTION);
	}
    }


    /* See why stat() failed. */

    if (errno != ENOENT) {
	log("Couldn't find my spool directory '%s': %s",
	       mapson_spool_dir, strerror(errno));
	free(mapson_spool_dir);
	THROW(MAPSON_SPOOL_DIR_EXCEPTION);
    }


    /* Directory does not exist. So we create it. */

    rc = mkdir(mapson_spool_dir, 0700);
    free(mapson_spool_dir);
    if (rc == -1) {
	log("Failed to create my spool directory '%s': %s",
	       mapson_spool_dir, strerror(errno));
	THROW(MAPSON_SPOOL_DIR_EXCEPTION);
    }
}
