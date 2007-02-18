#
# Build librfc822
#

noinst_LIBRARIES    = librfc822.a

librfc822_a_SOURCES = lexer.cpp parser.cpp
noinst_HEADERS      = rfc822.hh

noinst_PROGRAMS     = test
test_SOURCES        = test.cpp
test_LDADD          = librfc822.a

TESTS               = test
