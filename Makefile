#
# mapSoN Unix Makefile
#
# $Header$
#

SUBDIRS	= lib src doc

all:	include/mapson.mk $(SUBDIRS)

src::
	@echo "====> Building mapSoN"
	@(cd src;$(MAKE))

lib::
	@echo "====> Building libraries"
	@(cd lib;${MAKE})

doc::
	@echo "====> Building documentation"
	@(cd doc;${MAKE})

clean:
	@echo "====> Cleaning up"
	@for n in $(SUBDIRS) doc;do (cd $$n;$(MAKE) clean);done

distclean:	clean
	rm -f config.log config.cache config.log config.status include/mapson.mk
	rm -f include/paths.h

realclean:	distclean
	rm -f configure


build-dist:	all
	@scripts/build-dist.sh `sed -n -e '/^#define[	 ][	 ]*VERS/p' src/version.h | sed -e 's/^.*"\(.*\)"/\1/' -e 's/ /-/g' -e 's/-beta-/b/g'`

configure:	configure.in aclocal.m4
	autoconf

include/mapson.mk:	configure
	./configure $(ACFLAGS)

bump::
	@scripts/bump.sh

ChangeLog::
	-cp etc/ChangeLog ChangeLog
	rcs2log -u 'simons	Peter Simons	peter.simons@gmd.de' >ChangeLog.new;
	if [ -s ChangeLog.new ]; then \
		cat ChangeLog ChangeLog.new >etc/ChangeLog; \
	fi
	rm ChangeLog.new ChangeLog;

install:	all
	@(cd src;$(MAKE) install)
	@(cd doc;$(MAKE) install)
