/*
 * Copyright (C) 2001 by Peter Simons <simons@computer.org>.
 * All rights reserved.
 */

// POSIX.1 system headers.
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

// My own libraries.
#include "libvarexp/varexp.hh"
#include "system-error/system-error.hh"
#include "log.hh"
#include "config.hh"
#include "extract-addresses.hh"
#include "request-confirmation.hh"

using namespace std;

namespace
    {
    class lookup_t : public varexp::callback_t
        {
      public:
        lookup_t(const string& _mail, const string& _hash, const mail_addresses& _addresses)
                : mail(_mail), hash(_hash), addresses(_addresses)
            {
            // Find the beginning of the mail body; we'll need that
            // information later.

            for (body_pos = 0; body_pos != string::npos; body_pos = find_next_header_line(mail, body_pos))
                {
                if (mail[body_pos] == '\n')
                    {
                    ++body_pos;
                    break;
                    }
                }
            }

      private:
        virtual void operator()(const string& name, string& data)
            {
            if (strcasecmp("md5hash", name.c_str()) == 0)
                data = hash;
            else if (strcasecmp("envelope", name.c_str()) == 0)
                data = addresses.envelope;
            else if (strcasecmp("sender", name.c_str()) == 0)
                data = addresses.sender;
            else if (strcasecmp("return_path", name.c_str()) == 0)
                data = addresses.return_path;
            else if (strcasecmp("header", name.c_str()) == 0)
                data = mail.substr(0, body_pos - 1);
            else if (strcasecmp("body", name.c_str()) == 0)
                data = mail.substr(body_pos);
            else
                throw varexp::undefined_variable();
            }
        virtual void operator()(const string& name, int idx, string& data)
            {
            if (idx >= 0)
                {
                if (strcasecmp("header", name.c_str()) == 0)
                    {
                    for (size_t i = 0, pos = 0; pos != string::npos; pos = find_next_header_line(mail, pos), ++i)
                        {
                        if (mail[pos] == '\n')
                            break;
                        if (i == static_cast<size_t>(idx))
                            {
                            i = find_next_header_line(mail, pos);
                            data = mail.substr(pos, i - pos);
                            debug(("Found header '%s'", data.c_str()));
                            return;
                            }
                        }
                    }
                }
            throw varexp::undefined_variable();
            }
        const string& mail;
        const string& hash;
        const mail_addresses& addresses;
        size_t body_pos;
        };
    }

void request_confirmation(const string& mail, const string& hash, const mail_addresses& addresses)
    {
    lookup_t lookup(mail, hash, addresses);
    string mail_template;

    // Read request-for-confirmation mail template into buffer.

    const string& filename = config->request_for_confirmation_file;
    int fd = open(filename.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd < 0)
	throw system_error(string("Can't open address db '") + filename + "' for reading");

    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start  = 0;
    lock.l_len    = 0;
    if (fcntl(fd, F_SETLKW, &lock) != 0)
	throw system_error(string("Can't lock file '") + filename + "'");

    // Read the file into memory.

    char buffer[1024];
    ssize_t rc;
    for (rc = read(fd, buffer, sizeof(buffer)); rc > 0; rc = read(fd, buffer, sizeof(buffer)))
	mail_template.append(buffer, rc);
    if (rc < 0)
	throw system_error(string("Failed to read request-for-confirmation template '") + filename + "' into memory");

    // Expand variables in the template.

    varexp::unescape(mail_template, mail_template, false);
    varexp::expand(mail_template, mail_template, lookup);
    varexp::unescape(mail_template, mail_template, true);
    debug(("Expanded mail template is:\n%s", mail_template.c_str()));
    }
