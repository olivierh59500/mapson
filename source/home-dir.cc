/*
 * Copyright (C) 2001 by Peter Simons <simons@computer.org>.
 * All rights reserved.
 */

// ISO C++ headers.
#include <cerrno>

// POSIX.1 system headers.
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <fcntl.h>
#include <unistd.h>

// My own libraries.
#include "system-error/system-error.hh"
#include "home-dir.hh"

using namespace std;

string get_my_user_name()
    {
    string username;
    struct passwd* pwd;

    pwd = getpwuid(getuid());
    if (pwd != 0)
	{
	username = pwd->pw_name;
	endpwent();
	}
    else
	throw system_error("Can't get my user name");
    return username;
    }

string get_home_directory()
    {
    string home_dir;
    struct passwd* pwd;

    pwd = getpwuid(getuid());
    if (pwd != 0)
	{
	home_dir = pwd->pw_dir;
	endpwent();
	}
    else
	throw system_error("Can't get my home directory");
    return home_dir;
    }

string assert_mapson_home_dir_exists()
    {
    string mapson_home_dir = get_home_directory() + "/.mapson";
    struct stat sb;
    if(stat(mapson_home_dir.c_str(), &sb) == -1)
	{
	if (errno != ENOENT)
	    throw system_error("stat() to the mapSoN home directory failed");

	if (mkdir(mapson_home_dir.c_str(), S_IRWXU) == -1)
	    throw system_error(string("Can't create the mapSoN home directory '") +
			       mapson_home_dir + "'");
	}
    else
	{
	if ((sb.st_mode & S_IFDIR) == 0)
	    throw runtime_error(string("The mapSoN home directory '") +
				mapson_home_dir + "' is not a directory.");
	}
    return mapson_home_dir;
    }
