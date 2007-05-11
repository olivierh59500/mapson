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

#ifndef FD_SENTRY_HPP
#define FD_SENTRY_HPP

#include <unistd.h>             // POSIX.1: close(2)

struct fd_sentry
{
  explicit fd_sentry(int arg) throw() : fd(arg) { }
  ~fd_sentry() throw() { if (fd >= 0) close(fd); }
  int fd;
};

#endif
