#
# Build the mapson binary
#
# $Header$
#

.include "../include/mapson.mk"

SRCS	= main.c mail_rescue.c
OBJS	= $(SRCS:.c=.o)
LIBS   += -llists -lmpools -lrfc822 -ltext

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

proto:
	@echo "====> Creating proto.h"
	@echo > proto.h "/* Prototypes for mapSoN */"
	@echo >>proto.h ""
	@for n in $(SRCS); do \
		echo >>proto.h "/* $$n */"; \
		echo >>proto.h ""; \
		cproto -g $$n >>proto.h; \
		echo >>proto.h ""; \
	done;


#
# Dependencies
#
# DO NOT DELETE

main.o: proto.h version.h
