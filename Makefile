#
# mapSoN Unix Makefile
#
# $Header$
#

SUBDIRS	= lib src #man

all:	$(SUBDIRS)

src::
	@echo "====> Building mapSoN"
	@(cd src;$(MAKE))

lib::
	@echo "====> Building libraries"
	@(cd lib;${MAKE})

man::
	@echo "====> Building manfiles"
	@(cd man;${MAKE})

clean:
	@echo "====> Cleaning up"
	@for n in $(SUBDIRS);do (cd $$n;$(MAKE) clean);done
	rm -f configure config.log config.cache config.log config.status

build-dist:	all
	@${MAKE} real-build-dist VERS=`sed -n -e '/^#define[	 ][	 ]*VERS/p' src/version.h | sed -e 's/^.*"\(.*\)"/\1/' -e 's/ /-/g' -e 's/-beta-/b/g'`


bump:
#	@$$HOME/.bin/bump.sh

real-build-dist::
	@echo Assembling release archive for ${VERS}

ChangeLog:
	-cp ChangeLog ChangeLog.old
	rcs2log -u 'simons	Peter Simons	simons@rhein.de' >ChangeLog.new;
	if [ -s ChangeLog.new ]; then \
		cat ChangeLog.new ChangeLog.old >ChangeLog; \
		rm ChangeLog.new ChangeLog.old;\
	else \
		rm ChangeLog.new ChangeLog.old; \
	fi
