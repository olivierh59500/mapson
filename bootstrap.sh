#! /bin/sh

set -eu

if [ ! -d varexp ]; then
  echo ""
  echo " Remember to execute \"git submodule update --init\" before running this file."
  echo ""
  exit 1
fi

if [ -x "gnulib/gnulib-tool" ]; then
  gnulibtool=gnulib/gnulib-tool
else
  gnulibtool=gnulib-tool
fi

gnulib_modules=( git-version-gen gitlog-to-changelog gnupload
                 maintainer-makefile announce-gen crypto/md5
                 getopt-gnu setenv unsetenv )

$gnulibtool --m4-base build-aux --source-base libgnu --import "${gnulib_modules[@]}"

build-aux/gitlog-to-changelog >ChangeLog

autoreconf --install -Wall
