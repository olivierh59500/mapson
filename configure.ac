dnl
dnl configure.ac -- Process this file with autoconf to produce a configure script.
dnl

dnl Get our current version

AC_INIT([mapson], [3.1-beta-2], [simons@cryp.to])
AM_INIT_AUTOMAKE([foreign 1.9])
AC_CONFIG_SRCDIR([mapson.cc])

dnl Get rid of the lousy '-g -O2' defaults.
dnl
CFLAGS=$CFLAGS
CXXFLAGS=$CXXFLAGS

dnl Figure out which development environment we're using.
dnl
AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB
AC_LANG(C++)

dnl Does the system have a working <getopt.h> include and
dnl getopt_long() function?
dnl
AC_CHECKING([whether getopt library supports long-style parameters])
AC_CHECK_HEADER(getopt.h, have_gnu_getopt=yes, have_gnu_getopt=no, [#include <unistd.h>])
AC_CHECK_FUNC(getopt_long, have_gnu_getopt=yes, have_gnu_getopt=no)
AM_CONDITIONAL(HAVE_GNU_GETOPT, test x$have_gnu_getopt = xyes)

dnl If the system is missing setenv(3) or unsetenv(3), use
dnl the version included in this distribution instead.
dnl
AC_CHECK_FUNC(setenv, have_setenv=yes, have_setenv=no)
AM_CONDITIONAL(HAVE_SETENV, test x$have_setenv = xyes)
AC_CHECK_FUNC(unsetenv, have_unsetenv=yes, have_unsetenv=no)
AM_CONDITIONAL(HAVE_UNSETENV, test x$have_unsetenv = xyes)

dnl Perform some incredibly experimental magic to guess installation
dnl paths in an unexpectedly clever way. Henceforth evaluate the paths
dnl right away so that we can guarantee _absolute_ paths for our
dnl customers.
dnl
if test "$prefix" = "NONE"; then
    prefix=/usr/local
    if test "$exec_prefix" = "NONE"; then
        exec_prefix=/usr/local
    fi
    if test "$sysconfdir" = '${prefix}/etc'; then
        sysconfdir=/etc/mapson
    fi
fi
if test "$exec_prefix" = "NONE"; then
    exec_prefix=$prefix
fi
case $prefix in
    *mapson*)
        ;;
    *)
        if test "$sysconfdir" = '${prefix}/etc'; then
            sysconfdir=$sysconfdir/mapson
        fi
        if test "$datadir" = '${prefix}/share'; then
            datadir=$datadir/mapson
        fi
        ;;
esac

dnl Make sure u_int32_t is defined.
dnl
AC_CHECK_TYPE(u_int32_t, [],
    [
    AC_CHECK_TYPE(uint32_t, [CPPFLAGS="$CPPFLAGS -Du_int32_t=uint32_t"],
        [
        AC_MSG_ERROR([Can't find an appropriate type for u_int32_t.])
        ], [#include <sys/types.h>])
    ], [#include <sys/types.h>])

dnl Do we have large file system support?
dnl
case "$hosttuple" in
     *-*-solaris*)
        ;;
     *)
        AC_SYS_LARGEFILE
        ;;
esac

dnl Find some paths we might need.
dnl
AC_SUBST(mailboxdir)
AC_ARG_WITH(mailboxdir, [  --with-mailboxdir=DIR   location of the user mailboxes],
    [
    if test "$withval" = "no" -o "$withval" = "yes"; then
        AC_MSG_ERROR([The --with-mailboxdir option expects a path as parameter!])
    fi
    AC_MSG_CHECKING(for the location of the user mailboxes)
    mailboxdir=$withval
    AC_MSG_RESULT($mailboxdir)
    if test ! -d $mailboxdir; then
        AC_MSG_WARN(The specified mailbox directory '$mailboxdir' does not exist.)
    fi
    ],
    [
    AC_CHECK_FILE(/var/spool/mail, mailboxdir=/var/spool/mail, [
        AC_CHECK_FILE(/var/mail, mailboxdir=/var/mail, [
            AC_MSG_ERROR([Cannot determine the location of the mailboxes on this system.])
        ])])
    ])

AC_SUBST(mta)
AC_ARG_WITH(mta, [  --with-mta=PATH         location of the mail transport agent (sendmail?)],
    [
    if test "$withval" = "no" -o "$withval" = "yes"; then
        AC_MSG_ERROR([The --with-mta option expects a path as parameter!])
    fi
    AC_MSG_CHECKING(for the location of the mail transport agent)
    mta=$withval
    AC_MSG_RESULT($mta)
    if test ! -f $mta; then
        AC_MSG_WARN(The specified MTA '$mta' does not exist.)
    elif test ! -x $mta; then
        AC_MSG_WARN(The specified MTA '$mta' is not executable.)
    fi
    ],
    [
    AC_PATH_PROG(mta, sendmail, none, $PATH:/usr/lib:/etc/usr/etc)
    if test "$mta" = "none"; then
        AC_MSG_ERROR(Cannot locate your mail transport agent.)
    fi
    ])

dnl
dnl Include debug code?
dnl
AC_MSG_CHECKING([whether to include debugging capabilities])
AC_ARG_WITH(debug, [  --with-debug            Support debugging? (default: yes)],
    [
    if test "$withval" = "no"; then
       AC_MSG_RESULT(no)
    elif test "$withval" = "yes"; then
       CPPFLAGS="$CPPFLAGS -DDEBUG"
       AC_MSG_RESULT(yes)
    fi
    ],
    [
    CPPFLAGS="$CPPFLAGS -DDEBUG"
    AC_MSG_RESULT(yes)
    ])

dnl Grande Finale
dnl
AC_CONFIG_FILES([librfc822/Makefile])
AC_CONFIG_FILES([libvarexp/Makefile libvarexp/tests/Makefile])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
