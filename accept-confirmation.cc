/*
 * Copyright (C) 2002 by Peter Simons <simons@computer.org>.
 * All rights reserved.
 */

// ISO C++ headers.
#include <vector>

// POSIX.1 system headers.
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

// My own libraries.
#include "system-error/system-error.hh"
#include "log.hh"
#include "config.hh"
#include "fd-sentry.hh"
#include "deliver.hh"
#include "accept-confirmation.hh"

using namespace std;

inline string::size_type find_cookie(const string& buffer, string::size_type pos)
    {
    for(size_t hits_in_a_row = 0; pos < buffer.size(); ++pos)
        {
        if ((buffer[pos] >= 'a' && buffer[pos] <= 'f')
            || (buffer[pos] >= 'A' && buffer[pos] <= 'F')
            || (buffer[pos] >= '0' && buffer[pos] <= '9'))
            ++hits_in_a_row;
        else
            hits_in_a_row = 0;
        if (hits_in_a_row == 32)
            return pos - 31;
        }
    return string::npos;
    }

inline void deliver_approved_mail(const string& filename, const string& cookie, string& mail)
    {
    int fd = open(filename.c_str(), O_RDONLY, 0);
    if (fd > 0)
        {
        fd_sentry s(fd);

        info("Incoming e-mail '%s' contained cookie '%s'; delivering the corresponding spooled mail.",
             config->message_id.c_str(), cookie.c_str());

        mail.erase();
        char tmp[8*1024];
        ssize_t rc;
        for (rc = read(fd, tmp, sizeof(tmp)); rc > 0; rc = read(fd, tmp, sizeof(tmp)))
            mail.append(tmp, rc);
        if (rc < 0)
            throw system_error(string("Failed to read mail file '") + filename + "'");
        deliver(mail);
        unlink(filename.c_str());
        }
    else
        throw runtime_error(string("Failed to open file '") + filename + "'");
    }

bool accept_confirmation(string& mail, const string& cmdline_cookie)
    {
    // If cookie is set, it has been provided on the command line.
    // Hence, we assume it is for real and skip the finding-process.
    // If the cookie turns out to be wrong, we'll continue as if none
    // had been specified.

    if (!cmdline_cookie.empty())
        {
        debug(("Checking cookie '%s', which was specified on the command line.", cmdline_cookie.c_str()));
        struct stat sbuf;
        string filename = config->spool_dir + "/" + cmdline_cookie;
        if (stat(filename.c_str(), &sbuf) == 0)
            {
            deliver_approved_mail(filename, cmdline_cookie, mail);
            return true;
            }
        else
            debug(("Cookie was not correct. Continuing to searching."));
        }

    // Find all strings in the mail that could be cookie and check,
    // whether a mail has been spooled under that name. If, deliver
    // that mail and return.

    for (string::size_type i = 0; ; ++i)
        {
        i = find_cookie(mail, i);
        if (i != string::npos)
            {
            string cookie = mail.substr(i, 32);
            debug(("Found potential cookie '%s' at mail offset %u", cookie.c_str(), i));

            struct stat sbuf;
            string filename = config->spool_dir + "/" + cookie;
            if (stat(filename.c_str(), &sbuf) == 0)
                {
                deliver_approved_mail(filename, cookie, mail);
                return true;
                }
            }
        else
            break;
        }

    debug(("E-mail does not contain a valid cookie; continue processing."));
    return false;
    }
