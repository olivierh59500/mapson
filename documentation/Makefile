#
# Build the mapSoN documentation
#

.SUFFIXES:

%.tex : %.sgml
	sgmltools -b jadetex $<

%.html : %.sgml
	sgmltools -b onehtml $<

%.dvi : %.tex
	jadetex \\nonstopmode\\input $<; \
	while grep -i 'Rerun' $*.log; do \
	     jadetex \\nonstopmode\\input $<; \
	done

%.ps : %.dvi
	dvips $< -o $@

%.pdf : %.dvi
	dvipdf $< $@

.PHONY: all clean distclean realclean

all:	mapson.html mapson.pdf man-page.txt

man-page.html:	license.sgml

mapson.html mapson.tex:	license.sgml building.sgml synopsis.sgml cmdloptions.sgml
mapson.html mapson.tex: defconfig.sgml defreqmail.sgml

man-page.txt:	man-page.html synopsis.sgml cmdloptions.sgml
	lynx -width=80 -nolist -dump $< >$@

defconfig.sgml:	../config-sample Makefile
	sed <$< >$@ -e 's/</\&lt;/g' \
	            -e 's/>/\&gt;/g' \
	            -e '/^#/d'       \
	            -e '/^$$/d'

defreqmail.sgml:../reqmail.template-sample Makefile
	sed <$< >$@ -e 's/</\&lt;/g' \
	            -e 's/>/\&gt;/g' \
	            -e 's#example.com#<replaceable>&</replaceable>#g' \
	            -e 's#username#<replaceable>&</replaceable>#g'  \
	            -e 's#Real Name#<replaceable>&</replaceable>#g'

clean distclean realclean:
	@rm -f *.aux *.dvi *.log *.toc
	@rm -f *.css *.fot *.html *.ps
	@rm -f *.tex *.xml *.txt *.pdf
	@rm -f defconfig.sgml defreqmail.sgml
