#
# Build the mapSoN documentation
#
DSSLBASE	= /usr/share/sgml/docbook/dsssl-stylesheets-1.77

.SUFFIXES:
.SUFFIXES: .sgml .tex .txt .dvi .ps .pdf .html

.sgml.tex:
	openjade -t tex -d peti.dsl#print -D $(DSSLBASE)/print $<

.sgml.html:
	openjade -t sgml -d peti.dsl#html -D $(DSSLBASE)/html $< >$@

.html.txt:
	w3m -T text/html -dump $< >$@

.tex.dvi:
	jadetex \\nonstopmode\\input $<;
	jadetex \\nonstopmode\\input $<;
	jadetex \\nonstopmode\\input $<;

.dvi.ps:
	dvips $< -o $@

.dvi.pdf:
	dvipdf $< $@

.PHONY: all clean distclean realclean publish

all:	mapson.html mapson.pdf mapson.1 readme.txt

mapson.1:	synopsis.inc description.inc cmdloptions.inc configfile.inc \
		license.inc man-page.sgml
	docbook2man man-page.sgml

mapson.html mapson.tex:	license.inc building.inc synopsis.inc cmdloptions.inc
mapson.html mapson.tex: defconfig.inc defreqmail.inc configfile.inc description.inc
mapson.html mapson.tex: effectiveness.inc
mapson.html mapson.tex: ../libvarexp/documentation/arrays-of-variables.inc
mapson.html mapson.tex: ../libvarexp/documentation/looping.inc
mapson.html mapson.tex: ../libvarexp/documentation/operations-on-variables.inc
mapson.html mapson.tex: ../libvarexp/documentation/quoted-pairs.inc
mapson.html mapson.tex: ../libvarexp/documentation/variable-expressions.inc
mapson.html           :	principle1.png principle2.png
mapson.tex	      :	principle1.eps principle2.eps

readme.html:	readme.sgml building.inc ../VERSION

index.html:	description.inc effectiveness.inc ../VERSION

defconfig.inc:	../config-sample
	sed <$< >$@ -e 's/&/\&amp;/g' \
		    -e 's/</\&lt;/g' \
		    -e 's/>/\&gt;/g' \
		    -e 's#-#<markup>-</markup>#g' \
		    -e '/^#/d'	     \
		    -e '/^$$/d'

defreqmail.inc:../reqmail.template-sample
	sed <$< >$@ -e 's/</\&lt;/g' \
		    -e 's/>/\&gt;/g' \
		    -e 's/&/\&amp;/g' \
		    -e 's#-#<markup>-</markup>#g' \
		    -e 's#username@example.com#<replaceable>&</replaceable>#g' \
		    -e 's#Real Name#<replaceable>&</replaceable>#g'

publish::	mapson.html mapson.pdf index.html
	mkdir htdocs
	cp index.html peti.css principle?.png htdocs
	cp mapson.html htdocs/user-manual.html
	cp mapson.pdf htdocs/user-manual.pdf
	rsync -vaz --delete htdocs/ mapson.sourceforge.net:/home/groups/m/ma/mapson/htdocs/
	rm -rf htdocs

clean distclean:
	@rm -f *.aux *.dvi *.log *.toc
	@rm -f *.fot *.ps *.xml
	@rm -f *.tex defconfig.inc defreqmail.inc
	@rm -f manpage.links manpage.refs


realclean:	clean
	@rm -f *.txt *.pdf *.html
	@rm -f mapson.1
