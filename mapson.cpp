/*
 * Copyright (c) 2010 by Peter Simons <simons@cryp.to>.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"
#include "mapson.hpp"
#include "rfc822.hpp"
#include "regexp.hpp"
#include "hashcash/hashcash.h"
#undef word

using namespace std;

// ------------------------------------------------------------------------
//                          Gather Address Mode
// ------------------------------------------------------------------------

inline void print_progress(const string& addr)
{
  printf("    %s", addr.c_str());
  for (size_t i = addr.size(); i <= 50; ++i)
    printf(".");
  printf(" ");
}

inline void print_known()
{
  printf("known\n");
}

inline void print_new()
{
  printf("new\n");
}

static void gather_addresses(int argc, char** argv)
{
  AddressDB address_db(config->address_db);

  for (int i = 0; i < argc; ++i)
  {
    printf("%s:\n", argv[i]);
    int fd;
    char buffer[8*1024];
    ssize_t rc;
    try
    {
      // Open the file and read it into memory.

      fd = open(argv[i], O_RDONLY, 0);
      if (fd < 0)
        throw Mapson::system_error("Can't open file for reading");
      fd_sentry sentry(fd);
      string mail;
      for (rc = read(fd, buffer, sizeof(buffer));
           rc > 0;
           rc = read(fd, buffer, sizeof(buffer)))
      {
        mail.append(buffer, rc);
      }
      if (rc < 0)
        throw Mapson::system_error("Failed to read from file");

      // Extract the mail addresses.

      mail_addresses addresses = extract_sender_addresses(mail);

      // Check whether the addresses are known already and add
      // them to the database if not.

      if (!addresses.envelope.empty())
      {
        print_progress(addresses.envelope);
        if (address_db.find(addresses.envelope))
          print_known();
        else
        {
          print_new();
          address_db.insert(addresses.envelope);
        }
      }
      if (!addresses.sender.empty())
      {
        print_progress(addresses.sender);
        if (address_db.find(addresses.sender))
          print_known();
        else
        {
          print_new();
          address_db.insert(addresses.sender);
        }
      }
      if (!addresses.return_path.empty())
      {
        print_progress(addresses.return_path);
        if (address_db.find(addresses.return_path))
          print_known();
        else
        {
          print_new();
          address_db.insert(addresses.return_path);
        }
      }
      for (addrset_t::const_iterator j = addresses.from.begin(); j != addresses.from.end(); ++j)
      {
        print_progress(*j);
        if (address_db.find(*j))
          print_known();
        else
        {
          print_new();
          address_db.insert(*j);
        }
      }
      for (addrset_t::const_iterator j = addresses.reply_to.begin(); j != addresses.reply_to.end(); ++j)
      {
        print_progress(*j);
        if (address_db.find(*j))
          print_known();
        else
        {
          print_new();
          address_db.insert(*j);
        }
      }
    }
    catch(const exception& e)
    {
      printf("    %s\n", e.what());
    }
  }
}

// ------------------------------------------------------------------------
//                          Main Program Driver
// ------------------------------------------------------------------------

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
    throw Mapson::system_error("Failed to read mail from standard input");

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

  for (addrset_t::const_iterator i = addresses.hashcash.begin()
      ; i != addresses.hashcash.end()
      ; ++i)
  {
    int vers;
    int bits;
    char resource[1024], utct[1024];
    int rc = hashcash_parse( i->c_str()
                           , &vers
                           , &bits
                           , utct, sizeof(utct)-1
                           , resource, sizeof(resource)-1
                           , NULL, 0
                           );
    if (rc && whoami_db.find(resource))
    {
      debug(("HashCash '%s' is issued for us, but is it valid?", i->c_str()));
      void ** regex_cache( NULL );
      char * regex_error( NULL );
      time_t stamptime;
      int rc = hashcash_check( i->c_str()
                             , true /* no-case */
                             , resource
                             , regex_cache, &regex_error
                             , TYPE_STR
                             , time(NULL)
                             , config->hc_expiry
                             , config->hc_grace
                             , config->hc_req_bits
                             , &stamptime
                             );
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
        info("Adding address '%s' to the database.", i->c_str());
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
