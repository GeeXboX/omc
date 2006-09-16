include ../../config.mak

LIBNAME = widgets.a

SRCS = \
	widget.c \
	image.c \
	color.c \
	text.c \
	textblock.c \
	notifier.c \
	border.c \
	menu.c \

OBJS = $(SRCS:.c=.o)
CFLAGS += -I..

.SUFFIXES: .c .o

all: depend $(LIBNAME)

.c.o:
	$(CC) -c $(CFLAGS) -o $@ $<

$(LIBNAME): $(OBJS)
	$(AR) r $(LIBNAME) $(OBJS)
	$(RANLIB) $(LIBNAME)

clean:
	rm -f *.o *.a *~
	rm -f .depend

depend:
	$(CC) -MM $(CFLAGS) $(SRCS) 1>.depend

.phony: clean depend

#
# include dependency files if they exist
#
ifneq ($(wildcard .depend),)
include .depend
endif
