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

#include <myexceptions.h>
#include <paths.h>
#include "mapson.h"

static FILE * logfile = NULL;

void
log(const char * fmt, ...)
{
    va_list ap;

    if (logfile == NULL) {
	char * home_dir;
	char * filename;

	home_dir = get_home_directory();
	filename = fail_safe_sprintf("%s/%s/logfile", home_dir,
				     MAPSON_HOME_DIR_PATH);
	logfile = fopen(filename, "a");
	if (logfile == NULL) {
	    THROW(IO_EXCEPTION);
	}
    }

    va_start(ap, fmt);
    fprintf(logfile, "mapson[%u]: ", (unsigned int)getpid());
    vfprintf(logfile, fmt, ap);
    fprintf(logfile, "\n");
    va_end(ap);
}
