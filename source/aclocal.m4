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
