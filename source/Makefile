#
# Build the mapson binary
#
# $Header$
#

.include "../include/mapson.mk"

SRCS	= main.c
OBJS	= $(SRCS:.c=.o)
LIBS   += -llists -lmpools -lrfc822 -ltext

all:	$(LIBS) mapson

clean:
	@echo "====> Cleaning up"
	rm -f mapson *.o *.core

mapson:		$(OBJS)
	$(CC) $(OBJS) -o mapson $(LIBS)

$(LIBS):
	@(cd ../lib;$(MAKE))


#
# Dependencies
#
