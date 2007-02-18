/*
 * Copyright (C) 2002 by Peter Simons <simons@cryp.to>.
 * All rights reserved.
 */

#ifndef FILE_SENTRY_HH
#define FILE_SENTRY_HH

// POSIX.1 system headers.
#include <cstdio>

struct file_sentry
    {
    explicit file_sentry(FILE* arg) throw() : file(arg) { }
    ~file_sentry() throw() { if (file != 0) fclose(file); }
    FILE* file;
    };

#endif
