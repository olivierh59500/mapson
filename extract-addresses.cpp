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

#include "mapson.hpp"
#include <cctype>
#include "rfc822.hpp"

using namespace std;

size_t find_next_header_line(const string& mail, size_t pos)
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

inline string to_lowercase(const string& s)
{
  string tmp = s;
  for (string::size_type i = 0; i < tmp.size(); ++i)
    tmp[i] = tolower(tmp[i]);
  return tmp;
}

namespace
{
  class my_committer : public rfc822parser::address_committer
  {
  public:
    explicit my_committer(addrset_t& addrset) : myset(addrset) { }
    void operator() (const rfc822address& addr)
    {
      myset.insert(to_lowercase(addr.address));
    }
  private:
    addrset_t& myset;
  };
}

mail_addresses extract_sender_addresses(const string& mail)
{
  mail_addresses addresses;

  // Parse the mail header and extract anything that contains a
  // valid address.

  for (size_t current = 0, next; current != string::npos; current = next)
  {
    next = find_next_header_line(mail, current);
    string line = mail.substr(current, next - current);
    try
    {
      if (strncasecmp("From ", line.c_str(), sizeof("From ") - 1) == 0)
      {
        if (!addresses.envelope.empty())
          throw rfc822_syntax_error("Mail contains duplicate 'From ' header.");
        try
        {
          line.erase(line.size() - 1, 1).erase(0, sizeof("From ") - 1);
          rfc822parser parser(lex(line));
          addresses.envelope = to_lowercase(parser.addr_spec().address);
        }
        catch(const rfc822_syntax_error& e)
        {
          throw rfc822_syntax_error(string(e.what()) + " in 'From " + line + "'");
        }
      }

      else if (strncasecmp("From:", line.c_str(), sizeof("From:") - 1) == 0)
        try
        {
          line.erase(line.size() - 1, 1).erase(0, sizeof("From:") - 1);
          my_committer committer(addresses.from);
          rfc822parser parser(lex(line), &committer);
          parser.mailboxes();
          if (!parser.empty())
            throw rfc822_syntax_error("Unexpected trailing data");
        }
        catch(const rfc822_syntax_error& e)
        {
          throw rfc822_syntax_error(string(e.what()) + " in 'From:" + line + "'");
        }

      else if (strncasecmp("Reply-To:", line.c_str(), sizeof("Reply-To:") - 1) == 0)
        try
        {
          line.erase(line.size() - 1, 1).erase(0, sizeof("Reply-To:") - 1);
          my_committer committer(addresses.reply_to);
          rfc822parser parser(lex(line), &committer);
          parser.addresses();
          if (!parser.empty())
            throw rfc822_syntax_error("Unexpected trailing data");
        }
        catch(const rfc822_syntax_error& e)
        {
          throw rfc822_syntax_error(string(e.what()) + " in 'Reply-To:" + line + "'");
        }

      else if (strncasecmp("Sender:", line.c_str(), sizeof("Sender:") - 1) == 0)
      {
        if (!addresses.sender.empty())
          throw rfc822_syntax_error("Mail contains duplicate 'Sender:' header.");
        try
        {
          line.erase(line.size() - 1, 1).erase(0, sizeof("Sender:") - 1);
          rfc822parser parser(lex(line));
          addresses.sender = to_lowercase(parser.mailbox().address);
          if (!parser.empty())
            throw rfc822_syntax_error("Unexpected trailing data");
        }
        catch(const rfc822_syntax_error& e)
        {
          throw rfc822_syntax_error(string(e.what()) + " in 'Sender:" + line + "'");
        }
      }

      else if (strncasecmp("Return-Path:", line.c_str(), sizeof("Return-Path:") - 1) == 0)
      {
        if (!addresses.return_path.empty())
          throw rfc822_syntax_error("Mail contains duplicate 'Return-Path:' header.");
        try
        {
          line.erase(line.size() - 1, 1).erase(0, sizeof("Return-Path:") - 1);
          rfc822parser parser(lex(line));
          addresses.return_path = to_lowercase(parser.route_addr().address);
          if (!parser.empty())
            throw rfc822_syntax_error("Unexpected trailing data");
        }
        catch(const rfc822_syntax_error& e)
        {
          throw rfc822_syntax_error(string(e.what()) + " in 'Return-Path:" + line + "'");
        }
      }

      else if (strncasecmp("Message-Id:", line.c_str(), sizeof("Message-Id:") - 1) == 0)
      {
        if (config->have_message_id)
          throw rfc822_syntax_error("Mail contains duplicate 'Return-Path:' header.");
        try
        {
          line.erase(line.size() - 1, 1).erase(0, sizeof("Message-Id:") - 1);
          const_cast<configuration*>(config)->message_id = line;
          const_cast<configuration*>(config)->have_message_id = true;
          rfc822parser parser(lex(line));
          const_cast<configuration*>(config)->message_id.assign("<").append(parser.route_addr().address).append(">");
          if (!parser.empty())
            throw rfc822_syntax_error("Unexpected trailing data");
        }
        catch(const rfc822_syntax_error& e)
        {
          throw rfc822_syntax_error(string(e.what()) + " in 'Message-Id:" + line + "'");
        }
      }
      else if (strncasecmp("X-Hashcash:", line.c_str(), sizeof("X-Hashcash:") - 1) == 0)
      {
        line.erase(line.size() - 1, 1).erase(0, sizeof("X-Hashcash:") - 1);
        while(line.size() > 0 && isspace(line[0]))
          line.erase(0, 1);
        addresses.hashcash.insert(line);
      }
    }
    catch(const rfc822_syntax_error& e)
    {
      if (config->let_incorrect_mails_pass)
        info("Tolerating syntax error: %s", e.what());
      else
        throw;
    }
  }

#ifdef DEBUG
  debug(("Found the following addresses in the mail:"));
  debug(("    Envelope    = %s", addresses.envelope.c_str()));
  debug(("    Sender      = %s", addresses.sender.c_str()));
  debug(("    Return-Path = %s", addresses.return_path.c_str()));
  debug(("    Message-Id  = %s", config->message_id.c_str()));
  string tmp;
  for (addrset_t::const_iterator i = addresses.from.begin(); i != addresses.from.end(); ++i)
  {
    if (!tmp.empty())
      tmp += ", ";
    tmp += *i;
  }
  debug(("    From        = %s", tmp.c_str()));
  tmp.erase();
  for (addrset_t::const_iterator i = addresses.reply_to.begin(); i != addresses.reply_to.end(); ++i)
  {
    if (!tmp.empty())
      tmp += ", ";
    tmp += *i;
  }
  debug(("    Reply-To    = %s", tmp.c_str()));

  debug(("Found the following HashCash cookies in the mail:"));
  for (addrset_t::const_iterator i = addresses.hashcash.begin(); i != addresses.hashcash.end(); ++i)
    debug(("    %s", i->c_str()));
#endif

  // Now do consistency checks on the whole thing.

  if (!config->let_incorrect_mails_pass &&
     addresses.sender.empty() && addresses.from.size() != 1)
  {
    throw rfc822_syntax_error("The 'From:' line contains multiple addresses but no 'Sender:' has been provided.");
  }

  return addresses;
}
