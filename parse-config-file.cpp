/*
 * Copyright (c) 2010 by Peter Simons <simons@cryp.to>.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "parse-config-file.hpp"
#include "regexp.hpp"
#include "system-error.hpp"
#include <fstream>

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
 *  class "AbstractConfig", which is defined in this header. The caller should
 *  derive his own version of this class and pass it into parse_config_file()
 *  in order to process the data found by the routine. In fact, AbstractConfig
 *  is pure virtual and cannot be instantiated as it is.
 */
void parse_config_file(char const * filename, AbstractConfig& config)
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
