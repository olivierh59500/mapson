/*
 * Copyright (c) 2002 by Peter Simons <simons@ieee.org>.
 * All rights reserved.
 */

// POSIX.1 system headers.
#include <stdio.h>
#include <stdarg.h>
#include <syslog.h>

// My own libraries.
#include "config.hh"
#include "log.hh"

namespace
    {
    struct init_logging
	{
	explicit init_logging()
	    {
	    openlog("mapson", LOG_CONS | LOG_PID, LOG_MAIL);
	    }
	~init_logging()
	    {
	    closelog();
	    }
	};
    init_logging sentry;        // This global variable will call openlog() transparently.
    }

void _debug(const char* fmt, ...) throw()
    {
    if (config && config->debug)
        {
        va_list ap;
        va_start(ap, fmt);
        vsyslog(LOG_DEBUG, fmt, ap);
        fprintf(stderr, "debug: ");
        vfprintf(stderr, fmt, ap);
        fprintf(stderr, "\n");
        va_end(ap);
        }
    }

void info(const char* fmt, ...) throw()
    {
    va_list ap;
    va_start(ap, fmt);
    vsyslog(LOG_INFO, fmt, ap);
    fprintf(stderr, "info: ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    }

void error(const char* fmt, ...) throw()
    {
    va_list ap;
    va_start(ap, fmt);
    vsyslog(LOG_ERR, fmt, ap);
    fprintf(stderr, "error: ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    }
