dnl
dnl $Header$
dnl
dnl Local additions to Autoconf macros.
dnl


dnl Get the absolute path of the top source directory. Unfortunately
dnl the @top_src@ variable always gives the relative path, what is not
dnl what I need.

AC_DEFUN(PETI_PATH_SRCDIR, [
AC_MSG_CHECKING(path of top source directory)
SRCDIR=`(cd $srcdir;pwd)`
AC_MSG_RESULT($SRCDIR)
AC_SUBST(SRCDIR)
])

dnl This macro will define an user-specified option of the name
dnl "--with-dmalloc". If this is set to "yes" in some way, then the
dnl macro will add the appropriate defines to LIBS, CPPFLAGS and
dnl LDFLAGS so that the dmalloc library is compiled to the binary.
dnl
AC_DEFUN(PETI_WITH_DMALLOC, [
AC_MSG_CHECKING(whether to link with dmalloc library)
AC_ARG_WITH(dmalloc,
[  --with-dmalloc[=ARG]    Compile with dmalloc library],
if test "$withval" = "" -o "$withval" = "yes"; then
    ac_cv_dmalloc="/usr/local"
else
    ac_cv_dmalloc="$withval"
fi
AC_MSG_RESULT(yes)
CPPFLAGS="$CPPFLAGS -DDEBUG_DMALLOC -DDMALLOC_FUNC_CHECK -I$ac_cv_dmalloc/include"
LDFLAGS="$LDFLAGS -L$ac_cv_dmalloc/lib"
LIBS="$LIBS -ldmalloc"
,AC_MSG_RESULT(no))
])


dnl Find the sendmail binary with an extended path and replace
dnl @SENDMAIL@ with the path in all defined output files.
dnl
AC_DEFUN(PETI_PATH_SENDMAIL, [
peti_path_backup=$PATH
PATH=/bin:/sbin:/usr/bin:/usr/sbin:/usr/lib:/usr/libexec:/usr/local/bin:/usr/local/sbin:/usr/local/lib:/usr/local/libexec:/usr/etc
AC_PATH_PROG(SENDMAIL, sendmail, sendmail)
PATH=$peti_path_backup
])


dnl Find the directory that contains the user's mail folders.
dnl
AC_DEFUN(PETI_PATH_MAILDIR, [
AC_MSG_CHECKING(for system mail directory)
if test -d "/var/mail"; then
    MAILDIR="/var/mail"
    AC_MSG_RESULT($MAILDIR)
else
    if test -d "/var/spool/mail"; then
	MAILDIR="/var/spool/mail"
	AC_MSG_RESULT($MAILDIR)
    else
	MAILDIR="/var/mail"
	AC_MSG_RESULT($MAILDIR    ** default **)
	AC_MSG_WARN(Could not reliably determine the mail directory path!)
	AC_MSG_WARN(Better check include/paths.h!)
    fi
fi
AC_SUBST(MAILDIR)
])


dnl Find out where we have a berkeley db include and library.
dnl
AC_DEFUN(PETI_FIND_BERKELEY_DB, [
AC_MSG_CHECKING(where db.h is)
AC_ARG_WITH(db-include,
[  --with-db-include=ARG   Directory where db.h lives],
if test "$withval" = "yes" -o "$withval" = "no"; then
    AC_MSG_RESULT(trying to figure out myself)
    AC_CHECK_HEADERS(db.h)
    if test "$ac_cv_header_db_h" = no; then
	AC_MSG_WARN(no)
	AC_MSG_ERROR(Please set --with-db-include when calling configure.)
    fi
else
    CPPFLAGS="$CPPFLAGS -I$withval"
    AC_MSG_RESULT($withval)
fi,
[
    AC_MSG_RESULT(trying to figure out myself)
    AC_CHECK_HEADERS(db.h)
    if test "$ac_cv_header_db_h" = no; then
	AC_MSG_WARN(no)
	AC_MSG_ERROR(Please set --with-db-include when calling configure.)
    fi
])

AC_MSG_CHECKING(where dbopen() is)
AC_ARG_WITH(db-lib,
[  --with-db-lib=ARG       Path and name of the lib containing dbopen()],
if test "$withval" = "yes" -o "$withval" = "no"; then
    AC_MSG_RESULT(trying to figure out myself)
else
    LIBS="$LIBS $withval"
    AC_MSG_RESULT($withval)
fi,
[
    AC_MSG_RESULT(trying to figure out myself)
    AC_CHECK_LIB(db, dbopen, [
	LIBS="$LIBS -ldb"
    ], [
	AC_CHECK_FUNC(dbopen, [
	    :
	], [
	    AC_MSG_ERROR(Please set --with-db-lib when calling configure.)
	])
    ])
    if test "$ac_cv_lib_db" = no; then
	AC_MSG_WARN(mapSoN can't find the db.h include.)
	AC_MSG_ERROR(Please set --with--db-include when calling configure.)
    fi
])

])
