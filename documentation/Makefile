#
# Build the mapSoN documentation
#

.SUFFIXES:
.SUFFIXES: .sgml .tex .txt .dvi .ps .pdf .html

.sgml.tex:
	sgmltools -b jadetex $<

.sgml.html:
	sgmltools -b onehtml $<

.sgml.txt:
	sgmltools -b w3m $<

.tex.dvi:
	jadetex \\nonstopmode\\input $<; \
	while grep -i 'Rerun' $*.log; do \
	     jadetex \\nonstopmode\\input $<; \
	done

.dvi.dvi:
	dvips $< -o $@

.dvi.pdf:
	dvipdf $< $@

.PHONY: all clean distclean realclean

all:	mapson.html mapson.pdf man-page.txt readme.txt

man-page.html:	synopsis.inc description.inc cmdloptions.inc
man-page.html:	configfile.inc license.inc

mapson.html mapson.tex:	license.inc building.inc synopsis.inc cmdloptions.inc
mapson.html mapson.tex: defconfig.inc defreqmail.inc configfile.inc description.inc
mapson.html: principle.png
mapson.html: principle.eps

readme.txt:	readme.sgml building.inc ../VERSION

man-page.txt:	man-page.html
	lynx -width=80 -nolist -dump $< >$@

defconfig.inc:	../config-sample Makefile
	sed <$< >$@ -e 's/&/\&amp;/g' \
		    -e 's/</\&lt;/g' \
		    -e 's/>/\&gt;/g' \
		    -e 's#-#<markup>-</markup>#g' \
		    -e '/^#/d'	     \
		    -e '/^$$/d'

defreqmail.inc:../reqmail.template-sample Makefile
	sed <$< >$@ -e 's/</\&lt;/g' \
		    -e 's/>/\&gt;/g' \
		    -e 's/&/\&amp;/g' \
		    -e 's#-#<markup>-</markup>#g' \
		    -e 's#username@example.com#<replaceable>&</replaceable>#g' \
		    -e 's#Real Name#<replaceable>&</replaceable>#g'

clean distclean:
	@rm -f *.aux *.dvi *.log *.toc
	@rm -f *.css *.fot *.ps *.xml
	@rm -f *.tex defconfig.inc defreqmail.inc

realclean:	clean
	@rm -f *.txt *.pdf *.html

