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
SRCDIR=`pwd`
AC_MSG_RESULT($SRCDIR)
AC_SUBST(SRCDIR)
])


dnl Test whether out gcc version understands -pipe. This is a really
dnl smart macro, except for the fact it doesn't work. The bloody gcc
dnl doesn't abort with an error, if you call it with a wrong option,
dnl so the test can't determine what happened. *sigh*
dnl

dnl AC_DEFUN(PETI_PROG_GCC_PIPE, [
dnl if test "$ac_cv_prog_gcc" = "yes"; then
dnl     AC_MSG_CHECKING(whether gcc accepts -pipe)
dnl     OLDCC="$CC"
dnl     CC="$CC -wwwwwwwipe"
dnl     AC_TRY_COMPILE(,int main(int argc, char ** argv) { return 0; },
dnl 	AC_MSG_RESULT(yes),
dnl 	AC_MSG_RESULT(no)
dnl 	CC="$OLDCC")
dnl fi
dnl ])
