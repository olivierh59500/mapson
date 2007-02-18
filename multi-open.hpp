/*
 * Copyright (C) 2002 by Peter Simons <simons@cryp.to>.
 * All rights reserved.
 */

#ifndef MULTI_OPEN_HH
#define MULTI_OPEN_HH

// ISO C++ headers.
#include <string>
#include <cerrno>

// POSIX.1 system headers.
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// My own libraries.
#include "log.hpp"

inline int multi_open(std::string& pathname, int flags, mode_t mode)
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

#endif
