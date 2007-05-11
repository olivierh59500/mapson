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

#ifndef MAPSON_HPP_INCLUDED
#define MAPSON_HPP_INCLUDED

#include <set>
#include <string>
#include <ctime>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "parse-config-file.hpp"
#include "system-error.hpp"

// ----- Logging Interface

void init_logging(const char* file);
void _debug(const char* fmt, ...);
void info(const char* fmt, ...) ;
void error(const char* fmt, ...);

// Use the preprocessor the remove all debugging calls when compiled
// without debugging support. We can't depend on the optimizer to do
// that because of the variadic arguments.

#ifdef DEBUG
#  define debug(x) _debug x
#else
#  define debug(x) ((void)(0))
#endif

// ----- Runtime Configuration

class configuration : public AbstractConfig
{
public:
  // Exit with no error when displaying version or help.
  struct no_error { };

  explicit configuration(int, char**);
  ~configuration();
  void dump() const;

  // Paths.
  std::string config_file;
  std::string log_file;
  std::string spool_dir;
  std::string hashcash_db;
  std::string address_db;
  std::string whoami_db;
  std::string whitelist_db;
  std::string request_for_confirmation_file;
  std::string mta;
  std::string mailbox;

  // Filtering criterias.
  bool let_incorrect_mails_pass;
  time_t hc_expiry, hc_grace;
  unsigned int hc_req_bits;

  // Return codes.
  int runtime_error_rc;
  int syntax_error_rc;

  // Logging.
  bool debug;
  std::string message_id;
  bool have_message_id;

  // Run-time flags.
  bool accept;
  std::string cookie;
  bool address_db_auto_add;
  bool scan_for_cookie;

protected:
  friend int main(int, char**);
  int parameter_index;

private:
  configuration(configuration const &);
  configuration & operator= (configuration const &);
  virtual void set_option(std::string const &, std::string const &);
  virtual void unknown_line(std::string const &);
};

extern configuration const * config;

// ----- Address Database

class AddressDB
{
public:
  explicit AddressDB(const std::string& filename_arg);
  ~AddressDB() throw();
  bool find(const std::string& key) const;
  void insert(const std::string& key);

private:
  const std::string filename;
  int fd;
  std::string data;
};

// ----- E-Mail Handling

typedef std::set<std::string> addrset_t;

size_t find_next_header_line(std::string const & mail, size_t pos);

struct mail_addresses
{
  std::string   envelope;
  std::string   sender;
  std::string   return_path;
  addrset_t     from;
  addrset_t     reply_to;
  addrset_t     hashcash;
};

mail_addresses extract_sender_addresses(std::string const & mail);
std::string spool(std::string const & mail);
bool accept_confirmation(std::string& mail, std::string const & cmdline_cookie);
void request_confirmation(std::string const & mail, std::string const & hash, mail_addresses const &);
std::string lines2regex(std::string const & filename);
void deliver(const std::string& mail);

// ----- I/O Helper

int multi_open(std::string& pathname, int flags, mode_t mode);

struct fd_sentry
{
  int fd;
  explicit fd_sentry(int arg) : fd(arg) { }
  ~fd_sentry() { if (fd >= 0) close(fd); }
};

struct file_sentry
{
  FILE * file;
  explicit file_sentry(FILE * arg) : file(arg) { }
  ~file_sentry() { if (file) fclose(file); }
};

#endif // MAPSON_HPP_INCLUDED
