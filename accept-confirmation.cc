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
#include "RegExp/RegExp.hh"
#include "log.hh"
#include "config.hh"
#include "fd-sentry.hh"
#include "deliver.hh"
#include "accept-confirmation.hh"

using namespace std;

bool accept_confirmation(std::string& mail)
    {
    // Find all strings in the mail that match the pattern and check,
    // whether a mail has been spooled under that name. If, deliver
    // that mail and return.

    const RegExp cookie_pattern("[a-f0-9]{32}", REG_EXTENDED | REG_ICASE);
    regmatch_t pmatch[2];

    string::size_type pos = 0;
    while(regexec(cookie_pattern, mail.c_str() + pos, sizeof(pmatch) / sizeof(regmatch_t), pmatch, 0) == 0)
        {
        string cookie = mail.substr(pos + pmatch[0].rm_so, pmatch[0].rm_eo - pmatch[0].rm_so);
        debug(("Found potential cookie '%s' at mail offset %u to offset %u", cookie.c_str(),
               pmatch[0].rm_so, pmatch[0].rm_eo));

        string filename = config->spool_dir + "/" + cookie;
        int fd = open(filename.c_str(), O_RDONLY, 0);
        if (fd >= 0)
            {
            fd_sentry s(fd);

            // We found the mail; now deliver it.

            info("Incoming e-mail contained cookie '%s'; delivering the corresponding spooled mail.", cookie.c_str());
            mail.erase();
            char tmp[8*1024];
            ssize_t rc;
            for (rc = read(fd, tmp, sizeof(tmp));
                 rc > 0;
                 rc = read(fd, tmp, sizeof(tmp)))
                {
                mail.append(tmp, rc);
                }
            if (rc < 0)
                throw system_error(string("Failed to read mail file '") + filename + "'");
            deliver(mail);
            unlink(filename.c_str());
            return true;
            }
        else
            pos += pmatch[0].rm_so + 1;
        }

    debug(("E-mail does not contain a valid cookie; continue processing."));
    return false;
    }
