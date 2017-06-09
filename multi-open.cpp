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

int multi_open(std::string& pathname, int flags, mode_t mode)
{
  debug(("Trying to open one of multiple files '%s'.", pathname.c_str()));

  if (pathname.empty())
  {
    errno = ENOTDIR;
    return -1;
  }

  std::string filename;
  for (std::string::size_type currpos = 0, nextpos; currpos != std::string::npos; currpos = nextpos)
  {
    nextpos = pathname.find(':', currpos);
    if (nextpos == currpos)
    {
      ++nextpos;
      continue;
    }
    else if (nextpos != std::string::npos)
    {
      filename = pathname.substr(currpos, nextpos - currpos);
      ++nextpos;
    }
    else
      filename = pathname.substr(currpos);

    int fd = open(filename.c_str(), flags, mode);
    if (fd >= 0)
    {
      debug(("Successfully opened file '%s'.", filename.c_str()));
      pathname = filename;
      return fd;
    }
    else
      debug(("Failed to open file '%s'.", filename.c_str()));
  }

  debug(("Could not open any file."));
  return -1;
}
