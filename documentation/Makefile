#
# Build the mapSoN documentation
#

.SUFFIXES:

%.tex : %.sgml
	sgmltools -b jadetex $<

%.html : %.sgml
	sgmltools -b onehtml $<

%.txt : %.sgml
	sgmltools -b lynx $<

%_version.tex : %.tex
	sed -n -e '/^% $$Id/p' $< | \
	     awk '{ print $$5 }' | \
	     sed -e 's#/#-#g' >$@

%.dvi %.aux %.dvi %.log : %.tex
	jadetex \\nonstopmode\\input $<; \
	while grep -i 'Rerun' $*.log; do \
	     jadetex \\nonstopmode\\input $<; \
	done

%.ps : %.dvi
	dvips $< -o $@

.PHONY: all clean distclean realclean

all:	mapson.html mapson.txt

clean distclean realclean:
	rm -f mapson.aux mapson.dvi mapson.log mapson.toc
	rm -f mapson_version.tex mapson.ps
	rm -f mapson.css mapson.fot mapson.html
	rm -f mapson.tex mapson.xml mapson.txt
