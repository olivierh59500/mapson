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
#include <string.h>
#include <assert.h>
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

    fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0600);
    if (fd == -1) {
	log("Failed opening file '%s': %s", filename, strerror(errno));
	THROW(IO_EXCEPTION);
    }

    for (lock_attempts = 0;
	 lock_attempts == 0 || (rc == -1 && (errno == EACCES || errno == EAGAIN) && lock_attempts < 10);
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

    /* Write the mail. */

    rc = write(fd, mail_buffer, strlen(mail_buffer));
    if (rc != strlen(mail_buffer)) {
	log("i/o error while writing mail to file '%s': %s", filename, strerror(errno));
	THROW(IO_EXCEPTION);
    }


    /* That's it. */

    close(fd);
}
