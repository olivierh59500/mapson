/*
 * Copyright (C) 2002 by Peter Simons <simons@computer.org>.
 * All rights reserved.
 */

// POSIX.1 system headers.
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

// My own libraries.
#include "system-error/system-error.hh"
#include "config.hh"
#include "log.hh"
#include "deliver.hh"

using namespace std;

namespace
    {
    struct fd_sentry
        {
        explicit fd_sentry(int arg) throw() : fd(arg) { }
        ~fd_sentry() throw() { close(fd); }
        int fd;
        };
    }

void deliver(const string& mail)
    {
    // Open the mailbox file and write store the mail there.

    debug(("Delivering mail to mailbox '%s'.", config->mailbox.c_str()));
    int fd = open(config->mailbox.c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd < 0)
	throw system_error(string("Can't open mailbox file '") + config->mailbox + "' for writing");
    fd_sentry sentry(fd);
    for (size_t len = 0; len < mail.size(); )
	{
	ssize_t rc = write(fd, mail.data()+len, mail.size()-len);
	if (rc < 0)
	    throw system_error(string("Failed writing to the mailbox file '") + config->mailbox + "'");
	else
	    len += rc;
	}
    }
