#
# mapSoN Unix Makefile
#
# $Header$
#

SUBDIRS	= lib src

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
	@for n in $(SUBDIRS);do (cd $$n;$(MAKE) clean);done

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
	-cp ChangeLog ChangeLog.old
	rcs2log -u 'simons	Peter Simons	simons@rhein.de' >ChangeLog.new;
	if [ -s ChangeLog.new ]; then \
		cat ChangeLog.new ChangeLog.old >ChangeLog; \
		rm ChangeLog.new ChangeLog.old;\
	else \
		rm ChangeLog.new ChangeLog.old; \
	fi
