include ../../config.mak

LIBNAME = screens.a

SRCS = \
	screen.c \
	screen_main.c \
	screen_audio.c \
	screen_video.c \
	screen_tv.c \
	screen_image.c \
	screen_settings.c \
	screen_viewer.c \
	screen_aplayer.c \

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
