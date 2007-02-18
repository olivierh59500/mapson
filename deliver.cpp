/*
 * Copyright (C) 2002 by Peter Simons <simons@cryp.to>.
 * All rights reserved.
 */

// ISO C++ headers.
#include <cstdio>

// POSIX.1 system headers.
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

// My own libraries.
#include "sanity/system-error.hpp"
#include "config.hpp"
#include "log.hpp"
#include "fd-sentry.hpp"
#include "deliver.hpp"

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
