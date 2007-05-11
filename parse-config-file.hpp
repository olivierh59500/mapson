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
#include <fstream>
#include "regexp.hpp"
#include "system-error.hpp"

class AbstractConfig
{
public:
  virtual ~AbstractConfig() { }
protected:
  virtual void set_option(std::string const & keyword, std::string const & data) = 0;
  virtual void unknown_line(std::string const & line) = 0;
  friend void parse_config_file(char const *, AbstractConfig&);
};

/**
 *  The routine parse_config_file() will parse a configuration file from the
 *  file system and provide the results via a user-supplied callback. It
 *  understands any sort of input file that obeys to the following line format:
 *
 *  <code>
 *    keyword data
 *  </code>
 *
 *  "keyword" and "data" have to be separated by one or more white-space
 *  characters -- either tabs or blanks --, and "keyword" has to begin at the
 *  first column of the line. The "data"-part may be enclosed in quotation
 *  marks, but this is usually not necessary, even if it contains whitespace
 *  itself. The only occasion, when the data part must be quoted is in case it
 *  contains white-space as first or last characters.
 *
 *  Any line that starts with a '#' character as the first non-whitespace-char
 *  is ignored and assumed to be a comment. Note that comments after the data
 *  part are not recognized!
 *
 *  Anything else is an "unknown" line and will be reported but not parsed by
 *  the routine.
 *
 *  parse_config_file() reports its findings to the caller via the callback
 *  class "AbstractConfig", which is defined in this header. * The caller
 *  should derive his own version of this class and pass it * into
 *  parse_config_file() in order to process the data found by the * routine. In
 *  fact, AbstractConfig is pure virtual and cannot be * instantiated as it is.
 */
inline void parse_config_file(char const * filename, AbstractConfig& config)
{
  // Open the config file.

  std::ifstream file(filename);

  if (!file)
    throw system_error(std::string("parse_config_file() failed to open '") + filename + "'");

  // Now we read line by line and process each one seperately.

  RegExp const  parser("^([^\t ]+)[\t ]+([^\t ].*)$", REG_EXTENDED);
  RegExp const  comment("^[\t ]*#.*$", REG_EXTENDED);
  RegExp const  empty("^[\t ]*$", REG_EXTENDED);
  std::vector<std::string>  results;
  std::string               line;

  for (getline(file, line); file; getline(file, line))
  {
    if (line == empty)
      continue;
    else if (line == comment)
      continue;
    else if (parser.submatch(line, results))
    {
      // A hit. Find the substrings.

      std::string & keyword = results[1];
      std::string & data    = results[2];

      // Remove trailing whitespace.

      if (data.empty()) return;
      std::string::size_type i = data.size();
      while(i > 0 && (data[i-1] == ' ' || data[i-1] == '\t'))
        --i;
      if (i < data.size())
        data.erase(i);

      // Call the set_option() method in the provided class.

      config.set_option(keyword, data);
    }
    else
      config.unknown_line(line);
  }
}

#endif // PARSE_CONFIG_FILE_HPP_INCLUDED
