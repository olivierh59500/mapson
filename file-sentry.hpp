/*
 * Copyright (c) 2001-2007 Peter Simons <simons@cryp.to>
 *
 * This software is provided 'as-is', without any express or
 * implied warranty. In no event will the authors be held liable
 * for any damages arising from the use of this software.
 *
 * Copying and distribution of this file, with or without
 * modification, are permitted in any medium without royalty
 * provided the copyright notice and this notice are preserved.
 */

#ifndef FILE_SENTRY_HPP
#define FILE_SENTRY_HPP

#include <cstdio>

struct file_sentry
    {
    explicit file_sentry(FILE* arg) throw() : file(arg) { }
    ~file_sentry() throw() { if (file != 0) fclose(file); }
    FILE* file;
    };

#endif
