/*
 * Copyright (c) 2002 by Peter Simons <simons@ieee.org>.
 * All rights reserved.
 */

// ISO C++ headers.
#include <memory>

// POSIX.1 system headers.
#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <unistd.h>
#include <fcntl.h>

// My own libraries.
#include "system-error/system-error.hh"
#include "file-sentry.hh"
#include "config.hh"
#include "log.hh"

using namespace std;

namespace
    {
    auto_ptr<file_sentry> fileh;
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
    strftime(buf, sizeof(buf), "%Y-%m-%d %T", tmtime);
    return buf;
    }

void init_logging(const char* file)
    {
    FILE* fh = fopen(file, "a");
    if (fh == 0)
        throw system_error(string("Could not open log file ") + file);

    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start  = 0;
    lock.l_len    = 0;
    if (fcntl(fileno(fh), F_SETLKW, &lock) != 0)
        throw system_error(string("Can't lock file '") + file + "'");
    fileh = new file_sentry(fh);
    }

void _debug(const char* fmt, ...) throw()
    {
    if (config && config->debug)
        {
        va_list ap;
        va_start(ap, fmt);
        string new_fmt = make_timestamp();
        new_fmt += " debug: ";
        new_fmt += fmt;
        new_fmt += "\n";
        vfprintf(stderr, new_fmt.c_str(), ap);
        if (fileh.get() && fileh->file)
            vfprintf(fileh->file, new_fmt.c_str(), ap);
        va_end(ap);
        }
    }

void info(const char* fmt, ...) throw()
    {
    va_list ap;
    va_start(ap, fmt);
    string new_fmt = make_timestamp();
    new_fmt += " info: ";
    new_fmt += fmt;
    new_fmt += "\n";
    vfprintf(stderr, new_fmt.c_str(), ap);
    if (fileh.get() && fileh->file)
        vfprintf(fileh->file, new_fmt.c_str(), ap);
    va_end(ap);
    }

void error(const char* fmt, ...) throw()
    {
    va_list ap;
    va_start(ap, fmt);
    string new_fmt = make_timestamp();
    new_fmt += " error: ";
    new_fmt += fmt;
    new_fmt += "\n";
    vfprintf(stderr, new_fmt.c_str(), ap);
    if (fileh.get() && fileh->file)
        vfprintf(fileh->file, new_fmt.c_str(), ap);
    va_end(ap);
    }
