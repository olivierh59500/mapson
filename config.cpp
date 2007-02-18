/*
 * Copyright (c) 2002 by Peter Simons <simons@cryp.to>.
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
#include "sanity/system-error.hpp"
#include "varexp/varexp.hpp"
#include "config.hpp"
#include "log.hpp"

#ifdef USE_MY_SETENV
#  include "setenv.h"
#endif

#ifdef USE_MY_UNSETENV
#  include "unsetenv.h"
#endif

static const char USAGE[] =
"Usage: mapson [ -h | --help ] [ --version ] [ -d | --debug ]\n"    \
"              [ -a | --accept ] [ --cookie cookie ]\n"             \
"              [ -c config | --config-file config ]\n"              \
"              [ --dont-scan ] [ mail ... ]\n" ;

using namespace std;

// The global configuration.

const configuration* config = 0;

// Use the following sentry to make sure endpwent() is called in case
// our routine throws an exception before we can do it.

namespace
{
  struct no_bool_error { } ;
  struct no_rc_error { } ;
  struct no_num_bits_error { } ;
  struct no_uint_error { } ;
  struct pwd_sentry
  {
    explicit pwd_sentry(struct passwd* arg) throw() : pwd(arg) { }
    ~pwd_sentry() throw() { if (pwd) endpwent(); }
    struct passwd* pwd;
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
  hashcash_db.assign(sentry.pwd->pw_dir).append("/.mapson/hashcash-db");
  request_for_confirmation_file.assign(sentry.pwd->pw_dir).append("/.mapson/challenge-template");
  request_for_confirmation_file.append(":" DATADIR "/reqmail.template");
  mailbox.assign(MAILBOXDIR "/").append(sentry.pwd->pw_name);
  mta = MTA " '-f<>' -i -t";
  let_incorrect_mails_pass = true;
  runtime_error_rc = 75;
  syntax_error_rc = 65;
  address_db_auto_add = true;
  accept = false;
  debug = false;
  scan_for_cookie = true;
  message_id = "<no-message-id@localhost>";
  have_message_id = false;
  hc_expiry = 60*60*24*7;
  hc_grace = 60*60*1;
  hc_req_bits = 20;

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
      { "dont-scan",   no_argument,       0, 's' },
      { 0, 0, 0, 0 }          // mark end of array
    };
  int rc;
  bool cmdline_debug = false;
  opterr = 0;
  while ((rc = getopt_long(argc, argv, optstring, longopts, 0)) != -1)
  {
    switch(rc)
    {
      case 's':
        scan_for_cookie = false;
        break;
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
        printf("mapSoN version %s\n", PACKAGE_VERSION);
        throw no_error();
      case 'h':
        printf("%s", USAGE);
        throw no_error();
      default:
        fprintf(stderr, "%s", USAGE);
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
  debug(("    AddressDBAutoAdd   = '%s'", (address_db_auto_add) ? "true" : "false"));
  debug(("    ChallengeTemplate  = '%s'", request_for_confirmation_file.c_str()));
  debug(("    MTA                = '%s'", mta.c_str()));
  debug(("    PassIncorrectMails = '%s'", (let_incorrect_mails_pass) ? "true" : "false"));
  debug(("    RuntimeErrorRC     = '%d'", runtime_error_rc));
  debug(("    SyntaxErrorRC      = '%d'", syntax_error_rc));
  debug(("    Debug              = '%s'", (debug) ? "true" : "false"));
  debug(("    Accept             = '%s'", (accept) ? "true" : "false"));
  debug(("    Cookie             = '%s'", cookie.c_str()));
  debug(("    ScanForCookie      = '%s'", (scan_for_cookie) ? "true" : "false"));
  debug(("    WhoamiDB           = '%s'", whoami_db.c_str()));
  debug(("    WhiteListDB        = '%s'", whitelist_db.c_str()));
  debug(("    HashCashDB         = '%s'", hashcash_db.c_str()));
  debug(("    HashCashExpiry     = '%u'", hc_expiry));
  debug(("    HashCashGrace      = '%u'", hc_grace));
  debug(("    ReqHashCashBits    = '%u'", hc_req_bits));
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

inline unsigned int get_num_bits(const string& value)
{
  char* endptr;
  unsigned long int i = strtoul(value.c_str(), &endptr, 10);
  if (*endptr != '\0' || i > 160)
    throw no_num_bits_error();
  return i;
}

inline unsigned int get_uint(const string& value)
{
  char* endptr;
  unsigned long int i = strtoul(value.c_str(), &endptr, 10);
  if (*endptr != '\0')
    throw no_uint_error();
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
    else if (strcasecmp("ChallengeTemplate", keyword.c_str()) == 0)
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
    else if (strcasecmp("HashCashDB", keyword.c_str()) == 0)
    {
      hashcash_db = data;
    }
    else if (strcasecmp("HashCashExpiry", keyword.c_str()) == 0)
    {
      hc_expiry = get_uint(data);
    }
    else if (strcasecmp("HashCashGrace", keyword.c_str()) == 0)
    {
      hc_grace = get_uint(data);
    }
    else if (strcasecmp("ReqHashCashBits", keyword.c_str()) == 0)
    {
      hc_grace = get_num_bits(data);
    }
    else if (strcasecmp("WhoamiDB", keyword.c_str()) == 0)
    {
      whoami_db = data;
    }
    else if (strcasecmp("WhiteListDB", keyword.c_str()) == 0)
    {
      whitelist_db = data;
    }
    else
      throw runtime_error(string("The config file uses the unknown keyword '")
                         + keyword + "'; aborting.");
  }
  catch(const varexp::error& e)
  {
    error("Undefined variable in config option '%s %s'.",
         keyword.c_str(), _data.c_str());
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
  catch(const no_num_bits_error&)
  {
    throw runtime_error(string("Parameter for keyword '") + keyword +
                       "' in config file must be an integer in the range 0 <= rc <= 160.");
  }
  catch(const no_uint_error&)
  {
    throw runtime_error(string("Parameter for keyword '") + keyword +
                       "' in config file must be an unsigned integer.");
  }
}

void configuration::unknown_line(const string& line)
{
  info("Unknown line in config file: '%s'.", line.c_str());
}
