dnl
dnl configure.ac -- Process this file with autoconf to produce a configure script.
dnl
AC_PREREQ(2.52)
AC_INIT(mapson.cc)

dnl Get rid of the lousy '-g -O2' defaults.
dnl
CFLAGS=$CFLAGS
CXXFLAGS=$CXXFLAGS

dnl Find the canonical name of our host.
dnl
AC_MSG_CHECKING(the type of host we're running on)
hosttuple=`/bin/sh ./.shtool guessos`
AC_MSG_RESULT(${hosttuple})

dnl Figure out which development environment we're using.
dnl
AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB

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

dnl If the system misses the getopt.h include or the getopt_long()
dnl function, we use the version included in the distribution.
dnl
AC_CHECK_HEADER(getopt.h, have_getopt_h=yes, have_getopt_h=no, [#include <unistd.h>])
AC_CHECK_FUNC(getopt_long, have_getopt=yes, have_getopt=no)
if test "x$have_getopt_h" != "xyes" -o "x$have_getopt" != "xyes"; then
    CPPFLAGS="$CPPFLAGS -Ilibgetopt"
    AC_LIBOBJ(libgetopt/getopt)
    AC_LIBOBJ(libgetopt/getopt1)
fi

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
    AC_CHECK_FILE(/var/mail, mailboxdir=/var/mail, [
	AC_CHECK_FILE(/var/spool/mail, mailboxdir=/var/spool/mail, [
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


dnl Enable/disable debugging.
dnl
AC_MSG_CHECKING(whether to include debugging capabilities)
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


dnl Write results.
dnl
AC_CONFIG_FILES(Makefile)
AC_OUTPUT
