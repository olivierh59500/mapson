/*
 * Copyright (c) 2002 by Peter Simons <simons@cryp.to>.
 * All rights reserved.
 */

#ifndef CONFIG_HH
#define CONFIG_HH

// ISO C++ headers.
#include <ctime>
#include <string>
#include <set>

// My own libraries.
#include "sanity/parse-config-file.hpp"

class configuration : public AbstractConfig
  {
 public:
  // Exit with no error when displaying version or help.
  struct no_error { };

  // Construction and Destruction.
  explicit configuration(int, char**);
  ~configuration() throw();
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
  configuration(const configuration&);
  configuration& operator= (const configuration&);
  virtual void set_option(const std::string&, const std::string&);
  virtual void unknown_line(const std::string&);
  };
extern const configuration* config;

#endif
