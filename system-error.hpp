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

#ifndef SYSTEM_ERROR_HPP
#define SYSTEM_ERROR_HPP

#include <stdexcept>
#include <cerrno>
#include <string>

class system_error : public std::runtime_error
{
public:
  system_error() : runtime_error(str())
  {
  }

  explicit system_error(std::string const & msg)
    : runtime_error(msg + ": " + str())
  {
  }

private:
  static std::string str()
  {
    using namespace std;
    return string(strerror(errno));
  }
};

#endif
