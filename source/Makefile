#
# Build the mapson binary
#
# $Header$
#

include ../include/mapson.mk

SRCS	= main.c mail_rescue.c fail_safe.c array.c parse_mail.c confirm.c \
	  home_dir.c address_db.c ruleset.c save_to.c spool_dir.c mailbox_path.c \
	  spool.c loadfile.c is_confirmation.c get_user_name.c escape_from.c \
	  md5.c cookie.c
OBJS	= $(SRCS:.c=.o)
LIBS   += -lrfc822 -lmpools -llists -lexceptions

all:	$(LIBS) mapson

mapson:		$(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o mapson $(LIBS)

ruleset.c:	ruleset.l
	$(LEX) -Prlst ruleset.l
	mv lex.rlst.c ruleset.c

$(LIBS):
	@(cd ../lib;$(MAKE))

depend:
	makedepend -Y /usr/include $(SRCS)
	@rm -f Makefile.bak

install:	all
	$(INSTALL) -c -s -m 555 ./mapson $(BINDIR)/mapson

clean:
	@echo "====> Cleaning up"
	rm -f mapson *.o *.core

realclean:	clean
	rm -f ruleset.c


#
# Dependencies
#
# DO NOT DELETE

main.o: mapson.h version.h
mail_rescue.o: mapson.h
fail_safe.o: mapson.h
array.o: mapson.h
parse_mail.o: mapson.h
confirm.o: mapson.h
home_dir.o: mapson.h
address_db.o: mapson.h
ruleset.o: mapson.h
save_to.o: mapson.h
spool_dir.o: mapson.h
mailbox_path.o: mapson.h
spool.o: mapson.h
loadfile.o: mapson.h
is_confirmation.o: mapson.h
get_user_name.o: mapson.h
escape_from.o: mapson.h
md5.o: md5.h
