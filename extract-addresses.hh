/*
 * Copyright (C) 2001 by Peter Simons <simons@computer.org>.
 * All rights reserved.
 */

#ifndef EXTRACT_ADDRESSES_HH
#define EXTRACT_ADDRESSES_HH

// ISO C++ headers.
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
    };
size_t find_next_header_line(const std::string& mail, size_t pos);
mail_addresses extract_sender_addresses(const std::string& mail);

#endif
