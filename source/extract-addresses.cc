/*
 * Copyright (C) 2001 by Peter Simons <simons@computer.org>.
 * All rights reserved.
 */

// ISO C++ headers.
#include <set>

// My own libraries.
#include "librfc822/rfc822.hh"
#include "extract-addresses.hh"

using namespace std;

inline size_t find_next_header_line(const string& mail, size_t pos)
    {
    if (mail[pos] == '\n')
	return string::npos;	// Header ends here.

    while (pos < mail.size())
	{
	if (mail[pos] == '\n')
	    {
	    if (pos+1 < mail.size())
		{
		if (mail[pos+1] != ' ' && mail[pos+1] != '\t')
		    return pos+1;
		}
	    else
		return string::npos;
	    }
	++pos;
	}
    return (pos < mail.size()) ? pos : string::npos;
    }

void extract_sender_addresses(const string& mail, addrset_t& addrset)
    {
    class my_committer : public rfc822parser::address_committer
	{
      public:
	explicit my_committer(addrset_t& addrset) : myset(addrset) { }
	void operator() (const rfc822address& addr)
	    {
	    myset.insert(addr.address);
	    }
      private:
	addrset_t& myset;
	};
    my_committer committer(addrset);

    for (size_t current = 0, next; current != string::npos; current = next)
	{
	next = find_next_header_line(mail, current);
	std::string line = mail.substr(current, next - current);
	if (strncasecmp("From:", line.c_str(), sizeof("From:") - 1) == 0
	    || strncasecmp("Reply-To:", line.c_str(), sizeof("Reply-To:") - 1) == 0
	    || strncasecmp("Sender:", line.c_str(), sizeof("Sender:") - 1) == 0
	    || strncasecmp("Return-Path:", line.c_str(), sizeof("Return-Path:") - 1) == 0)
	    {
	    line.erase(0, line.find(":") + 1);
	    rfc822parser parser(lex(line), &committer);
	    parser.addresses();
	    if (!parser.empty())
		throw rfc822_syntax_error("Unexpected trailing data.");
	    }
	}
    }
