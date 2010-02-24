#
# Build mapSoN documentation
#

man_MANS		= mapson.1
data_DATA		= mapson.html
EXTRA_DIST		= mapson.1 mapson.html

mapson.1:
	docbook2man doc/man-page.xml

mapson.html:
	xmlto html-nochunks doc/mapson.xml
