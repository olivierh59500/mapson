/*
 * Copyright (C) 2001 by Peter Simons <simons@computer.org>.
 * All rights reserved.
 */

// ISO C++ headers.

// POSIX.1 system headers.
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

// My own libraries.
#include "system-error/system-error.hh"
#include "extract-addresses.hh"
#include "process-ack.hh"

using namespace std;

namespace
    {
    struct fdsentry
	{
	fdsentry(int fd_arg) : fd(fd_arg) { }
	~fdsentry() { if (fd >= 0) close(fd); }
	operator int () const throw() { return fd; }
	int fd;
	};
    }

void process_ack(AddressDB& address_db, const char* filename)
    {
    // Read the file.

    fdsentry fd = open(filename, O_RDONLY);
    if (fd == -1)
	throw system_error(string("Can't open file '") + filename + "'");

    string mail;
    char buffer[8*1024];
    ssize_t rc;
    for (rc = read(fd, buffer, sizeof(buffer)); rc > 0; rc = read(fd, buffer, sizeof(buffer)))
	mail.append(buffer, rc);
    if (rc < 0)
	throw system_error(string("Failed to read file '") + filename + "'");


    // Extract the sender addresses from the header and find out
    // whether any of the found address is known already.

    addrset_t addresses;
    extract_sender_addresses(mail, addresses);
    if (addresses.empty())
	throw runtime_error(string("Could not extract any valid address from '") + filename + "'");

    addrset_t::const_iterator i;
    for (i = addresses.begin(); i != addresses.end(); ++i)
	{
	if (address_db.find(*i) == false)
	    address_db.insert(*i);
	}
    }
