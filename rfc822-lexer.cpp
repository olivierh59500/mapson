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

#include "rfc822.hpp"
using namespace std;

ostream & operator<< (ostream & os, const token & t)
{
  switch(t.type)
  {
    case token::atom:
      os << "atom";
      break;
    case token::character:
      os << "char";
      break;
    case token::domain_literal:
      os << "domain-literal";
      break;
    case token::quoted_string:
      os << "quoted-string";
      break;
    default:
      throw logic_error("token type is unknown!");
  }
  os << "('" << t.rep << "')";
  return os;
}

inline bool is_CHAR(char c) { return (c >= 0 /* && c <= 127 */) ? true : false;  }
inline bool is_CTL(char c) { return (c >= 0 && c <= 31) ? true : false; }
inline bool is_SPACE(char c) { return (c == ' ' || c == '\t') ? true : false; }
inline bool is_LWSP(char c) { return (c == 32 || c == 9) ? true : false; }

inline bool is_BIGCHAR(char c)
{
  return ((unsigned char)c >= 160 /* && (unsigned char)c <= 255) */) ? true : false;
}

inline bool is_special(char c)
{
  return (c == '(' || c == ')' || c == '<' || c == '>' || c == '@' ||
         c == ',' || c == ';' || c == ':' || c == '\\' || c == '\"' ||
         c == '.' || c == '[' || c == ']')
    ? true : false;
}

inline bool is_atom(char c)
{
  return (is_CHAR(c) && !is_special(c) && !is_SPACE(c) && !is_CTL(c))
    ? true : false;
}

inline bool is_ctext(char c)
{
  return ((is_CHAR(c) || is_BIGCHAR(c)) && c != '(' && c != ')' && c != '\\')
    ? true : false;
}

inline bool is_dtext(char c)
{
  return (is_CHAR(c) && c != '[' && c != ']' && c != '\\')
    ? true : false;
}

inline bool is_qtext(char c)
{
  return (is_CHAR(c) && c != '"' && c != '\\')
    ? true : false;
}

inline string::const_iterator eat_comment(string::const_iterator p, string::const_iterator end)
{
  while(p <= end)
  {
    if (is_ctext(*p))
      ++p;
    else if (*p == '\\')
    {
      if (p < end)
      {
        p += 2;
      }
      else
        throw rfc822_syntax_error("Incomplete 'quoted-pair'.");
    }
    else if (*p == '(')
      p = eat_comment(p+1, end);
    else if (*p == ')')
      return ++p;
    else
      throw rfc822_syntax_error("Unallowed character in comment.");
  }
  return p;
}

tokstream_t lex(const string & input)
{
  tokstream_t tokstream;
  string::const_iterator p = input.begin();
  char c;

  for (c = *p++; p <= input.end(); c = *p++)
  {
    if (c == '(')
    {
      p = eat_comment(p, input.end());
    }
    if (is_atom(c))
    {
      token t(token::atom, string(1, c));
      for (c = *p++; p <= input.end() && is_atom(c); c = *p++)
        t.rep.append(1, c);
      tokstream.push_back(t);
      if (!is_atom(p[-1]))
        --p;
      else
        break;
    }
    else if (c == '<' || c == '>' || c == '@' || c == ',' ||
            c == ';' || c == ':' || c == '.')
    {
      tokstream.push_back(token(token::character, string(1, c)));
    }
    else if (c == '\r')
    {
      if (p < input.end() && *p == '\n')
        continue;
      else
        throw rfc822_syntax_error("Line break without proper continuation.");
    }
    else if (c == '\n')
    {
      if (p < input.end())
      {
        c = *p++;
        if (is_LWSP(c))
        {
          while(is_LWSP(*p))
            ++p;
        }
        else
          throw rfc822_syntax_error("Line break without proper continuation.");
      }
      else
        break;
    }
    else if (c == ' ' || c == '\t')
    {
      // ignore
    }
    else if (c == '[')
    {
      token t(token::domain_literal, string(1, c));
      for (c = *p++;
           p <= input.end() &&
             (is_dtext(c) || c == '\\');
           c = *p++)
      {
        t.rep.append(1, c);
        if (c == '\\')
        {
          if (p == input.end())
            throw rfc822_syntax_error("Incomplete 'quoted-pair'.");
          t.rep.append(1, *p);
          ++p;
        }
      }
      if (c != ']')
        throw rfc822_syntax_error("domain-literal lacks terminating ']'.");
      t.rep.append(1, c);
      tokstream.push_back(t);
    }
    else if (c == '"')
    {
      token t(token::quoted_string, string(1, c));
      for (c = *p++;
           p <= input.end() &&
             (is_qtext(c) || c == '\\');
           c = *p++)
      {
        t.rep.append(1, c);
        if (c == '\\')
        {
          if (p == input.end())
            throw rfc822_syntax_error("Incomplete 'quoted-pair'.");
          t.rep.append(1, *p);
          ++p;
        }
      }
      if (c != '"')
        throw rfc822_syntax_error("Quoted-string lacks terminating '\"'.");
      t.rep.append(1, c);
      tokstream.push_back(t);
    }
  }

  return tokstream;
}
