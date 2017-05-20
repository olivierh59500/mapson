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
#include <vector>

using namespace std;

inline string::size_type find_cookie(string const & buffer, string::size_type pos)
{
  for(size_t hits_in_a_row = 0; pos < buffer.size(); ++pos)
  {
    if ((buffer[pos] >= 'a' && buffer[pos] <= 'f')
       || (buffer[pos] >= 'A' && buffer[pos] <= 'F')
       || (buffer[pos] >= '0' && buffer[pos] <= '9'))
      ++hits_in_a_row;
    else
      hits_in_a_row = 0;
    if (hits_in_a_row == 32)
      return pos - 31;
  }
  return string::npos;
}

inline void deliver_approved_mail(string const & filename, string const & cookie, string& mail)
{
  int fd = open(filename.c_str(), O_RDONLY, 0);
  if (fd > 0)
  {
    fd_sentry s(fd);

    info("Incoming e-mail '%s' contained cookie '%s'; delivering the corresponding spooled mail.",
        config->message_id.c_str(), cookie.c_str());

    mail.erase();
    char tmp[8*1024];
    ssize_t rc;
    for (rc = read(fd, tmp, sizeof(tmp)); rc > 0; rc = read(fd, tmp, sizeof(tmp)))
      mail.append(tmp, rc);
    if (rc < 0)
      throw Mapson::system_error(string("Failed to read mail file '") + filename + "'");
    deliver(mail);
    unlink(filename.c_str());
  }
  else
    throw runtime_error(string("Failed to open file '") + filename + "'");
}

bool accept_confirmation(string& mail, string const & cmdline_cookie)
{
  // If cookie is set, it has been provided on the command line.
  // Hence, we assume it is for real and skip the finding-process.
  // If the cookie turns out to be wrong, we'll continue as if none
  // had been specified.

  if (!cmdline_cookie.empty())
  {
    debug(("Checking cookie '%s', which was specified on the command line.", cmdline_cookie.c_str()));
    struct stat sbuf;
    string filename = config->spool_dir + "/" + cmdline_cookie;
    if (stat(filename.c_str(), &sbuf) == 0)
    {
      deliver_approved_mail(filename, cmdline_cookie, mail);
      return true;
    }
    else
      debug(("Command line cookie was not correct."));
  }


  // Scan the body only when the command line configuration says to
  // do so.

  if (config->scan_for_cookie == false)
  {
    debug(("--dont-scan option was set on the command line. So we don't."));
    return false;
  }

  // Find all strings in the mail that could be cookie and check,
  // whether a mail has been spooled under that name. If, deliver
  // that mail and return.

  for (string::size_type i = 0; ; ++i)
  {
    i = find_cookie(mail, i);
    if (i != string::npos)
    {
      string cookie = mail.substr(i, 32);
      debug(("Found potential cookie '%s' at mail offset %u", cookie.c_str(), i));

      struct stat sbuf;
      string filename = config->spool_dir + "/" + cookie;
      if (stat(filename.c_str(), &sbuf) == 0)
      {
        deliver_approved_mail(filename, cookie, mail);
        return true;
      }
    }
    else
      break;
  }

  debug(("E-mail does not contain a valid cookie; continue processing."));
  return false;
}
