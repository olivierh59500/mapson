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
#include "librfc822/rfc822.hh"
#include "system-error/system-error.hh"
#include "extract-addresses.hh"
#include "config.hh"
#include "log.hh"
#include "address-db.hh"

using namespace std;

int main(int argc, char * argv[])
try
    {
    // Create our configuration.

    config = new configuration;

    // First of all, initialize our environment.

    AddressDB address_db(config->address_db);

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

    mail_addresses addresses = extract_sender_addresses(header);

#if 0
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
#endif
    return 0;
    }
catch(const rfc822_syntax_error& e)
    {
    info("Syntax error in mail: %s", e.what());
    return (config) ? config->syntax_error_rc : 75;
    }
catch(const exception& e)
    {
    error("Runtime error: %s", e.what());
    return (config) ? config->syntax_error_rc : 75;
    }
catch(...)
    {
    error("Caught unknown exception. Aborting.");
    return (config) ? config->syntax_error_rc : 75;
    }
