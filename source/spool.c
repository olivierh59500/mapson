/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1997 by Peter Simons, Germany
 *      All rights reserved.
 */

#include <errno.h>
#include <syslog.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <myexceptions.h>
#include "mapson.h"

void
store_mail_in_spool(char * mail_buffer, char * id)
{
    char *  home_dir;
    char *  filename;

    home_dir = get_home_directory();
    filename = fail_safe_sprintf("%s/.mapson/spool/%s", home_dir, id);
    free(home_dir);

    remove(filename);		/* We don't keep several copies of the
				   same mail. */
    save_to(mail_buffer, filename);
    free(filename);
}
