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

#ifndef REQUEST_CONFIRMATION_HPP
#define REQUEST_CONFIRMATION_HPP

#include <string>

void request_confirmation(const std::string& mail, const std::string& hash, const mail_addresses& _addresses);

#endif
