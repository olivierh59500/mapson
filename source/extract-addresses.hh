/*
 * Copyright (C) 2001 by Peter Simons <simons@computer.org>.
 * All rights reserved.
 */

#include <set>
#include <string>

struct ltstr
    {
    bool operator()(const std::string& s1, const std::string& s2) const throw()
	{
	return strcasecmp(s1.c_str(), s2.c_str()) < 0;
	}
    };
typedef std::set<std::string,ltstr> addrset_t;
void extract_sender_addresses(const std::string& header, addrset_t& addrset);
