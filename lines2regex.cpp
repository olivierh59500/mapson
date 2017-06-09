/*
 * Copyright (c) 2010-2017 by Peter Simons <simons@cryp.to>.
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
      throw Mapson::system_error(string("Can't open regex db '") +
                        filename + "' for reading");
  }
  fd_sentry sentry(fd);

  struct flock lock;
  lock.l_type   = F_RDLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start  = 0;
  lock.l_len    = 0;
  if (fcntl(fd, F_SETLKW, &lock) != 0)
    throw Mapson::system_error(string("Can't lock file '") + filename + "'");

  // Read the file into memory.

  string data;
  char buffer[1024];
  ssize_t rc;
  for (rc = read(fd, buffer, sizeof(buffer));
       rc > 0;
       rc = read(fd, buffer, sizeof(buffer)))
    data.append(buffer, rc);
  if (rc < 0)
    throw Mapson::system_error(string("Failed to read regex db '") +
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
