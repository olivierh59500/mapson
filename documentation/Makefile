#
# Build the librfc822 documentation
#
DSSSLBASE	= /usr/share/sgml/docbook/dsssl-stylesheets-1.77

.SUFFIXES:
.SUFFIXES: .sgml .tex .txt .dvi .ps .pdf .html

.sgml.tex:
	openjade -t tex -d peti.dsl#print -D $(DSSSLBASE)/print $<

.sgml.html:
	openjade -t sgml -d peti.dsl#html -D $(DSSSLBASE)/html $< >$@

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

all:	user-manual.html user-manual.pdf readme.txt

user-manual.html:	../VERSION peti.dsl
user-manual.tex:	../VERSION peti.dsl
readme.html:		../VERSION peti.dsl

index.html: ../VERSION peti.dsl

clean distclean:
	@rm -f *.aux *.dvi *.log *.toc
	@rm -f *.fot *.ps *.xml
	@rm -f *.tex

realclean:	clean
	@rm -f *.pdf *.html *.txt
