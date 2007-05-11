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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>

// My own libraries.
#include "system-error.hpp"
#include "fd-sentry.hpp"
#include "lines2regex.hpp"

using namespace std;

string lines2regex(string const & filename)
{
  // Open the file and lock it for reading.

  int fd = open(filename.c_str(), O_RDONLY, 0);
  if (fd < 0)
  {
    if (errno == ENOENT)
      return "";
    else
      throw system_error(string("Can't open regex db '") +
                        filename + "' for reading");
  }
  fd_sentry sentry(fd);

  struct flock lock;
  lock.l_type   = F_RDLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start  = 0;
  lock.l_len    = 0;
  if (fcntl(fd, F_SETLKW, &lock) != 0)
    throw system_error(string("Can't lock file '") + filename + "'");

  // Read the file into memory.

  string data;
  char buffer[1024];
  ssize_t rc;
  for (rc = read(fd, buffer, sizeof(buffer));
       rc > 0;
       rc = read(fd, buffer, sizeof(buffer)))
    data.append(buffer, rc);
  if (rc < 0)
    throw system_error(string("Failed to read regex db '") +
                      filename + "' into memory");

  // Walk through the lines and compile the regexes.

  string regex;
  string::size_type cur, next;
  for (cur = 0; cur != string::npos && cur < data.length(); cur = next+1)
  {
    next = data.find("\r\n", cur);
    if (next == string::npos)
      next = data.find('\n', cur);

    if (next != string::npos && next - cur > 0)
    {
      regex.append("|").append(data.substr(cur, next-cur));
    }
  }
  if (!regex.empty())
  {
    regex.erase(0, 1);
    regex = "(" + regex + ")";
  }
  return regex;
}
