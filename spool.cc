/*
 * Copyright (C) 2002 by Peter Simons <simons@computer.org>.
 * All rights reserved.
 */

// POSIX.1 system headers.
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

// My own libraries.
#include "libmd5/md5.h"
#include "system-error/system-error.hh"
#include "log.hh"
#include "config.hh"
#include "fd-sentry.hh"
#include "spool.hh"

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
    debug(("Spooling e-mail as '%s'.", filename.c_str()));
    int fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd < 0)
	throw system_error(string("Can't open spool file '") + filename + "' for writing");
    fd_sentry sentry(fd);
    for (size_t len = 0; len < mail.size(); )
	{
	ssize_t rc = write(fd, mail.data()+len, mail.size()-len);
	if (rc < 0)
	    throw system_error(string("Failed writing to the spool file '") + filename + "'");
	else
	    len += rc;
	}

    return buf;
    }
