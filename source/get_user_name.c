/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1997 by Peter Simons, Germany
 *      All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <errno.h>

#include <myexceptions.h>
#include <paths.h>
#include "mapson.h"

char *
get_user_name(void)
{
    char *             username;
    struct passwd *    pwd;

    pwd = getpwuid(getuid());
    if (pwd == NULL) {
	THROW(UNKNOWN_FATAL_EXCEPTION);
    }
    username = fail_safe_strdup(pwd->pw_name);
    endpwent();
    return username;
}
