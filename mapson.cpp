/*
 * Copyright (C) 2002 by Peter Simons <simons@cryp.to>.
 * All rights reserved.
 */

// ISO C++ headers.
#include <cstdio>
#include <string>

// POSIX.1 system headers.
#include <sys/types.h>
#include <unistd.h>

// My own libraries.
#include "hashcash/hashcash.h"
#undef word
#include "rfc822/rfc822.hpp"
#include "sanity/regexp.hpp"
#include "sanity/system-error.hpp"
#include "extract-addresses.hpp"
#include "config.hpp"
#include "log.hpp"
#include "address-db.hpp"
#include "lines2regex.hpp"
#include "spool.hpp"
#include "deliver.hpp"
#include "request-confirmation.hpp"
#include "gather-addresses.hpp"
#include "accept-confirmation.hpp"

using namespace std;

int main(int argc, char** argv)
try
{
  // Create our configuration.

  config = new configuration(argc, argv);

  // Log the fact we're here.

  debug(("mapSoN version " VERSION " starting up"));
  config->dump();

  // If we have parameters left on the command line that were no
  // options, we go into "gather addresses"-mode.

  if (config->parameter_index < argc)
  {
    gather_addresses(argc - config->parameter_index,
                    argv + config->parameter_index);
    return 0;
  }

  // OK, normal mode of operation. First of all, initialize our
  // environment.

  AddressDB address_db(config->address_db);
  AddressDB hashcash_db(config->hashcash_db);
  AddressDB whoami_db(config->whoami_db);
  RegExp* whitelist = 0;
  {
    string str = lines2regex(config->whitelist_db);
    if (!str.empty())
      whitelist = new RegExp(str);
  }

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

  // Check whether the mail contains a valid cookie. If it does,
  // mail will be replaced with the original e-mail, that was
  // confirmed now. The mail has already been delivered; all we have
  // to do is to add the addresses to the database.

  bool was_confirmation = accept_confirmation(mail, config->cookie);

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
  for (addrset_t::const_iterator i = addresses.from.begin();
       i != addresses.from.end();
       ++i)
    all_addresses.insert(*i);
  for (addrset_t::const_iterator i = addresses.reply_to.begin();
       i != addresses.reply_to.end();
       ++i)
    all_addresses.insert(*i);

  // Make sure we found any addresses at all.

  if (all_addresses.empty())
    throw runtime_error("Couldn't extract any addresses from the incoming mail.");

  // If the mail has a valid HashCash header, it will always be
  // accepted.

  for (addrset_t::const_iterator i = addresses.hashcash.begin();
       i != addresses.hashcash.end();
       ++i)
  {
    int vers;
    char resource[1024], utct[1024];
    int rc = hashcash_parse(i->c_str(),
                           &vers, utct, sizeof(utct)-1, resource, sizeof(resource)-1);

    if (whoami_db.find(resource))
    {
      debug(("HashCash '%s' is issued for us, but is it valid?", i->c_str()));
      int rc = hashcash_check(i->c_str(), resource, time(NULL),
                             config->hc_expiry, config->hc_grace,
                             config->hc_req_bits);
      if (rc > 0)
      {
        if (hashcash_db.find(i->c_str()) == true)
          info("Ignoring duplicate HashCash '%s'", i->c_str());
        else
        {
          info("Accepting valid HashCash '%s'", i->c_str());
          hashcash_db.insert(i->c_str());
          const_cast<configuration*>(config)->accept = true;
          goto hc_done;
        }
      }
    }
    else
      debug(("Ignoring unknown HashCash '%s'", i->c_str()));
  }
 hc_done:

  // Check whether any of the addresses is found in the whitelist.

  for (addrset_t::iterator i = all_addresses.begin();
       i != all_addresses.end();
       ++i)
  {
    if (whitelist && *whitelist == *i)
    {
      info("Address '%s' is in the whitelist", i->c_str());
      const_cast<configuration*>(config)->accept = true;
    }
  }

  // Check whether any of the addresses we found is already in the
  // database. we have to do this even if we already know we're
  // accepting the mail because we we need to filter the addresses
  // out that are already in the database.

  bool had_a_hit = (was_confirmation || config->accept);
  for (addrset_t::iterator i = all_addresses.begin(); i != all_addresses.end();)
  {
    if (address_db.find(*i))
    {
      debug(("The address '%s' is already in the database.", i->c_str()));
      had_a_hit = true;
      addrset_t::iterator tmp = i;
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

    if (config->address_db_auto_add)
    {
      for (addrset_t::const_iterator i = all_addresses.begin();
           i != all_addresses.end();
           ++i)
      {
        info(("Adding address '%s' to the database.", i->c_str()));
        address_db.insert(*i);
      }
    }

    // Deliver the mail.

    if (!was_confirmation)
    {
      info("Letting mail '%s' pass.", config->message_id.c_str());
      deliver(mail);
    }
  }
  else
  {
    // Spool the mail.

    string hash = spool(mail);
    request_confirmation(mail, hash, addresses);
  }

  return 0;
}
catch(const configuration::no_error&)
{
  return 0;
}
catch(const rfc822_syntax_error& e)
{
  error("Syntax error in '%s': %s", (config) ? config->message_id.c_str() : "no-message-id", e.what());
  return (config) ? config->syntax_error_rc : 75;
}
catch(const exception& e)
{
  error("'%s': %s", (config) ? config->message_id.c_str() : "internal", e.what());
  return (config) ? config->syntax_error_rc : 75;
}
catch(...)
{
  error("'%s': Unknown exception; aborting.", (config) ? config->message_id.c_str() : "internal");
  return (config) ? config->syntax_error_rc : 75;
}
