#
# Build the mapson user manual
#
# $Header$
#

include ../include/mapson.mk

all:	mapson.info mapson.ps

mapson.info:	mapson.texi
	$(MAKEINFO) mapson.texi

mapson.ps:	mapson.texi
	$(TEXI2DVI) mapson.texi
	$(DVIPS) mapson.dvi -o mapson.ps

clean:
	rm -f mapson.aux mapson.cp mapson.fn mapson.ky mapson.log
	rm -f mapson.pg mapson.toc mapson.tp mapson.vr mapson.dvi

realclean:	clean
	rm -f mapson.info mapson.ps
