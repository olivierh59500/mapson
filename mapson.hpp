/*
 * Copyright (c) 2001-2007 Peter Simons <simons@cryp.to>
 *
 * This software is provided 'as-is', without any express or
 * implied warranty. In no event will the authors be held liable
 * for any damages arising from the use of this software.
 *
 * Copying and distribution of this file, with or without
 * modification, are permitted in any medium without royalty
 * provided the copyright notice and this notice are preserved.
 */

#ifndef EXTRACT_ADDRESSES_HPP
#define EXTRACT_ADDRESSES_HPP

#include <set>
#include <string>

typedef std::set<std::string> addrset_t;
struct mail_addresses
{
  std::string envelope;
  std::string sender;
  std::string return_path;
  addrset_t from;
  addrset_t reply_to;
  addrset_t hashcash;
};
size_t find_next_header_line(const std::string& mail, size_t pos);
mail_addresses extract_sender_addresses(const std::string& mail);

#endif
