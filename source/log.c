/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1997 by Peter Simons, Germany
 *      All rights reserved.
 */

#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <myexceptions.h>
#include <paths.h>
#include "mapson.h"

static FILE * logfile = NULL;

void
log(const char * fmt, ...)
{
    va_list ap;

    if (logfile == NULL) {
	int           fd;
	unsigned int  lock_attempts;
	int           rc = 0;
	char *        home_dir;
	char *        filename;
	struct flock  lock;

	home_dir = get_home_directory();
	filename = fail_safe_sprintf("%s/%s/logfile", home_dir,
				     MAPSON_HOME_DIR_PATH);

	fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0600);
	if (fd == -1) {
	    log("Failed opening file '%s': %s", filename, strerror(errno));
	    THROW(IO_EXCEPTION);
	}

	for (lock_attempts = 0;
	     lock_attempts == 0 ||
		 (rc == -1 && (errno == EACCES || errno == EAGAIN) && lock_attempts < 10);
	     lock_attempts++) {
	    lock.l_start  = 0;
	    lock.l_len    = 0;
	    lock.l_type   = F_WRLCK;
	    lock.l_whence = SEEK_SET;
	    if (lock_attempts > 0)
	      sleep(1);
	    rc = fcntl(fd, F_SETLK, &lock);
	}

	if (rc == -1 && errno != EACCES) {
	    log("Failed to lock file '%s': %s", filename, strerror(errno));
	    THROW(IO_EXCEPTION);
	}

	if (lock_attempts >= 10)
	  log("Couldn't get exclusive lock on '%s'. Writing nonetheless now.",
	      filename);

	logfile = fdopen(fd, "a");
	if (logfile == NULL) {
	    log("Couldn't re-open '%s': %s", filename, strerror(errno));
	    THROW(IO_EXCEPTION);
	}
    }

    va_start(ap, fmt);
    fprintf(logfile, "mapson[%u]: ", (unsigned int)getpid());
    vfprintf(logfile, fmt, ap);
    fprintf(logfile, "\n");
    va_end(ap);

}
