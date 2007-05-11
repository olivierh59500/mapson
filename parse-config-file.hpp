/*
 * Copyright (c) 1999-2007 Peter Simons <simons@cryp.to>
 *
 * This software is provided 'as-is', without any express or
 * implied warranty. In no event will the authors be held liable
 * for any damages arising from the use of this software.
 *
 * Copying and distribution of this file, with or without
 * modification, are permitted in any medium without royalty
 * provided the copyright notice and this notice are preserved.
 */

#ifndef PARSE_CONFIG_FILE_HPP_INCLUDED
#define PARSE_CONFIG_FILE_HPP_INCLUDED

#include <string>

class AbstractConfig
{
public:
  virtual ~AbstractConfig() { }
protected:
  virtual void set_option(std::string const & keyword, std::string const & data) = 0;
  virtual void unknown_line(std::string const & line) = 0;
  friend void parse_config_file(char const *, AbstractConfig&);
};

void parse_config_file(char const * filename, AbstractConfig& config);

#endif // PARSE_CONFIG_FILE_HPP_INCLUDED
