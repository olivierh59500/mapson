/*
 * Copyright (c) 2002 by Peter Simons <simons@ieee.org>.
 * All rights reserved.
 */

#ifndef LOG_HH
#define LOG_HH

// Our logging routines.

void _debug(const char* fmt, ...) throw();
void info(const char* fmt, ...)  throw();
void error(const char* fmt, ...) throw();

// Use the preprocessor the remove all debugging calls when compiled
// without debugging support. We can't depend on the optimizer to do
// that because of the variadic arguments.

#ifdef DEBUG
#    define debug(x) _debug x;
#else
#    define debug(x) if (false)
#endif

#endif
