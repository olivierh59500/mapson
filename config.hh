/*
 * Copyright (c) 2001 by Peter Simons <simons@ieee.org>.
 * All rights reserved.
 */

#ifndef CONFIG_HH
#define CONFIG_HH

// ISO C++ headers.
#include <string>

class configuration
    {
  public:
    // Construction and Destruction.
    explicit configuration(int, char**);
    ~configuration() throw();

    // Paths.
    std::string config_file;
    std::string spool_dir;
    std::string address_db;
    std::string request_for_confirmation_file;
    std::string mta;
    std::string mailbox;

    // Filtering criterias.
    bool strict_rfc_parser;
    bool let_incorrect_mails_pass;

    // Return codes.
    int runtime_error_rc;
    int syntax_error_rc;

  protected:
    friend int main(int, char**);
    int parameter_index;

  private:
    configuration(const configuration&);
    configuration& operator= (const configuration&);
    };
extern const configuration* config;

#endif
