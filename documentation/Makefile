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

man-page.txt:	man-page.html
	lynx -width=72 -hiddenlinks=ignore -dump $< >$@

clean distclean realclean:
	rm -f *.aux *.dvi *.log *.toc
	rm -f *.css *.fot *.html *.ps
	rm -f *.tex *.xml *.txt *.pdf
