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

#include "mapson.hpp"
#include "varexp/varexp.hpp"

using namespace std;

namespace
{
  class lookup_t : public varexp::callback_t
  {
  public:
    lookup_t(const string& _mail, const string& _hash, const mail_addresses& _addresses)
                : mail(_mail), hash(_hash), addresses(_addresses)
    {
      // Find the beginning of the mail body; we'll need that
      // information later.

      for (body_pos = 0; body_pos != string::npos; body_pos = find_next_header_line(mail, body_pos))
      {
        if (mail[body_pos] == '\n')
        {
          ++body_pos;
          break;
        }
      }
    }

  private:
    virtual void operator()(const string& name, string& data)
    {
      if (strcasecmp("md5hash", name.c_str()) == 0)
        data = hash;
      else if (strcasecmp("envelope", name.c_str()) == 0)
        data = addresses.envelope;
      else if (strcasecmp("sender", name.c_str()) == 0)
        data = addresses.sender;
      else if (strcasecmp("return_path", name.c_str()) == 0)
        data = addresses.return_path;
      else if (strcasecmp("header", name.c_str()) == 0)
        data = mail.substr(0, body_pos - 1);
      else if (strcasecmp("body", name.c_str()) == 0)
        data = mail.substr(body_pos);
      else if (strcasecmp("messageid", name.c_str()) == 0)
        data = config->message_id;
      else
      {
        const char* p = getenv(name.c_str());
        if (p == NULL)
          throw varexp::undefined_variable();
        else
          data = p;
      }
    }
    virtual void operator()(const string& name, int idx, string& data)
    {
      if (idx >= 0)
      {
        if (strcasecmp("headerlines", name.c_str()) == 0)
        {
          for (size_t i = 0, pos = 0; pos != string::npos; pos = find_next_header_line(mail, pos), ++i)
          {
            if (mail[pos] == '\n')
              break;
            if (i == static_cast<size_t>(idx))
            {
              i = find_next_header_line(mail, pos);
              data = mail.substr(pos, i - pos);
              return;
            }
          }
        }
        else if (strcasecmp("header", name.c_str()) == 0)
        {
          for (size_t nextpos, i = 0, pos = 0; pos != string::npos && pos < mail.size(); ++i, pos = nextpos)
          {
            if (mail[pos] == '\n')
              break;
            nextpos = mail.find("\n", pos);
            if (nextpos != string::npos)
              ++nextpos;
            if (i == static_cast<size_t>(idx))
            {
              data = mail.substr(pos, nextpos - pos);
              return;
            }
          }
        }
        else if (strcasecmp("body", name.c_str()) == 0)
        {
          for (size_t nextpos, i = 0, pos = body_pos; pos != string::npos && pos < mail.size(); ++i, pos = nextpos)
          {
            nextpos = mail.find("\n", pos);
            if (nextpos != string::npos)
              ++nextpos;
            if (i == static_cast<size_t>(idx))
            {
              data = mail.substr(pos, nextpos - pos);
              return;
            }
          }
        }
        else if (strcasecmp("from", name.c_str()) == 0)
        {
          for (addrset_t::const_iterator i = addresses.from.begin(); i != addresses.from.end(); ++i)
          {
            if (idx-- == 0)
            {
              data = *i;
              return;
            }
          }
        }
        else if (strcasecmp("replyto", name.c_str()) == 0)
        {
          for (addrset_t::const_iterator i = addresses.reply_to.begin(); i != addresses.reply_to.end(); ++i)
          {
            if (idx-- == 0)
            {
              data = *i;
              return;
            }
          }
        }
      }
      throw varexp::undefined_variable();
    }
    const string& mail;
    const string& hash;
    const mail_addresses& addresses;
    size_t body_pos;
  };
}

void request_confirmation(const string& mail, const string& hash, const mail_addresses& addresses)
{
  lookup_t lookup(mail, hash, addresses);
  string mail_template;

  // Read request-for-confirmation mail template into buffer.

  string filename = config->request_for_confirmation_file;
  int fd = multi_open(filename, O_RDONLY, S_IRUSR | S_IWUSR);
  if (fd < 0)
    throw system_error(string("Can't open request-mail template file '") + filename + "' for reading");
  fd_sentry sentry(fd);

  // Read the file into memory.

  char buffer[1024];
  ssize_t rc;
  for (rc = read(fd, buffer, sizeof(buffer)); rc > 0; rc = read(fd, buffer, sizeof(buffer)))
    mail_template.append(buffer, rc);
  if (rc < 0)
    throw system_error(string("Failed to read request-mail template file '") + filename + "' into memory");

  // Expand variables in the template.

  varexp::unescape(mail_template, mail_template, false);
  varexp::expand(mail_template, mail_template, lookup);
  varexp::unescape(mail_template, mail_template, true);

  // Pipe expanded buffer into MTA.

  debug(("Executing mail transport agent '%s'.", config->mta.c_str()));
  FILE* fh = popen(config->mta.c_str(), "w");
  if (fh == NULL)
    throw system_error(string("Can't start MTA '") + config->mta + "'");
  if (fwrite(mail_template.data(), mail_template.size(), 1, fh) != 1)
  {
    pclose(fh);
    throw system_error(string("Failed to pipe to MTA process '") + config->mta + "'");
  }
  pclose(fh);
}
