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
#include <assert.h>
#include <syslog.h>
#include <errno.h>

#include <myexceptions.h>
#include "mapson.h"

void
save_to(char * mail_buffer, char * filename)
{
    struct flock  lock;
    int           fd;
    unsigned int  lock_attempts;
    int           rc = 0;

    /* Sanity checks. */

    assert(mail_buffer != NULL);
    assert(filename != NULL);
    if (!mail_buffer || !filename) {
	THROW(UNKNOWN_FATAL_EXCEPTION);
    }

    /* Open and lock the file. */

    fd = open(filename, O_WRONLY | O_APPEND | O_CREAT, 0600);
    if (fd == -1) {
	syslog(LOG_ERR, "Failed opening file '%s': %m", filename);
	THROW(IO_EXCEPTION);
    }

    for (lock_attempts = 0, rc = -1;
	 rc == -1 && errno == EACCES && lock_attempts < 10;
	 lock_attempts++, sleep(1)) {
	lock.l_start  = 0;
	lock.l_len    = 0;
	lock.l_pid    = getpid();
	lock.l_type   = F_WRLCK;
	lock.l_whence = SEEK_SET;
	rc = fcntl(fd, F_SETLK, &lock);
    }

    if (rc == -1 && errno != EACCES) {
	syslog(LOG_ERR, "Failed to lock file '%s': %m", filename);
	THROW(IO_EXCEPTION);
    }

    if (lock_attempts >= 10)
      syslog(LOG_WARNING, "Couldn't get exclusive lock on '%s'. Writing nonetheless now.",
	     filename);


    /* Write the mail. */

}
