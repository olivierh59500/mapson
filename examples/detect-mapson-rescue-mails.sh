#! /bin/sh
#
# This script is intended to be run hourly/daily by the cron(8)
# utility. It will detect any "rescue files", present in the
# $HOME/.mapson directory and notify you of them.
#
# $Header$
#

#
# Make sure the TMPFILE is deleted when we exit.
#
TMPFILE=/tmp/detect-rescue-mails.$$
trap 'rm -f $TMPFILE; exit' 0 2 3 5 10 13 15


#
# Sanity checks.
#
if [ -z "$HOME" ]; then
    echo >&2 "The \$HOME variable isn't set! Please edit your crontab"
    echo >&2 "to set \$HOME to the path of your home directory."
    exit 1;
fi

if [ ! -d "$HOME/.mapson/spool" ]; then
    echo >&2 "Can't find directory \"$HOME/.mapson/spool\" directory!"
    exit 1;
fi


#
# Now find any rescue mails we may have.
#
RESCUE_FILES=`find "$HOME/.mapson" -type f -and -name rescue_file_* -print`
if [ -z "$RESCUE_FILES" ]; then
    exit			# nothing to do
fi


#
# Create a digest and print it to stdout, so that cron will mail it to
# the user.
#
echo  >$TMPFILE "The following mails couldn't be processed due to an error:"
for file in $RESCUE_FILES; do
    echo >>$TMPFILE ""
    echo >>$TMPFILE "=============== $file ==============="
    echo >>$TMPFILE ""
    sed -e 's/^From />From /g' \
	-e 's/mapSoN-Confirm-Cookie/Escaped-Mapson-Cookie/g' \
	<$file >>$TMPFILE
    echo >>$TMPFILE ""
    DELETE_ME="$DELETE_ME $file"
done

cat $TMPFILE
rm -f $TMPFILE $DELETE_ME
