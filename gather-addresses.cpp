/*
 * Copyright (C) 2002 by Peter Simons <simons@cryp.to>.
 * All rights reserved.
 */

// ISO C++ headers.
#include <cstdio>

// POSIX.1 system headers.
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// My own libraries.
#include "sanity/system-error.hpp"
#include "address-db.hpp"
#include "config.hpp"
#include "fd-sentry.hpp"
#include "extract-addresses.hpp"
#include "gather-addresses.hpp"

using namespace std;

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

void gather_addresses(int argc, char** argv)
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
        throw system_error("Can't open file for reading");
      fd_sentry sentry(fd);
      string mail;
      for (rc = read(fd, buffer, sizeof(buffer));
           rc > 0;
           rc = read(fd, buffer, sizeof(buffer)))
      {
        mail.append(buffer, rc);
      }
      if (rc < 0)
        throw system_error("Failed to read from file");

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
