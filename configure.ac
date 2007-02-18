dnl
dnl configure.ac -- Process this file with autoconf to produce a configure script.
dnl

dnl Get our current version

AC_INIT(librfc822, 1.1, simons@cryp.to)
AM_INIT_AUTOMAKE([foreign 1.7])
AC_CONFIG_SRCDIR([rfc822.hh])

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

dnl Write results.
dnl
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
