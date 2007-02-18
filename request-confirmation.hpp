/*
 * Copyright (C) 2002 by Peter Simons <simons@cryp.to>.
 * All rights reserved.
 */

#ifndef REQUEST_CONFIRMATION_HH
#define REQUEST_CONFIRMATION_HH

// ISO C++ headers.
#include <string>

void request_confirmation(const std::string& mail, const std::string& hash, const mail_addresses& _addresses);

#endif
