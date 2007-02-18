/*
 * Copyright (C) 2002 by Peter Simons <simons@cryp.to>.
 * All rights reserved.
 */

#ifndef FD_SENTRY_HH
#define FD_SENTRY_HH

// POSIX.1 system headers.
#include <unistd.h>

struct fd_sentry
    {
    explicit fd_sentry(int arg) throw() : fd(arg) { }
    ~fd_sentry() throw() { if (fd >= 0) close(fd); }
    int fd;
    };

#endif
