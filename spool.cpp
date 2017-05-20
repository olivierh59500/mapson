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
#include "md5.h"

using namespace std;

string spool(const string& mail)
{
  // Calculate the md5 hash of the mail.

  unsigned char sum[16];
  char buf[33];
  md5_buffer(mail.data(), mail.size(), sum);
  snprintf(buf, 33, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
          sum[0], sum[1], sum[2], sum[3], sum[4], sum[5], sum[6], sum[7],
          sum[8], sum[9], sum[10], sum[11], sum[12], sum[13], sum[14], sum[15]);

  // Make sure the spool directory exists.

  struct stat mystat;
  if (stat(config->spool_dir.c_str(), &mystat) == -1)
    if (mkdir(config->spool_dir.c_str(), S_IRUSR | S_IWUSR | S_IRWXU) == -1)
      throw runtime_error(string("Can't create spool directory '") + config->spool_dir + "'");

  // Open the spool file and write store the mail there.

  string filename = config->spool_dir + "/" + buf;
  info("Spooling e-mail '%s' as '%s'.", config->message_id.c_str(), filename.c_str());
  int fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  if (fd < 0)
    throw Mapson::system_error(string("Can't open spool file '") + filename + "' for writing");
  fd_sentry sentry(fd);
  for (size_t len = 0; len < mail.size(); )
  {
    ssize_t rc = write(fd, mail.data()+len, mail.size()-len);
    if (rc < 0)
      throw Mapson::system_error(string("Failed writing to the spool file '") + filename + "'");
    else
      len += rc;
  }

  return buf;
}
