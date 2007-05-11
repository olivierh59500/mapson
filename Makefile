#
# Build libmd5.a.
#
# This Makefile assumes you are using the GNU compiler. Please change
# these defines if you are not.
#
CC		= gcc
AR		= ar
RANLIB		= ranlib

OPTIMFLAGS	= -O3
WARNFLAGS	= -Wall -pedantic
CPPFLAGS	=
CXXFLAGS	=
LDFLAGS		=

OBJS		= md5.o

.SUFFIXES:
.SUFFIXES:	.c .o

.c.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) $(WARNFLAGS) $(OPTIMFLAGS) -c $< -o $@

.PHONY:		all check clean distclean realclean

all:		libmd5.a

libmd5.a:	$(OBJS)
	$(AR) cr $@ $(OBJS)
	$(RANLIB) $@

md5test:	md5test.o libmd5.a
	$(CC) $(LDFLAGS) -o $@ md5test.o -L. -lmd5

check:		md5test
	./md5test

realclean distclean clean::
	@rm -f *.o *.a m5test
	@echo All dependent files have been removed.

# Dependencies

md5.o: md5.h
md5test.o: md5.h
