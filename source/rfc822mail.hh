/*
 * Copyright (C) 2001 by Peter Simons <simons@computer.org>.
 * All rights reserved.
 */

#ifndef __RCF822MAIL_HH__
#define __RCF822MAIL_HH__

#include <string>
#include <set>
#include <stdexcept>
#include "librfc822/rfc822.hh"

class rfc822mail
    {
  public:
    explicit rfc822mail()
	{
	}
    explicit rfc822mail(const string& raw_mail)
	{
	*this = raw_mail;
	}
    ~rfc822mail()
	{
	}

    rfc822mail& operator= (const string& raw_mail)
	{
	// Find header and body of the mail.

	string::size_type pos = raw_mail.find("\n\n", 0, 2);
	if (pos == string::npos)
	    throw rfc822_syntax_error("Wrong format for an RFC822 mail.");
	header = raw_mail.substr(0, pos+1);
	body   = raw_mail.substr(pos+2, string::npos);

	// Parse the relevant fields of the header.

	for (string::size_type next, current = 0; current < header.size(); current = next)
	    {
	    next = find_next_header_line(header, current);
	    assert(next > current);
	    string line = header.substr(current, next-current);
#if 0
	    if (strncasecmp(line.c_str(), "From ", sizeof("From ")-1) == 0)
		{
		printf("*** Parsing 'From ' line:\n%s", line.c_str());
		line.erase(0, sizeof("From ")-1);
		parse_rfc822_addresses(&ii, line);
		}
	    else
#endif
	    if (strncasecmp(line.c_str(), "From:", sizeof("From:")-1) == 0)
		{
		printf("*** Parsing 'From:' line:\n%s", line.c_str());
		line.erase(0, sizeof("From:")-1);
		parse_rfc822_addresses(&ii, line);
		}
	    else if (strncasecmp(line.c_str(), "Reply-To:", sizeof("Reply-To:")-1) == 0)
		{
		printf("*** Parsing 'Reply-To:' line:\n%s", line.c_str());
		line.erase(0, sizeof("Reply-To:")-1);
		parse_rfc822_addresses(&ii, line);
		}
	    else if (strncasecmp(line.c_str(), "Sender:", sizeof("Sender:")-1) == 0)
		{
		printf("*** Parsing 'Sender:' line:\n%s", line.c_str());
		line.erase(0, sizeof("Sender:")-1);
		parse_rfc822_addresses(&ii, line);
		}
	    else if (strncasecmp(line.c_str(), "Return-Path:", sizeof("Return-Path:")-1) == 0)
		{
		printf("*** Parsing 'Return-Path:' line:\n%s", line.c_str());
		line.erase(0, sizeof("Return-Path:")-1);
		parse_rfc822_addresses(&ii, line);
		}

	    }



	for (deque<rfc822address>::const_iterator i = sender_addresses.begin();
	     i != sender_addresses.end();
	     ++i)
	    {
	    printf("%s --> %s @ %s\n", i->address.c_str(),
		   i->localpart.c_str(), i->hostpart.c_str());
	    }

	return *this;
	}

  private:
    string::size_type find_next_header_line(const string& buf, string::size_type pos)
	{
	while (pos < buf.size())
	    {
	    if (buf[pos] == '\n')
		{
		if (pos+1 >= buf.size())
		    return string::npos;
		else if (buf[pos+1] != ' ' && buf[pos+1] != '\t')
		    return pos+1;
		}
	    ++pos;
	    }
	return string::npos;
	}

    class my_committer : public rfc822parser::address_committer
	{
      public:
	void operator() (const rfc822address& addr)
	    {

	    }
	};

  public:
    string header, body;
    set<string> sender_addresses;
    };

#endif
