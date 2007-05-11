/*
 * Copyright (c) 2001-2007 Peter Simons <simons@cryp.to>
 *
 * This software is provided 'as-is', without any express or
 * implied warranty. In no event will the authors be held liable
 * for any damages arising from the use of this software.
 *
 * Copying and distribution of this file, with or without
 * modification, are permitted in any medium without royalty
 * provided the copyright notice and this notice are preserved.
 */

#include "mapson.hpp"

using namespace std;

void deliver(const string& mail)
{
  // If the mailbox starts with a pipe ('|'), we'll use the string
  // as a command to which to pipe the mail to. Otherwise, we
  // interpret it as a file to which the mail is written.

  if (!config->mailbox.empty() && config->mailbox[0] == '|')
  {
    debug(("Delivering mail via pipe '%s'.", config->mailbox.c_str()));

    FILE* fh = popen(config->mailbox.c_str()+1, "w");
    if (fh == NULL)
      throw system_error(string("Can't start delivery pipe '") + config->mailbox + "'");
    int len = fwrite(mail.data(), mail.size(), 1, fh);
    pclose(fh);
    if (len != 1)
      throw system_error(string("Failed to pipe to MTA process '") + config->mailbox + "'");
  }
  else
  {
    debug(("Delivering mail to mailbox '%s'.", config->mailbox.c_str()));

    int fd = open(config->mailbox.c_str(), O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    if (fd < 0)
      throw system_error(string("Can't open mailbox file '") + config->mailbox + "' for writing");
    fd_sentry sentry(fd);

    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start  = 0;
    lock.l_len    = 0;
    if (fcntl(fd, F_SETLKW, &lock) != 0)
      throw system_error(string("Can't lock file '") + config->mailbox + "'");

    for (size_t len = 0; len < mail.size(); )
    {
      ssize_t rc = write(fd, mail.data()+len, mail.size()-len);
      if (rc < 0)
        throw system_error(string("Failed writing to the mailbox file '") + config->mailbox + "'");
      else
        len += rc;
    }
  }
}
