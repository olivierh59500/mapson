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
