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
#include "spool.hh"
#include "deliver.hh"
#include "request-confirmation.hh"
#include "gather-addresses.hh"

using namespace std;

int main(int argc, char** argv)
try
    {
    // Create our configuration.

    config = new configuration(argc, argv);

    // Log the fact we're here.

    debug(("mapSoN verion 0.0 (%s %s) starting up", __DATE__, __TIME__));
    config->dump();

    // If we have parameters left on the command line that were no
    // options, we go into "gather addresses"-mode.

    if (config->parameter_index < argc)
        {
        gather_addresses(argc - config->parameter_index, argv + config->parameter_index);
        return 0;
        }

    // OK, normal mode of operation. First of all, initialize our
    // environment.

    AddressDB address_db(config->address_db);

    // We have to decide whether to accept the mail or not. Read the
    // e-mail coming on the standard input stream.

    string mail;
    char buffer[8*1024];
    ssize_t rc;
    for (rc = read(STDIN_FILENO, buffer, sizeof(buffer));
	 rc > 0;
	 rc = read(STDIN_FILENO, buffer, sizeof(buffer)))
	{
	mail.append(buffer, rc);
	}
    if (rc < 0)
	throw system_error("Failed to read mail from standard input");

    // Extract the sender addresses from the mail and copy them into
    // an addrset_t for easier handling in the code that follows. This
    // will also filter out duplicates automatically.

    mail_addresses addresses = extract_sender_addresses(mail);
    addrset_t all_addresses;
    if (!addresses.envelope.empty())
        all_addresses.insert(addresses.envelope);
    if (!addresses.sender.empty())
        all_addresses.insert(addresses.sender);
    if (!addresses.return_path.empty())
        all_addresses.insert(addresses.return_path);
    for (addrset_t::const_iterator i = addresses.from.begin(); i != addresses.from.end(); ++i)
        all_addresses.insert(*i);
    for (addrset_t::const_iterator i = addresses.reply_to.begin(); i != addresses.reply_to.end(); ++i)
        all_addresses.insert(*i);

    // Make sure we found any addresses at all.

    if (all_addresses.empty())
        throw runtime_error("Couldn't extract any addresses from the incoming mail.");

    // Check whether any of the addresses we found is already in the
    // database.

    bool had_a_hit = false;
    for (addrset_t::const_iterator i = all_addresses.begin(); i != all_addresses.end(); )
        {
	if (address_db.find(*i))
            {
            debug(("The address '%s' is already in the database.", i->c_str()));
	    had_a_hit = true;
            addrset_t::const_iterator tmp = i;
            ++tmp;
            all_addresses.erase(i);
            i = tmp;
            }
        else
            {
            debug(("The address '%s' is unknown.", i->c_str()));
            ++i;
            }
        }

    if (had_a_hit)
	{
	// Add all addresses to the database that are not in it
	// already.

        for (addrset_t::const_iterator i = all_addresses.begin(); i != all_addresses.end(); ++i)
            {
            debug(("Adding address '%s' to the database.", i->c_str()));
            address_db.insert(*i);
            }

        // Deliver the mail.

        deliver(mail);
	}
    else
        {
        // Spool the mail.

        string hash = spool(mail);
        request_confirmation(mail, hash, addresses);
        }

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
