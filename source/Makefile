#
# Build the mapson binary
#
# $Header$
#

.include "../include/mapson.mk"

SRCS	= main.c mail_rescue.c fail_safe.c array.c parse_mail.c confirm.c \
	  home_dir.c address_db.c
OBJS	= $(SRCS:.c=.o)
LIBS   += -lconfigfile -lrfc822 -lmpools -llists -lexceptions

all:	$(LIBS) mapson

clean:
	@echo "====> Cleaning up"
	rm -f mapson *.o *.core

mapson:		$(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o mapson $(LIBS)

$(LIBS):
	@(cd ../lib;$(MAKE))

depend:
	makedepend -Y /usr/include $(SRCS)
	@rm -f Makefile.bak


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
