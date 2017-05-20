/*
 * Copyright (c) 2010 by Peter Simons <simons@cryp.to>.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "mapson.hpp"
#include <cstdarg>

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
    throw Mapson::system_error("time(2) failed");
  struct tm* tmtime = localtime(&tstamp);
  if (tmtime == 0)
    throw Mapson::system_error("localtime(3) failed");
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tmtime);
  return buf;
}

void init_logging(const char* file)
{
  fileh.file = fopen(file, "a");
  if (fileh.file == 0)
    throw Mapson::system_error(string("Could not open log file ") + file);

  struct flock lock;
  lock.l_type = F_WRLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start  = 0;
  lock.l_len    = 0;
  if (fcntl(fileno(fileh.file), F_SETLKW, &lock) != 0)
    throw Mapson::system_error(string("Can't lock file '") + file + "'");
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
