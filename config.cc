/*
 * Copyright (c) 2002 by Peter Simons <simons@ieee.org>.
 * All rights reserved.
 */

// ISO C++ headers
#include <cstdio>
#include <cstdlib>
#include <memory>

// POSIX.1 system headers.
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <unistd.h>

// POSIX.2 system headers.
#include <getopt.h>

// My own libraries.
#include "system-error/system-error.hh"
#include "libvarexp/varexp.hh"
#include "config.hh"
#include "log.hh"
#include "version.h"

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
    struct env_lookup : public varexp::callback_t
        {
        virtual void operator()(const string& name, string& data)
            {
            const char* p = getenv(name.c_str());
            if (p == NULL)
                throw varexp::undefined_variable();
            else
                data = p;
            }
        virtual void operator()(const string& name, int idx, string& data)
            {
            throw runtime_error("Index lookups are not implemented for config files.");
            }
        };
    inline void mysetenv(const char* name, const char* value)
        {
        string tmp = string(name) + "=" + value;
        char* env = strdup(tmp.c_str());
        if (env == 0)
            throw system_error("strdup() failed");
        if (putenv(env) != 0)
            throw system_error("putenv() failed");
        }
    }

// Construct configuration.

configuration::configuration(int argc, char** argv)
    {
    // Initialize the variables with the default settings.

    pwd_sentry sentry(getpwuid(getuid()));
    if (sentry.pwd == 0)
        throw system_error("Can't get my user name");
    log_file.assign(sentry.pwd->pw_dir).append("/.mapson/log");
    spool_dir.assign(sentry.pwd->pw_dir).append("/.mapson/spool");
    address_db.assign(sentry.pwd->pw_dir).append("/.mapson/address-db");
    request_for_confirmation_file.assign(sentry.pwd->pw_dir).append("/.mapson/reqmail.template");
    request_for_confirmation_file.append(":" DATADIR "/reqmail.template");
    mailbox.assign(MAILBOXDIR "/").append(sentry.pwd->pw_name);
    mta = MTA " '-f<>' -i -t";
    strict_rfc_parser = false;
    let_incorrect_mails_pass = true;
    runtime_error_rc = 75;
    syntax_error_rc = 65;
    address_db_auto_add = true;
    accept = false;
    debug = false;
    message_id = "<no-message-id@localhost>";
    have_message_id = false;

    // Set the environment variables supported in the config file.

    mysetenv("MAILBOXDIR", MAILBOXDIR);
    mysetenv("DATADIR", DATADIR);
    mysetenv("MTA", MTA);
    mysetenv("HOME", sentry.pwd->pw_dir);
    mysetenv("USER", sentry.pwd->pw_name);

    // Parse the command line into temporary variables except for the
    // location of the config file.

    const char* optstring = "hc:da";
    const option longopts[] =
        {
        { "config-file", required_argument, 0, 'c' },
        { "debug",       no_argument,       0, 'd' },
        { "accept",      no_argument,       0, 'a' },
        { "cookie",      required_argument, 0, 'o' },
        { "help",        no_argument,       0, 'h' },
        { "version",     no_argument,       0, 'v' },
        { 0, 0, 0, 0 }          // mark end of array
        };
    int rc;
    bool cmdline_debug = false;
    opterr = 0;
    while ((rc = getopt_long(argc, argv, optstring, longopts, 0)) != -1)
        {
        switch(rc)
            {
            case 'c':
                config_file = optarg;
                break;
            case 'd':
                cmdline_debug = true;
                break;
            case 'a':
                accept = true;
                break;
            case 'o':
                cookie = optarg;
                break;
            case 'v':
                printf("mapSoN version %s\n", VERSION);
                throw no_error();
            case 'h':
                fprintf(stderr, "Usage: mapson [-h | --help] [--version] [-d | --debug] [-a | --accept]\n" \
                        "              [--cookie cookie] [-c config | --config-file config] [mail...]\n");
                throw no_error();
            default:
                fprintf(stderr, "Usage: mapson [-h | --help] [--version] [-d | --debug] [-a | --accept]\n" \
                        "              [--cookie cookie] [-c config | --config-file config] [mail...]\n");
                throw runtime_error("Incorrect command line syntax.");
            }
        }
    parameter_index = optind;

    // If a config file has been specified on the command line, use
    // that one exclusively. If not, try the default location in the
    // home and then the global one.

    if (!config_file.empty())
        {
        parse_config_file(config_file.c_str(), *this);
        }
    else
        {
        struct stat mystat;
        config_file.assign(sentry.pwd->pw_dir).append("/.mapson/config");
        if (stat(config_file.c_str(), &mystat) == -1)
            config_file = SYSCONFDIR "/mapson.config";
        parse_config_file(config_file.c_str(), *this);
        }

    // Don't let the config file overwrite the debug flag from the
    // command line.

    if (cmdline_debug)
        debug = true;

    // Init the log file routines.

    init_logging(log_file.c_str());
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
    debug(("    Mailbox            = '%s'", mailbox.c_str()));
    debug(("    ConfigFile         = '%s'", config_file.c_str()));
    debug(("    LogFile            = '%s'", log_file.c_str()));
    debug(("    SpoolDir           = '%s'", spool_dir.c_str()));
    debug(("    AddressDB          = '%s'", address_db.c_str()));
    debug(("    AddressDBAutoAdd   = '%s'", address_db_auto_add?"true":"false"));
    debug(("    ReqConfirmTemplate = '%s'", request_for_confirmation_file.c_str()));
    debug(("    MTA                = '%s'", mta.c_str()));
    debug(("    StrictRFCParser    = '%s'", (strict_rfc_parser) ? "true" : "false"));
    debug(("    PassIncorrectMails = '%s'", (let_incorrect_mails_pass) ? "true" : "false"));
    debug(("    RuntimeErrorRC     = '%d'", runtime_error_rc));
    debug(("    SyntaxErrorRC      = '%d'", syntax_error_rc));
    debug(("    Debug              = '%s'", (debug) ? "true" : "false"));
    debug(("    Accept             = '%s'", (accept) ? "true" : "false"));
    debug(("    Cookie             = '%s'", cookie.c_str()));
    }

inline bool get_bool(const string& value)
    {
    if (strcasecmp(value.c_str(), "true") == 0)
        return true;
    else if (strcasecmp(value.c_str(), "false") == 0)
        return false;
    else
        throw no_bool_error();
    }

inline unsigned int get_rc(const string& value)
    {
    char* endptr;
    unsigned long int i = strtoul(value.c_str(), &endptr, 10);
    if (*endptr != '\0' || i > 128)
        throw no_rc_error();
    return i;
    }

void configuration::set_option(const string& keyword, const string& _data)
    {
    try
        {
        // Expand environment variables in the data part.

        string data;
        env_lookup lookup;
        varexp::config_t myconfig;
        myconfig.startindex = myconfig.endindex = '\0';
        varexp::unescape(_data, data, false);
        varexp::expand(data, data, lookup);
        varexp::unescape(data, data, true);

        // Assign the value to our class.

        if (strcasecmp("Debug", keyword.c_str()) == 0)
            {
            debug = get_bool(data);
            }
        else if (strcasecmp("SpoolDir", keyword.c_str()) == 0)
            {
            spool_dir = data;
            }
        else if (strcasecmp("AddressDB", keyword.c_str()) == 0)
            {
            address_db = data;
            }
        else if (strcasecmp("AddressDBAutoAdd", keyword.c_str()) == 0)
            {
            address_db_auto_add = get_bool(data);
            }
        else if (strcasecmp("LogFile", keyword.c_str()) == 0)
            {
            log_file = data;
            }
        else if (strcasecmp("ReqConfirmTemplate", keyword.c_str()) == 0)
            {
            request_for_confirmation_file = data;
            }
        else if (strcasecmp("MTA", keyword.c_str()) == 0)
            {
            mta = data;
            }
        else if (strcasecmp("Mailbox", keyword.c_str()) == 0)
            {
            mailbox = data;
            }
        else if (strcasecmp("StrictRFCParser", keyword.c_str()) == 0)
            {
            strict_rfc_parser = get_bool(data);
            }
        else if (strcasecmp("PassIncorrectMails", keyword.c_str()) == 0)
            {
            let_incorrect_mails_pass = get_bool(data);
            }
        else if (strcasecmp("RuntimeErrorRC", keyword.c_str()) == 0)
            {
            runtime_error_rc = get_rc(data);
            }
        else if (strcasecmp("SyntaxErrorRC", keyword.c_str()) == 0)
            {
            syntax_error_rc = get_rc(data);
            }
        else
            throw runtime_error(string("The config file uses the unknown keyword '")
                                + keyword + "'; ignoring line.");
        }
    catch(const varexp::error& e)
        {
        error("Undefined variable in config option '%s %s'.", keyword.c_str(), _data.c_str());
        throw runtime_error("Cannot parse the config file.");
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

void configuration::unknown_line(const string& line)
    {
    info("Unknown line in config file: '%s'.", line.c_str());
    }
