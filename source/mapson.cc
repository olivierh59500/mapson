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

using namespace std;

int
main(int argc, char * argv[])
try
    {
    // Initialize our environment.

    AddressDB address_db(assert_mapson_home_dir_exists() + "/address.db");

    // Read the e-mail header coming on the standard input stream.

    string header;
    char buffer[1024];
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

	return 0;		// // Let the mail pass.
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
