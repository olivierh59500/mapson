/*
 * Copyright (c) 2010-2017 by Peter Simons <simons@cryp.to>.
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
