#
# Build the mapson binary
#
# $Header$
#

.include "../include/mapson.mk"

SRCS	= main.c mail_rescue.c fail_safe.c
OBJS	= $(SRCS:.c=.o)
LIBS   += -llists -lmpools -lrfc822 -lexceptions

CFLAGS += -Wall

all:	$(LIBS) mapson

clean:
	@echo "====> Cleaning up"
	rm -f mapson *.o *.core

mapson:		$(OBJS)
	$(CC) $(OBJS) -o mapson $(LIBS)

$(LIBS):
	@(cd ../lib;$(MAKE))

depend:
	makedepend -Y /usr/include $(SRCS)
	@rm -f Makefile.bak


#
# Dependencies
#
# DO NOT DELETE

main.o: proto.h version.h
mail_rescue.o: proto.h
