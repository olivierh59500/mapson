dnl
dnl $Header$
dnl
dnl Local additions to Autoconf macros.
dnl


dnl Get the absolute path of the top source directory. Unfortunately
dnl the @top_src@ variable always gives the relative path, what is not
dnl what I need.
dnl

AC_DEFUN(PETI_PATH_SRCDIR, [
AC_MSG_CHECKING(path of top source directory)
if test "$srcdir" = "."; then
    SRCDIR=`pwd`
else
    SRCDIR=$srcdir
fi
AC_MSG_RESULT($SRCDIR)
AC_SUBST(SRCDIR)
])

dnl This macro will define an user-specified option of the name
dnl "--with-dmalloc". If this is set to "yes" in some way, then the
dnl macro will add the appropriate defines to LIBS, CPPFLAGS and
dnl LDFLAGS so that the dmalloc library is compiled to the binary.
dnl
AC_DEFUN(PETI_WITH_DMALLOC, [
AC_MSG_CHECKING(whether to dmalloc library)
AC_ARG_WITH(dmalloc,
[  --with-dmalloc[=ARG]     Compile with dmalloc library],
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
AC_DEFUN(AC_PETI_PATH_SENDMAIL, [
peti_path_backup=$PATH
PATH=/bin:/sbin:/usr/bin:/usr/sbin:/usr/lib:/usr/libexec:/usr/local/bin:/usr/local/sbin:/usr/local/lib:/usr/local/libexec:/usr/etc
AC_PATH_PROG(SENDMAIL, sendmail, sendmail)
PATH=$peti_path_backup
])

dnl Find the directory that contains the user's mail folders.
dnl
AC_DEFUN(AC_PETI_PATH_MAILDIR, [
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
