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

// POSIX.1 system headers.
#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <unistd.h>
#include <fcntl.h>

// My own libraries.
#include "system-error.hpp"
#include "file-sentry.hpp"
#include "config.hpp"
#include "log.hpp"

using namespace std;

namespace
{
  file_sentry fileh(0);
}

inline string make_timestamp()
{
  char buf[64];
  time_t tstamp = time(0);
  if (tstamp == static_cast<time_t>(-1))
    throw system_error("time(2) failed");
  struct tm* tmtime = localtime(&tstamp);
  if (tmtime == 0)
    throw system_error("localtime(3) failed");
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tmtime);
  return buf;
}

void init_logging(const char* file)
{
  fileh.file = fopen(file, "a");
  if (fileh.file == 0)
    throw system_error(string("Could not open log file ") + file);

  struct flock lock;
  lock.l_type = F_WRLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start  = 0;
  lock.l_len    = 0;
  if (fcntl(fileno(fileh.file), F_SETLKW, &lock) != 0)
    throw system_error(string("Can't lock file '") + file + "'");
}

void _debug(const char* fmt, ...)
{
  if (config && config->debug)
  {
    va_list ap;
    va_start(ap, fmt);
    string new_fmt = "debug: ";
    new_fmt += fmt;
    new_fmt += "\n";
    vfprintf(stderr, new_fmt.c_str(), ap);
    new_fmt = make_timestamp() + " " + new_fmt;
    if (fileh.file)
      vfprintf(fileh.file, new_fmt.c_str(), ap);
    va_end(ap);
  }
}

void info(const char* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  string new_fmt = "info: ";
  new_fmt += fmt;
  new_fmt += "\n";
  vfprintf(stderr, new_fmt.c_str(), ap);
  new_fmt = make_timestamp() + " " + new_fmt;
  if (fileh.file)
    vfprintf(fileh.file, new_fmt.c_str(), ap);
  va_end(ap);
}

void error(const char* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  string new_fmt = "error: ";
  new_fmt += fmt;
  new_fmt += "\n";
  vfprintf(stderr, new_fmt.c_str(), ap);
  new_fmt = make_timestamp() + " " + new_fmt;
  if (fileh.file)
    vfprintf(fileh.file, new_fmt.c_str(), ap);
  va_end(ap);
}
