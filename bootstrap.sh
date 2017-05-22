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

sed -i -e 's/^sc_program_name:/disabled_sc_program_name:/' \
       -e 's/^sc_prohibit_atoi_atof/disabled_sc_prohibit_atoi_atof/' \
       -e 's/^sc_prohibit_magic_number_exit/disabled_sc_prohibit_magic_number_exit/' \
       -e 's/^sc_prohibit_strcmp/disabled_sc_prohibit_strcmp/' \
       -e 's/^sc_require_config_h/disabled_sc_require_config_h/' \
       -e 's/^sc_useless_cpp_parens/disabled_sc_useless_cpp_parens/' \
       -e 's/^sc_bindtextdomain/disabled_sc_bindtextdomain/' \
       -e 's/^sc_error_message_period/disabled_sc_error_message_period/' \
       -e 's/^sc_error_message_uppercase/disabled_sc_error_message_uppercase/' \
       -e 's/^sc_prohibit_always-defined_macros/disabled_sc_prohibit_always-defined_macros/' \
       -e 's/^sc_prohibit_path_max_allocation/disabled_sc_prohibit_path_max_allocation/' \
       -e 's/^sc_prohibit_strncpy/disabled_sc_prohibit_strncpy/' \
       -e 's/^sc_prohibit_test_minus_ao/disabled_sc_prohibit_test_minus_ao/' \
       -e 's/^sc_unmarked_diagnostics/disabled_sc_unmarked_diagnostics/' \
  maint.mk

build-aux/gitlog-to-changelog >ChangeLog

autoreconf --install
