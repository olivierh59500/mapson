#! /bin/sh
#
# This script is intended to be run hourly/daily by the cron(8)
# utility. It will delete all files in $HOME/.mapson/spool that are
# older than a certain number of days. The number of days can be
# configured below.
#
# $Header$
#

EXPIRE_AFTER_SO_MANY_DAYS="+7"	       # The "+" sign is important!

if [ -z "$HOME" ]; then
    echo >&2 "The \$HOME variable isn't set! Please edit your crontab"
    echo >&2 "to set \$HOME to the path of your home directory."
    exit 1;
fi

if [ ! -d "$HOME/.mapson/spool" ]; then
    echo >&2 "Can't find directory \"$HOME/.mapson/spool\" directory!"
    exit 1;
fi

find "$HOME/.mapson/spool" -type f -and -mtime $EXPIRE_AFTER_SO_MANY_DAYS \
    -exec rm -f {} \;
