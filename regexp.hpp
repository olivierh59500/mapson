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

#ifndef REGEXPP_HPP_INCLUDED
#define REGEXPP_HPP_INCLUDED

#include <stdexcept>
#include <vector>
#include <string>
#include <memory>
#include <sys/types.h>
#include <regex.h>

class RegExp
{
public:
  struct init_failure : public std::runtime_error
  {
    init_failure(const std::string& s) : std::runtime_error(s) { }
  };

  explicit RegExp(const std::string& pattern, int flags = REG_EXTENDED | REG_NEWLINE | REG_ICASE | REG_NOSUB)
  {
    int rc = regcomp(&preg, pattern.c_str(), flags);
    if (rc != 0)
    {
      char errormsg[256];
      strcpy(errormsg, "RegExp: ");
      regerror(rc, &preg, errormsg+8, sizeof(errormsg)-8);
      regfree(&preg);
      throw init_failure(errormsg);
    }
  }

  ~RegExp() throw()
  {
    regfree(&preg);
  }

  // The number of submatches found when compiling the regular
  // expression. Use this to determine the size of the pmatch array
  // when doing submatching regexec() calls.

  unsigned int submatch_num() const throw() { return preg.re_nsub; }

  // Perform a submatch. Returns 'true' if the string matched. Partial matches
  // are placed into the provided array. The string at offset 0 is the part
  // that matched all of the expression.
  //
  // If the expression doesn't match the string, 'false' is returned.

  bool submatch(const std::string& s, std::vector<std::string>& res_vec, int eflags = 0) const
  {
    std::auto_ptr<regmatch_t> pmatch(new regmatch_t[submatch_num()+1]);

    if (regexec(&preg, s.c_str(), submatch_num()+1, pmatch.get(), eflags) != 0)
      return false;

    res_vec.clear();
    for (std::vector<std::string>::size_type i = 0; i < submatch_num()+1; ++i)
    {
      res_vec.push_back(s.substr(pmatch.get()[i].rm_so, pmatch.get()[i].rm_eo - pmatch.get()[i].rm_so));
    }
    return true;
  }

  operator const regex_t* () const throw()
  {
    return &preg;
  }

private:			// don't copy me
  RegExp(const RegExp& rhs);
  RegExp& operator= (const RegExp& rhs);

protected:
  regex_t     preg;
  friend inline bool operator== (const std::string&, const RegExp&) throw();
  friend inline bool operator== (const RegExp&, const std::string&) throw();
  friend inline bool operator!= (const std::string&, const RegExp&) throw();
  friend inline bool operator!= (const RegExp&, const std::string&) throw();
};


// Match a string against a RegExp instance.

inline bool operator== (const std::string& s, const RegExp& e) throw()
{
  return (regexec(&e.preg, s.c_str(), 0, 0, 0) == 0) ? true : false;
}
inline bool operator== (const RegExp& e, const std::string& s) throw()
{
  return (regexec(&e.preg, s.c_str(), 0, 0, 0) == 0) ? true : false;
}
inline bool operator!= (const std::string& s, const RegExp& e) throw()
{
  return !(s == e);
}
inline bool operator!= (const RegExp& e, const std::string& s) throw()
{
  return !(e == s);
}

#endif // REGEXPP_HPP_INCLUDED
