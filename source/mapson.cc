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
#include "system-error.hh"
#include "extract-addresses.hh"

using namespace std;

int
main(int argc, char * argv[])
try
    {
    // Read the e-mail coming on the standard input stream.

    string body;
    char buffer[1024];
    ssize_t rc;
    for (rc = read(STDIN_FILENO, buffer, sizeof(buffer));
	 rc > 0;
	 rc = read(STDIN_FILENO, buffer, sizeof(buffer)))
	{
	body.append(buffer, rc);
	}
    if (rc < 0)
	throw system_error("Failed to read mail from standard input");

    // Split mail into header and body.

    string::size_type pos = body.find("\n\n");
    if (pos == string::npos)
	throw runtime_error("Malformatted input; expected an RFC822-compliant e-mail.");
    string header = body.substr(0, pos+1);
    body.erase(0, pos+2);

    // Extract the sender addresses from the header.

    addrset_t addresses;
    extract_sender_addresses(header, addresses);

    return 0;
    }
catch(const exception& e)
    {
    fprintf(stderr, "*** RUN-TIME ERROR: %s\n", e.what());
    return 1;
    }
catch(...)
    {
    fprintf(stderr, "*** Caught unknown exception. Aborting.\n");
    return 1;
    }
