/*
 * Copyright (C) 2001 by Peter Simons <simons@computer.org>.
 * All rights reserved.
 */

// POSIX.1 system headers.
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

// My own libraries.
#include "libmd5/md5.h"
#include "system-error/system-error.hh"
#include "log.hh"
#include "config.hh"
#include "spool.hh"

using namespace std;

string spool(const string& mail)
    {
    unsigned char sum[16];
    char buf[33];
    string filename;

    // Calculate the md5 hash of the mail.

    md5_buffer(mail.data(), mail.size(), sum);
    snprintf(buf, 33, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
            sum[0], sum[1], sum[2], sum[3], sum[4], sum[5], sum[6], sum[7],
            sum[8], sum[9], sum[10], sum[11], sum[12], sum[13], sum[14], sum[15]);

    filename = config->spool_dir + "/" + buf;
    debug(("Spooling e-mail as '%s'.", filename.c_str()));

    // Open the spool file and write store the mail there.

    int fd = open(filename.c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd < 0)
	throw system_error(string("Can't open spool file '") + filename + "' for writing");
    for (size_t len = 0; len < mail.size(); )
	{
	ssize_t rc = write(fd, mail.data()+len, mail.size()-len);
	if (rc < 0)
	    throw system_error(string("Failed writing to the spool file '") + filename + "'");
	else
	    len += rc;
	}
    close(fd);

    return buf;
    }
