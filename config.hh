/*
 * Copyright (c) 2002 by Peter Simons <simons@ieee.org>.
 * All rights reserved.
 */

#ifndef CONFIG_HH
#define CONFIG_HH

// ISO C++ headers.
#include <string>

// My own libraries.
#include "libparse-config-file/parse-config-file.hh"

class configuration : public AbstractConfig
    {
  public:
    // Construction and Destruction.
    explicit configuration(int, char**);
    ~configuration() throw();
    void dump() const;

    // Paths.
    std::string config_file;
    std::string log_file;
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

    // Logging.
    bool debug;
    std::string message_id;
    bool have_message_id;

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
