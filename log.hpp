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

#ifndef LOG_HPP
#define LOG_HPP

// Our logging routines.

void init_logging(const char* file);
void _debug(const char* fmt, ...);
void info(const char* fmt, ...) ;
void error(const char* fmt, ...);

// Use the preprocessor the remove all debugging calls when compiled
// without debugging support. We can't depend on the optimizer to do
// that because of the variadic arguments.

#ifdef DEBUG
#    define debug(x) _debug x
#else
#    define debug(x) ((void)(0))
#endif

#endif
