include ../config.mak

BIN_NAME = omc

SRCS =	omc.c \
	cfgparser.c \
	langinfo.c \
	lang_i18n.c \
	exmllexer.c \
	exmlparser.c \
	widget.c \
	filter.c \
	infos.c \
	gfx.c \
	avplayer.c \
	av_demuxer.c \
	amazon.c \
	screen.c \
	screen_main.c \
	screen_audio.c \
	screen_video.c \
	screen_tv.c \
	screen_image.c \
	screen_settings.c \
	screen_viewer.c \
	screen_aplayer.c

LDFLAGS += -levas -lecore_evas -lecore
LDFLAGS += -lplayer
LDFLAGS += -lcurl
LDFLAGS += -lpthread
LDFLAGS += -lxine

OBJS = $(SRCS:.c=.o)

.SUFFIXES: .c .o

all: config.h depend $(BIN_NAME)

.c.o:
	$(CC) -c $(CFLAGS) -o $@ $<

config.h:
	echo "#define CFG_DIR \"$(CFG)/omc\"" >> config.h
	echo "#define DATA_DIR \"$(PREFIX)/share/omc\"" >> config.h

$(BIN_NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $(BIN_NAME)

clean:
	rm -f *.o $(BIN_NAME) config.h
	rm -f .depend

install: $(BIN_NAME)
	$(INSTALL) -c $(BIN_NAME) $(PREFIX)/bin

depend:
	$(CC) -MM $(CFLAGS) $(SRCS) 1>.depend

.phony: clean depend

#
# include dependency files if they exist
#
ifneq ($(wildcard .depend),)
include .depend
endif
