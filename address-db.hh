/*
 * Copyright (C) 2002 by Peter Simons <simons@computer.org>.
 * All rights reserved.
 */

#ifndef ADDRESS_DB_HH
#define ADDRESS_DB_HH

// ISO C++ headers.
#include <string>

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

#endif
