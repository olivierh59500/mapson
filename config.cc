/*
 * Copyright (c) 2001 by Peter Simons <simons@ieee.org>.
 * All rights reserved.
 */

// POSIX.1 system headers.
#include <sys/types.h>
#include <sys/stat.h>
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
    struct no_bool_error
        {
        };
    struct no_rc_error
        {
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
    debug = false;

    // Parse the command line into temporary variables except for the
    // location of the config file.

    const char* optstring = "c:d";
    const option longopts[] =
        {
        { "config-file", required_argument, 0, 'c' },
        { "debug",       no_argument,       0, 'd' },
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
            case 'd':
                debug = true;
                break;
            default:
                error("Usage: %s [-c config-file] [mail [mail ...]]\n", argv[0]);
                throw runtime_error("Incorrect command line syntax.");
            }
        }
    parameter_index = optind;

    // Parse the config file.

    struct stat mystat;
    if (stat(config_file.c_str(), &mystat) != -1)
        {
        parse_config_file(config_file.c_str(), *this);
        }
    }

// The destructor is pretty straightforward.

configuration::~configuration() throw()
    {
    }

// Log the configuration via debug().

void configuration::dump() const
    {
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
    debug(("    debug = '%s'", (debug) ? "true" : "false"));
    }

inline bool get_bool(const std::string& value)
    {
    if (strcasecmp(value.c_str(), "true") == 0)
        return true;
    else if (strcasecmp(value.c_str(), "false") == 0)
        return false;
    else
        throw no_bool_error();
    }

inline unsigned int get_rc(const std::string& value)
    {
    char* endptr;
    unsigned long int i = strtoul(value.c_str(), &endptr, 10);
    if (*endptr != '\0' || i > 128)
        throw no_rc_error();
    return i;
    }

void configuration::set_option(const std::string& keyword, const std::string& data)
    {
    try
        {
        if (strcasecmp("debug", keyword.c_str()) == 0)
            {
            debug = get_bool(data);
            }
        else if (strcasecmp("spool_dir", keyword.c_str()) == 0)
            {
            spool_dir = data;
            }
        else if (strcasecmp("address_db", keyword.c_str()) == 0)
            {
            address_db = data;
            }
        else if (strcasecmp("request_for_confirmation_file", keyword.c_str()) == 0)
            {
            request_for_confirmation_file = data;
            }
        else if (strcasecmp("mta", keyword.c_str()) == 0)
            {
            mta = data;
            }
        else if (strcasecmp("mailbox", keyword.c_str()) == 0)
            {
            mailbox = data;
            }
        else if (strcasecmp("strict_rfc_parser", keyword.c_str()) == 0)
            {
            strict_rfc_parser = get_bool(data);
            }
        else if (strcasecmp("let_incorrect_mails_pass", keyword.c_str()) == 0)
            {
            let_incorrect_mails_pass = get_bool(data);
            }
        else if (strcasecmp("runtime_error_rc", keyword.c_str()) == 0)
            {
            runtime_error_rc = get_rc(data);
            }
        else if (strcasecmp("syntax_error_rc", keyword.c_str()) == 0)
            {
            syntax_error_rc = get_rc(data);
            }
        else
            throw runtime_error(string("The Config file uses the unknown keyword '")
                                + keyword + "'; ignoring line.");
        }
    catch(const no_bool_error&)
        {
        throw runtime_error(string("Parameter for keyword '") + keyword +
                            "' in config file must be either 'true' or 'false'.");
        }
    catch(const no_rc_error&)
        {
        throw runtime_error(string("Parameter for keyword '") + keyword +
                            "' in config file must be an integer in the range 0 <= rc <= 128.");
        }
    }

void configuration::unknown_line(const std::string& line)
    {
    info("Unknown line in config file: '%s'.", line.c_str());
    }
