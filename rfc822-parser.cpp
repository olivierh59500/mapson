/*
 * Copyright (c) 1998-2007 Peter Simons <simons@cryp.to>
 *
 * This software is provided 'as-is', without any express or
 * implied warranty. In no event will the authors be held liable
 * for any damages arising from the use of this software.
 *
 * Copying and distribution of this file, with or without
 * modification, are permitted in any medium without royalty
 * provided the copyright notice and this notice are preserved.
 */

#include "rfc822.hpp"
using namespace std;

#ifdef DEBUG_RFC_PARSER
   unsigned int   rfc822parser::Tracer::instances = 0;
#  define TRACE() Tracer T(__FUNCTION__, tokstream)
#else
#  define TRACE()
#endif

void
rfc822parser::addresses()
{				// address ("," address)*
  TRACE();

  for (address();
       !tokstream.empty() && tokstream.front().type == token::character && tokstream.front().rep == ",";
       /* nothing */)
  {
    comma();
    address();
  }
}

void
rfc822parser::mailboxes()
{				// mailbox ("," mailbox)*
  TRACE();

  if (commit != 0)
    (*commit)(mailbox());
  else
    mailbox();

  while(!tokstream.empty() && tokstream.front().type == token::character && tokstream.front().rep == ",")
  {
    comma();
    if (commit != 0)
      (*commit)(mailbox());
    else
      mailbox();
  }
}

void
rfc822parser::address()
{				// mailbox | group
  TRACE();

  // Do lookeahead to decide which kind of address to expect.

  tokstream_t::iterator p;
  for (p = tokstream.begin(); p != tokstream.end(); ++p)
  {
    if ((*p).type == token::atom || (*p).type == token::quoted_string)
      continue;
    else
      break;
  }

  if (p == tokstream.end())
    throw rfc822_syntax_error("Input is neither 'addr_spec' nor 'route_addr'.");

  if ((*p).type == token::character && (*p).rep == ":")
  {
    tokstream.erase(tokstream.begin(), p);
    group();
  }
  else
  {
    if (commit != 0)
      (*commit)(mailbox());
    else
      mailbox();
  }
}

void
rfc822parser::group()
{				// ":" [ mailbox ("," mailbox)* ] ";"
  TRACE();

  if (!tokstream.empty() && tokstream.front().type == token::character && tokstream.front().rep == ":")
  {
    tokstream.pop_front();
  }
  else
    throw rfc822_syntax_error("Expected a ':'.");

  if (commit != 0)
    (*commit)(mailbox());
  else
    mailbox();

  while (!tokstream.empty() && (tokstream.front().type != token::character || tokstream.front().rep != ";"))
  {
    comma();
    if (commit != 0)
      (*commit)(mailbox());
    else
      mailbox();
  }

  if (tokstream.empty() || tokstream.front().type != token::character || tokstream.front().rep != ";")
    throw rfc822_syntax_error("Missing ';' at end of address group.");

  tokstream.pop_front();
}

rfc822address
rfc822parser::mailbox()
{				// addr_spec | phrase route_addr
  TRACE();

  // Do lookeahead to decide which kind of address to expect.

  tokstream_t::iterator p;
  for (p = tokstream.begin(); p != tokstream.end(); ++p)
  {
    if ((*p).type == token::atom || (*p).type == token::quoted_string)
      continue;
    else
      break;
  }

  if (p == tokstream.end())
    throw rfc822_syntax_error("Input is neither 'addr_spec' nor 'route_addr'.");

  if ((*p).type == token::character && (*p).rep == "<")
  {
    tokstream.erase(tokstream.begin(), p);
    return route_addr();
  }
  else
    return addr_spec();
}

rfc822address
rfc822parser::route_addr()
{				// "<" [route] addr_spec ">"
  TRACE();

  smaller_than();
  if (!tokstream.empty() && tokstream.front().type == token::character &&
     tokstream.front().rep == "@")
  {
    // We expect a route.

    rfc822address route;
    rfc822address addr(addr_spec());
    greater_than();
    if (route.localpart.empty())
    {
      route.localpart += addr.address;
      route.address  = "<";
      route.address += "@";
      route.address += route.hostpart;
      route.address += ":";
      route.address += route.localpart;
      route.address += ">";
    }
    else
    {
      route.localpart += addr.address;
      route.address  = "<";
      route.address += "@";
      route.address += route.hostpart;
      route.address += ",";
      route.address += route.localpart;
      route.address += ">";
      route.localpart.insert(0, "<").append(">");
    }
    return route;
  }

  rfc822address addr  = addr_spec();
  greater_than();
  return addr;
}

rfc822address
rfc822parser::route()
{				// "@" domain ( "," "@" domain)* ":"
  TRACE();
  rfc822address res;
  token  tok_colon(token::character, ":");

  at();
  res.hostpart = domain();

  while(!tokstream.empty() && tokstream.front() != tok_colon)
  {
    if (res.localpart.empty())
      comma();
    else
      res.localpart += comma();
    res.localpart += at();
    res.localpart += domain();
  }

  if (!tokstream.empty() && tokstream.front() == tok_colon)
  {
    if (!res.localpart.empty())
      res.localpart += ":";
    tokstream.pop_front();
  }
  else
    throw rfc822_syntax_error("Expected ':'.");

  return res;
}

rfc822address
rfc822parser::addr_spec()
{				// local_part "@" domain
  TRACE();
  rfc822address res;

  res.localpart = res.address = local_part();
  res.address += at();
  res.hostpart = domain();
  res.address += res.hostpart;

  return res;
}

string
rfc822parser::local_part()
{				// word ( "." word )*
  TRACE();
  string res;

  res = word();
  tokstream_t  tsback;
  string tmp;
  try {
    for (;;)
    {
      tsback = tokstream;
      tmp = dot();
      tmp += word();
      res += tmp;
    }
  }
  catch(rfc822_syntax_error)
  {
    tokstream = tsback;
  }

  return res;
}

string
rfc822parser::domain()
{				// sub_domain ( "." sub_domain )*
  TRACE();
  string res;

  res = sub_domain();
  tokstream_t  tsback;
  string tmp;
  try {
    for (;;)
    {
      tsback = tokstream;
      tmp = dot();
      tmp += sub_domain();
      res += tmp;
    }
  }
  catch(rfc822_syntax_error)
  {
    tokstream = tsback;
  }

  return res;
}

string
rfc822parser::sub_domain()
    {				// domain_ref | domain_literal
    TRACE();

    if (tokstream.empty())
	throw rfc822_syntax_error("Unexpected end of input.");

    token t(tokstream.front());
    if (t.type == token::atom || t.type == token::domain_literal)
	{
	tokstream.pop_front();
	return t.rep;
	}
    else
	throw rfc822_syntax_error("Expected 'sub_domain'.");
    }

string
rfc822parser::domain_ref()
    {				// ATOM
    return atom();
    }

string
rfc822parser::word()
    {				// ATOM | QUOTED_STRING
    TRACE();

    if (tokstream.empty())
	throw rfc822_syntax_error("Unexpected end of input.");

    token t(tokstream.front());
    if (t.type == token::atom || t.type == token::quoted_string)
	{
	tokstream.pop_front();
	return t.rep;
	}
    else
	throw rfc822_syntax_error("Expected 'word'.");
    }

string
rfc822parser::atom()
    {				// ATOM
    TRACE();

    if (tokstream.empty())
	throw rfc822_syntax_error("Unexpected end of input.");

    token t(tokstream.front());
    if (t.type == token::atom)
	{
	tokstream.pop_front();
	return t.rep;
	}
    else
	throw rfc822_syntax_error("Expected 'atom'.");
    }

string
rfc822parser::domain_literal()
    {				// DOMAIN_LITERAL
    TRACE();

    if (tokstream.empty())
	throw rfc822_syntax_error("Unexpected end of input.");

    token t(tokstream.front());
    if (t.type == token::domain_literal)
	{
	tokstream.pop_front();
	return t.rep;
	}
    else
	throw rfc822_syntax_error("Expected 'domain_literal'.");
    }

string
rfc822parser::quoted_string()
    {				// QUOTED_STRING
    TRACE();

    if (tokstream.empty())
	throw rfc822_syntax_error("Unexpected end of input.");

    token t(tokstream.front());
    if (t.type == token::quoted_string)
	{
	tokstream.pop_front();
	return t.rep;
	}
    else
	throw rfc822_syntax_error("Expected 'quoted_string'.");
    }

string
rfc822parser::at()
    {				// "@"
    TRACE();

    if (tokstream.empty())
      throw rfc822_syntax_error("Unexpected end of input.");

    token t(tokstream.front());
    if (t.type == token::character && t.rep == "@")
	{
	tokstream.pop_front();
	return "@";
	}
    else
	throw rfc822_syntax_error("Expected '@'.");
    }

string
rfc822parser::dot()
    {				// "."
    TRACE();

    if (tokstream.empty())
	throw rfc822_syntax_error("Unexpected end of input.");

    token t(tokstream.front());
    if (t.type == token::character && t.rep == ".")
	{
	tokstream.pop_front();
	return ".";
	}
    else
	throw rfc822_syntax_error("Expected '.'.");
    }

string
rfc822parser::smaller_than()
    {				// "<"
    TRACE();

    if (tokstream.empty())
	throw rfc822_syntax_error("Unexpected end of input.");

    token t(tokstream.front());
    if (t.type == token::character && t.rep == "<")
	{
	tokstream.pop_front();
	return "<";
	}
    else
	throw rfc822_syntax_error("Expected '<'.");
    }

string
rfc822parser::greater_than()
    {				// ">"
    TRACE();

    if (tokstream.empty())
	throw rfc822_syntax_error("Unexpected end of input.");

    token t(tokstream.front());
    if (t.type == token::character && t.rep == ">")
	{
	tokstream.pop_front();
	return ">";
	}
    else
	throw rfc822_syntax_error("Expected '>'.");
    }

string
rfc822parser::comma()
    {				// (",")+
    TRACE();
    token  tok_comma(token::character, ",");

    if (tokstream.empty() || tokstream.front() != tok_comma)
	throw rfc822_syntax_error("Expected ','.");

    do  {
	tokstream.pop_front();
	}
    while (!tokstream.empty() && tokstream.front() == tok_comma);

    return ",";
}
