/*
 * Copyright (c) 2010-2017 by Peter Simons <simons@cryp.to>.
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

#ifndef RFC822_HPP_INCLUDED
#define RFC822_HPP_INCLUDED

#include "config.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <iterator>
#include <deque>
#include <cctype>
#include <list>

struct rfc822address
{
  std::string address;
  std::string localpart;
  std::string hostpart;
};

inline std::ostream& operator << (std::ostream& os, const rfc822address& addr)
{
  return os << addr.address
            << ": "
            << addr.localpart
            << " @ "
            << addr.hostpart;
}

class rfc822_syntax_error : public std::runtime_error
{
public:
  rfc822_syntax_error(const std::string& s) : std::runtime_error(s) { }
};

struct token
{
  enum token_type
    {
      atom,
      character,
      domain_literal,
      quoted_string,
      unknown
    };

  token_type   type;
  std::string  rep;

  token(const token_type t = unknown, const std::string& s = "") : type(t), rep(s) { }
  token(const token& rhs) : type(rhs.type), rep(rhs.rep) { }

  token& operator= (const token& rhs)
  {
    if (&rhs != this)
    {
      type = rhs.type;
      rep  = rhs.rep;
    }
    return *this;
  }

  bool operator== (const token& rhs)
  {
    return (type == rhs.type && rep == rhs.rep) ? true : false;
  }

  bool operator!= (const token& rhs)
  {
    return (type != rhs.type || rep != rhs.rep) ? true : false;
  }
};

std::ostream& operator<< (std::ostream& os, const token& t);

typedef std::deque<token> tokstream_t;

tokstream_t lex(const std::string& buffer);

class rfc822parser
{
public:

  class address_committer
  {
  public:
    virtual void operator() (const rfc822address&) { }
  };

public:
  rfc822parser(tokstream_t ts, class address_committer* c = 0)
	    : tokstream(ts), commit(c) { }
  ~rfc822parser() { }

private:			// don't copy me
  rfc822parser(const rfc822parser&);
  rfc822parser& operator= (const rfc822parser&);

public:
  void           addresses();
  void           mailboxes();
  void           address();
  rfc822address  mailbox();
  rfc822address  route_addr();
  rfc822address  addr_spec();
  bool           empty() { return tokstream.empty(); }

private:
  void           group();
  rfc822address  route();
  std::string    local_part();
  std::string    domain();
  std::string    sub_domain();
  std::string    domain_ref();
  std::string    word();
  std::string    atom();
  std::string    domain_literal();
  std::string    quoted_string();
  std::string    at();
  std::string    dot();
  std::string    comma();
  std::string    smaller_than();
  std::string    greater_than();

private:
  tokstream_t              tokstream;
  class address_committer* commit;

#ifdef DEBUG_RFC_PARSER
private:
  class Tracer
  {
  private:
    tokstream_t& ts;

  public:
    Tracer(const char* name, tokstream_t& ts_) : ts(ts_), funcname(name)
    {
      indent(std::cout);
      std::cout << "Entering " << funcname << "(";
      tokstream_t::const_iterator i;
      for (i = ts.begin(); i != ts.end(); ++i)
        std::cout << *i << " ";
      std::cout << ")" << std::endl;
      ++instances;
    }
    ~Tracer()
    {
      --instances;
      indent(std::cout);
      std::cout << "Leaving " << funcname << "()" << std::endl;
    }

  private:
    void indent(std::ostream& os)
    {
      for (unsigned int i = 0; i < instances*4; ++i)
        os << " ";
    }

    const char*          funcname;
    static unsigned int  instances;
  };
#endif
};

template<class T>
class insert_iterator_commit : public rfc822parser::address_committer
{
private:
  std::insert_iterator<T>* ii;
public:
  insert_iterator_commit(std::insert_iterator<T>* ii_ = 0) : ii(ii_) { }
  void operator() (const rfc822address& addr)
  {
    if (ii != 0)
    {
      **ii = addr;
      ++(*ii);
    }
  }
};

inline void check_rfc822_addresses(const std::string& input)
{
  rfc822parser parser(lex(input));
  parser.addresses();
  if (!parser.empty())
    throw rfc822_syntax_error("Unexpected trailing data.");
}

inline void check_rfc822_mailboxes(const std::string& input)
{
  rfc822parser parser(lex(input));
  parser.mailboxes();
  if (!parser.empty())
    throw rfc822_syntax_error("Unexpected trailing data.");
}

inline void check_rfc822_address(const std::string& input)
{
  rfc822parser parser(lex(input));
  parser.address();
  if (!parser.empty())
    throw rfc822_syntax_error("Unexpected trailing data.");
}

inline rfc822address parse_rfc822_mailbox(const std::string& input)
{
  rfc822parser parser(lex(input), 0);
  rfc822address res = parser.mailbox();
  if (!parser.empty())
    throw rfc822_syntax_error("Unexpected trailing data.");
  return res;
}

inline void check_rfc822_mailbox(const std::string& input)
{
  parse_rfc822_mailbox(input);
}

inline rfc822address parse_rfc822_route_addr(const std::string& input)
{
  rfc822parser parser(lex(input), 0);
  rfc822address res = parser.route_addr();
  if (!parser.empty())
    throw rfc822_syntax_error("Unexpected trailing data.");
  return res;
}

inline void check_rfc822_route_addr(const std::string& input)
{
  parse_rfc822_route_addr(input);
}

inline rfc822address parse_rfc822_addr_spec(const std::string& input)
{
  rfc822parser parser(lex(input), 0);
  rfc822address res = parser.addr_spec();
  if (!parser.empty())
    throw rfc822_syntax_error("Unexpected trailing data.");
  return res;
}

inline void check_rfc822_addr_spec(const std::string& input)
{
  parse_rfc822_addr_spec(input);
}

template<class T>
inline void parse_rfc822_addresses(std::insert_iterator<T>* ii, const std::string& input)
{
  insert_iterator_commit<T> committer(ii);
  rfc822parser parser(lex(input), &committer);
  parser.addresses();
  if (!parser.empty())
    throw rfc822_syntax_error("Unexpected trailing data.");
}

template<class T>
inline void parse_rfc822_mailboxes(std::insert_iterator<T>* ii, const std::string& input)
{
  insert_iterator_commit<T> committer(ii);
  rfc822parser parser(lex(input), &committer);
  parser.mailboxes();
  if (!parser.empty())
    throw rfc822_syntax_error("Unexpected trailing data.");
}

template<class T>
inline void parse_rfc822_address(std::insert_iterator<T>* ii, const std::string& input)
{
  insert_iterator_commit<T> committer(ii);
  rfc822parser parser(lex(input), &committer);
  parser.address();
  if (!parser.empty())
    throw rfc822_syntax_error("Unexpected trailing data.");
}

#endif // RFC822_HPP_INCLUDED
