/*
 * Copyright (C) 2002 by Peter Simons <simons@computer.org>.
 * All rights reserved.
 */

#ifndef ACCEPT_CONFIRMATION_HH
#define ACCEPT_CONFIRMATION_HH

// ISO C++ headers.
#include <string>

bool accept_confirmation(std::string& mail, const std::string& cmdline_cookie);

#endif
