/*
 * Copyright (C) 2001 by Peter Simons <simons@computer.org>.
 * All rights reserved.
 */

// ISO C++ headers.
#include <cstdio>
#include <string>

// POSIX.1 system headers.
#include <sys/types.h>
#include <unistd.h>

// My own libraries.
#include "RegExp/RegExp.hh"
#include "system-error.hh"
#include "rfc822mail.hh"

int
main(int argc, char * argv[])
try
    {
    // Read the e-mail coming on the standard input stream.

    rfc822mail mail;
    {
    string raw_mail;
    char buffer[1024];
    ssize_t rc;
    for (rc = read(STDIN_FILENO, buffer, sizeof(buffer));
	 rc > 0;
	 rc = read(STDIN_FILENO, buffer, sizeof(buffer)))
	{
	raw_mail.append(buffer, rc);
	}
    if (rc < 0)
	throw system_error("Failed to read mail from standard input");
    mail = raw_mail;
    }

    return 0;
    }
catch(const exception& e)
    {
    fprintf(stderr, "RUN-TIME ERROR: %s\n", e.what());
    return 1;
    }
catch(...)
    {
    fprintf(stderr, "Caught unknown exception. Aborting.\n");
    return 1;
    }
