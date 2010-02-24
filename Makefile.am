#
# Build mapSoN
#

ACLOCAL_AMFLAGS = -I build-aux

bin_PROGRAMS    = mapson

mapson_SOURCES  =                                               \
  accept-confirmation.cpp address-db.cpp config.cpp deliver.cpp \
  extract-addresses.cpp log.cpp mapson.cpp multi-open.cpp       \
  parse-config-file.cpp request-confirmation.cpp                \
  rfc822-lexer.cpp rfc822-parser.cpp rfc822.hpp spool.cpp       \
  lines2regex.cpp

mapson_CPPFLAGS =                                               \
  -I${srcdir}/libgnu                                            \
  -DREGEXP_POSIX                                                \
  -DMAILBOXDIR=\"${mailboxdir}\"                                \
  -DMTA=\"${mta}\"                                              \
  -DSYSCONFDIR=\"${sysconfdir}\"                                \
  -DDATADIR=\"${datadir}\"

mapson-mapson.o \
mapson-config.o:        Makefile

noinst_HEADERS  = mapson.hpp parse-config-file.hpp regexp.hpp   \
                  rfc822.hpp system-error.hpp

mapson_LDADD    = varexp/libvarexp.a hashcash/libhashcash.a libgnu/libgnu.a

SUBDIRS         = libgnu varexp hashcash doc
EXTRA_DIST      = README COPYING sample-config sample-challenge-template build-aux/gnulib-cache.m4
sysconf_DATA    = sample-config
data_DATA       = sample-challenge-template

dist-hook:
	echo $(VERSION) > $(distdir)/.tarball-version
	if test -d .git; then						\
	  $(top_srcdir)/build-aux/gitlog-to-changelog			\
	    >$(distdir)/cl-t -- master $(top_srcdir)/m4;		\
	  rm -f $(distdir)/ChangeLog;					\
	  mv $(distdir)/cl-t $(distdir)/ChangeLog;			\
	fi
