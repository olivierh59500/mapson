/*
 * Copyright (c) 2001 by Peter Simons <simons@ieee.org>.
 * All rights reserved.
 */

// POSIX.1 system headers.
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>

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

configuration::configuration()
    {
    // Determine path of the user's home directory to construct the
    // other paths relative to it.

    pwd_sentry sentry(getpwuid(getuid()));
    if (sentry.pwd == 0)
        throw system_error("Can't get my user name");
    config_file.assign(sentry.pwd->pw_dir).append("/.mapson/config");
    spool_dir.assign(sentry.pwd->pw_dir).append("/.mapson/spool");
    address_db.assign(sentry.pwd->pw_dir).append("/.mapson/address-db");
    request_for_confirmation_file.assign(sentry.pwd->pw_dir).append("/.mapson/request-for-confirmation.txt");

    // Set the default MTA.

    mta = "/usr/sbin/sendmail -i";

    // Should we enforce correct a Sender: header?

    strict_rfc_parser = false;

    // Per default, we bounce syntactically incorrect mail.

    let_incorrect_mails_pass = true;

    // Use temporary failure in case of runtime errors.

    runtime_error_rc = 75;

    // Fail fatally if the mail is incorrect.

    syntax_error_rc = 65;

    // Log the final settings for debugging purposes.

    debug(("config_file = '%s'", config_file.c_str()));
    debug(("spool_dir = '%s'", spool_dir.c_str()));
    debug(("address_db = '%s'", address_db.c_str()));
    debug(("request_for_confirmation_file = '%s'", request_for_confirmation_file.c_str()));
    debug(("mta = '%s'", mta.c_str()));
    debug(("strict_rfc_parser = '%s'", (strict_rfc_parser) ? "true" : "false"));
    debug(("let_incorrect_mails_pass = '%s'", (let_incorrect_mails_pass) ? "true" : "false"));
    debug(("runtime_error_rc = '%d'", runtime_error_rc));
    debug(("syntax_error_rc = '%d'", syntax_error_rc));
    }

// The destructor is pretty straightforward.

configuration::~configuration() throw()
    {
    }
