#
# Build mapSoN
#

bin_PROGRAMS    = mapson

mapson_SOURCES  = address-db.cpp config.cpp deliver.cpp         \
                  extract-addresses.cpp gather-addresses.cpp    \
                  log.cpp mapson.cpp request-confirmation.cpp   \
                  spool.cpp accept-confirmation.cpp             \
                  libmd5/md5.c lines2regex.cpp                  \
                  libparse-config-file/parse-config-file.cpp    \
                  libhashcash/libhc.c libhashcash/libsha1.c     \
                  libhashcash/timer.c libhashcash/utct.c        \
                  libhashcash/random.c

mapson_CPPFLAGS = -DMAILBOXDIR=\"${mailboxdir}\"                \
                  -DMTA=\"${mta}\"                              \
                  -DSYSCONFDIR=\"${sysconfdir}\"                \
                  -DDATADIR=\"${datadir}\"

if !HAVE_GNU_GETOPT
mapson_CPPFLAGS += -Igetopt
mapson_SOURCES  += libgetopt/getopt.c libgetopt/getopt1.c
endif

if !HAVE_SETENV
mapson_CPPFLAGS += -Ilibenv -DUSE_MY_SETENV
mapson_SOURCES  += libenv/setenv.c
endif

if !HAVE_UNSETENV
mapson_CPPFLAGS += -Ilibenv -DUSE_MY_UNSETENV
mapson_SOURCES  += libenv/unsetenv.c
endif

mapson-mapson.o \
mapson-config.o:        Makefile

noinst_HEADERS  = accept-confirmation.hpp address-db.hpp		\
                  config.hpp deliver.hpp extract-addresses.hpp		\
                  fd-sentry.hpp file-sentry.hpp lines2regex.hpp		\
                  gather-addresses.hpp log.hpp multi-open.hpp		\
                  request-confirmation.hpp spool.hpp

mapson_LDADD    = librfc822/librfc822.a                         \
                  libvarexp/libvarexp.a

mapson.1:
	docbook2man documentation/man-page.sgml
	@rm -f manpage.links manpage.refs

mapson.html:
	@echo >VERSION ${PACKAGE_VERSION}
	docbook2html -u documentation/mapson.sgml
	@rm -f VERSION

SUBDIRS         = librfc822 libvarexp
man_MANS        = mapson.1
EXTRA_DIST      = libhashcash/LICENSE sample-challenge-template \
                  sample-config ${man_MANS} mapson.html
sysconf_DATA    = sample-config
data_DATA       = sample-challenge-template mapson.html

CLEANFILES      = manpage.links manpage.refs mapson-*.tar.gz VERSION
MAINTAINERCLEANFILES = mapson.1 mapson.html

init-src:
	mtn co --branch=to.cryp.RegExp RegExp
	mtn co --branch=to.cryp.system-error system-error
	mtn co --branch=to.cryp.libgetopt libgetopt
	mtn co --branch=to.cryp.libmd5 libmd5
	mtn co --branch=to.cryp.libparse-config-file libparse-config-file
	mtn co --branch=to.cryp.librfc822 librfc822
	mtn co --branch=to.cryp.libvarexp libvarexp
	mtn co --branch=to.cryp.libenv libenv
	mtn co --branch=to.cryp.hashcash libhashcash
	autoreconf -i
