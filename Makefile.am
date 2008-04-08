#
# Build mapSoN
#

bin_PROGRAMS	= mapson

mapson_SOURCES	=						\
  accept-confirmation.cpp address-db.cpp config.cpp deliver.cpp \
  extract-addresses.cpp log.cpp mapson.cpp multi-open.cpp	\
  parse-config-file.cpp request-confirmation.cpp md5.c		\
  rfc822-lexer.cpp rfc822-parser.cpp rfc822.hpp spool.cpp	\
  lines2regex.cpp

mapson_CPPFLAGS =						\
  -DREGEXP_POSIX						\
  -DMAILBOXDIR=\"${mailboxdir}\"				\
  -DMTA=\"${mta}\"						\
  -DSYSCONFDIR=\"${sysconfdir}\"				\
  -DDATADIR=\"${datadir}\"

if !HAVE_GNU_GETOPT
mapson_SOURCES	+= getopt.c getopt1.c
endif

if !HAVE_SETENV
mapson_CPPFLAGS += -DUSE_MY_SETENV
mapson_SOURCES	+= setenv.c
endif

if !HAVE_UNSETENV
mapson_CPPFLAGS += -DUSE_MY_UNSETENV
mapson_SOURCES	+= unsetenv.c
endif

mapson-mapson.o \
mapson-config.o:	Makefile

noinst_HEADERS	= mapson.hpp parse-config-file.hpp regexp.hpp	\
		  rfc822.hpp system-error.hpp md5.h

mapson_LDADD	= varexp/libvarexp.a hashcash/libhashcash.a

mapson.1:
	docbook2man docs/man-page.sgml
	@rm -f manpage.links manpage.refs
	@mv mapson.en.1 mapson.1

mapson.html:
	@echo >VERSION ${PACKAGE_VERSION}
	docbook2html -u docs/mapson.sgml
	@rm -f VERSION

SUBDIRS			= varexp hashcash
man_MANS		= mapson.1
EXTRA_DIST		= README COPYING ${man_MANS} sample-config \
			  sample-challenge-template mapson.html
sysconf_DATA		= sample-config
data_DATA		= sample-challenge-template mapson.html

CLEANFILES		= manpage.links manpage.refs mapson-*.tar.gz VERSION
MAINTAINERCLEANFILES	= mapson.1 mapson.html Makefile.in aclocal.m4	\
			  configure depcomp install-sh missing compile	\
			  INSTALL hashcash/Makefile.in

TESTS			= md5test
check_PROGRAMS		= $(TESTS)
md5test_SOURCES		= md5test.c md5.c
