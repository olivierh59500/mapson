/*
 * Copyright (C) 2001 by Peter Simons <simons@computer.org>.
 * All rights reserved.
 */

#ifndef GET_HOME_DIR_HH
#define GET_HOME_DIR_HH

// ISO C++ headers.
#include <string>

std::string get_my_user_name();
std::string get_home_directory();
std::string assert_mapson_home_dir_exists();

#endif
