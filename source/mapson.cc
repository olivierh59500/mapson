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
#include "address-db.hh"
#include "home-dir.hh"
#include "process-ack.hh"

using namespace std;

int
main(int argc, char * argv[])
try
    {
    // First of all, initialize our environment.

    AddressDB address_db(get_home_directory() + "/.mapson-address-db");

    // If we have command line parameters, we go into ACK mode.

    if (argc > 1)
	{
	int rc = 0;
	for (int i = 1; i < argc; ++i)
	    {
	    try { process_ack(address_db, argv[i]); }
	    catch(const exception& e)
		{
		fprintf(stderr, "*** RUN-TIME ERROR: %s\n", e.what());
		rc = 2;
		}
	    catch(...)
		{
		fprintf(stderr, "*** Caught unknown exception. Aborting.\n");
		rc = 2;
		}
	    }

	return rc;
	}

    // OK, we have to decide whether to accept the mail or not. Read
    // the e-mail header coming on the standard input stream.

    string header;
    char buffer[8*1024];
    ssize_t rc;
    for (rc = read(STDIN_FILENO, buffer, sizeof(buffer));
	 rc > 0;
	 rc = read(STDIN_FILENO, buffer, sizeof(buffer)))
	{
	header.append(buffer, rc);
	}
    if (rc < 0)
	throw system_error("Failed to read mail from standard input");

    // Extract the sender addresses from the header and find out
    // whether any of the found address is known already.

    addrset_t addresses;
    extract_sender_addresses(header, addresses);
    if (addresses.empty())
	throw runtime_error("Could not extract any valid address from the mail!");
    addrset_t::const_iterator i;
    bool had_a_hit = false;
    for (i = addresses.begin(); i != addresses.end(); )
	{
	if (address_db.find(*i))
	    {
	    addrset_t::const_iterator tmp = i++;
	    addresses.erase(tmp);
	    had_a_hit = true;
	    }
	else
	    ++i;
	}

    if (had_a_hit)
	{
	// Add all addresses to the database that are not in it
	// already.

	for (i = addresses.begin(); i != addresses.end(); ++i)
	    address_db.insert(*i);

	return 0;		// Let the mail pass.
	}
    else
	return 1;		// Don't let the mail pass.
    }
catch(const exception& e)
    {
    fprintf(stderr, "*** RUN-TIME ERROR: %s\n", e.what());
    return 2;
    }
catch(...)
    {
    fprintf(stderr, "*** Caught unknown exception. Aborting.\n");
    return 2;
    }
