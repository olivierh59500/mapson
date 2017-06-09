dnl configure.ac -- Process this file with autoconf to produce a configure script.

AC_INIT([mapson], m4_esyscmd([build-aux/git-version-gen .tarball-version]), [simons@cryp.to], [mapson], [http://mapson.sourceforge.net/])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([1.11 -Wall gnu])
AC_CONFIG_SRCDIR([mapson.cpp])
AC_COPYRIGHT([dnl
Copyright (c) 2010-2017 Peter Simons

This program is free software: you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.])

AC_MSG_NOTICE([mapSoN version $VERSION])

AC_PROG_CC
gl_EARLY
AM_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_RANLIB
gl_INIT

dnl Magically transform installation paths to avoid having the
dnl name "mapson" in it several times.

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

dnl Enable large file support.

AC_SYS_LARGEFILE

dnl Find some paths we might need.

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

dnl Include debug code?

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

dnl Find docbook utilities

AC_PATH_PROG(xmlto, xmlto, none)
if test "$xmlto" = "none"; then
    AC_MSG_ERROR(cannot locate xmlto utility)
fi
AC_PATH_PROGS(docbook2man, docbook2man docbook2x-man, none)
if test "$docbook2man" = "none"; then
    AC_MSG_ERROR(cannot locate docbook2man utility)
fi

dnl Grande Finale

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([hashcash/Makefile varexp/Makefile varexp/tests/Makefile varexp/doc/Makefile])
AC_CONFIG_FILES([doc/Makefile libgnu/Makefile Makefile])
AC_OUTPUT
