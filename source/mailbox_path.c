/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1997 by Peter Simons, Germany
 *      All rights reserved.
 */

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <assert.h>

#include <myexceptions.h>
#include <paths.h>
#include "mapson.h"

char *
get_mailbox_path(void)
{
    struct passwd *    pwd;
    char *             mailbox_path;

    pwd = getpwuid(getuid());
    if (pwd == NULL) {
	THROW(UNKNOWN_FATAL_EXCEPTION);
    }
    mailbox_path = fail_safe_sprintf("%s/%s", MAIL_DIR_PATH, pwd->pw_name);
    endpwent();
    return mailbox_path;
}
