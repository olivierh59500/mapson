/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1997 by Peter Simons, Germany
 *      All rights reserved.
 */

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <syslog.h>

#include <myexceptions.h>
#include "mapson.h"

char *
loadfile(const char *  filename)
{
    char *   buffer;
    int      fd;
    int      len;
    int      rc;

    /* Sanity checks. */

    assert(filename);
    if (!filename) {
	THROW(UNKNOWN_FATAL_EXCEPTION);
    }

    /* Determine file length. */

    if ((fd = open(filename, O_RDONLY, 0)) == -1) {
	THROW(IO_EXCEPTION);
    }
    if ((len = lseek(fd, 0, SEEK_END)) == -1) {
	THROW(IO_EXCEPTION);
    }
    if ((lseek(fd, 0, SEEK_SET) == -1)) {
	THROW(IO_EXCEPTION);
    }

    /* Allocate memory buffer. */

    buffer = fail_safe_malloc(len+1);

    /* Read file. */

    rc = read(fd, buffer, len);
    if (rc != len) {
	free(buffer);
	close(fd);
	THROW(IO_EXCEPTION);
    }

    buffer[len] = '\0';
    close(fd);
    errno = len;

    return buffer;
}
