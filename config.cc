/*
 * Copyright (c) 2001 by Peter Simons <simons@ieee.org>.
 * All rights reserved.
 */

// POSIX.1 system headers.
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>

// POSIX.2 system headers.
#include <getopt.h>

// My own libraries.
#include "system-error/system-error.hh"
#include "config.hh"
#include "log.hh"

using namespace std;

// The global configuration.

const configuration* config = 0;

// Use the following sentry to make sure endpwent() is called in case
// our routine throws an exception before we can do it.

namespace
    {
    struct pwd_sentry
        {
        explicit pwd_sentry(struct passwd* arg) throw() : pwd(arg) { }
        ~pwd_sentry() throw() { if (pwd) endpwent(); }
        struct passwd* pwd;
        };
    }

// Construct configuration.

configuration::configuration(int argc, char** argv)
    {
    // Initialize the variables with the default settings.

    pwd_sentry sentry(getpwuid(getuid()));
    if (sentry.pwd == 0)
        throw system_error("Can't get my user name");
    config_file.assign(sentry.pwd->pw_dir).append("/.mapson/config");
    spool_dir.assign(sentry.pwd->pw_dir).append("/.mapson/spool");
    address_db.assign(sentry.pwd->pw_dir).append("/.mapson/address-db");
    request_for_confirmation_file.assign(sentry.pwd->pw_dir).append("/.mapson/request-for-confirmation.txt");
    mailbox.assign("/var/spool/mail/").append(sentry.pwd->pw_name);
    mta = "/usr/sbin/sendmail -i -t";
    strict_rfc_parser = false;
    let_incorrect_mails_pass = true;
    runtime_error_rc = 75;
    syntax_error_rc = 65;

    // Parse the command line into temporary variables except for the
    // location of the config file.

    const char* optstring = "c:";
    const option longopts[] =
        {
        { "config-file", required_argument, 0, 'c' },
        { 0, 0, 0, 0 }          // mark end of array
        };
    int rc;
    opterr = 0;
    while ((rc = getopt_long(argc, argv, optstring, longopts, 0)) != -1)
        {
        switch(rc)
            {
            case 'c':
                config_file = optarg;
                break;
            default:
                error("Usage: %s [-c config-file] [mail [mail ...]]\n", argv[0]);
                throw runtime_error("Incorrect command line syntax.");
            }
        }

    parameter_index = optind;

    // Log the final settings for debugging purposes.

    debug(("My configuration:"));
    debug(("    mailbox = '%s'", mailbox.c_str()));
    debug(("    config_file = '%s'", config_file.c_str()));
    debug(("    spool_dir = '%s'", spool_dir.c_str()));
    debug(("    address_db = '%s'", address_db.c_str()));
    debug(("    request_for_confirmation_file = '%s'", request_for_confirmation_file.c_str()));
    debug(("    mta = '%s'", mta.c_str()));
    debug(("    strict_rfc_parser = '%s'", (strict_rfc_parser) ? "true" : "false"));
    debug(("    let_incorrect_mails_pass = '%s'", (let_incorrect_mails_pass) ? "true" : "false"));
    debug(("    runtime_error_rc = '%d'", runtime_error_rc));
    debug(("    syntax_error_rc = '%d'", syntax_error_rc));
    }

// The destructor is pretty straightforward.

configuration::~configuration() throw()
    {
    }
