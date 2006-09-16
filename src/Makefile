include ../config.mak

BIN_NAME = omc

SUBDIRS = exml screens

SRCS =	omc.c \
	cfgparser.c \
	langinfo.c \
	lang_i18n.c \
	widget.c \
	filter.c \
	infos.c \
	gfx.c \
	avplayer.c \
	av_demuxer.c \
	amazon.c \

DEP_LIBS = \
	exml/exml.a \
	screens/screens.a \

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

exml/exml.a:
	$(MAKE) -C exml

screens/screens.a:
	$(MAKE) -C screens

$(BIN_NAME): $(DEP_LIBS) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(DEP_LIBS) $(LDFLAGS) -o $(BIN_NAME)

clean:
	rm -f *.o $(BIN_NAME) config.h
	rm -f .depend
	for s in $(SUBDIRS); do \
	  $(MAKE) -C $$s clean; \
	done

install: $(BIN_NAME)
	$(INSTALL) -c $(BIN_NAME) $(PREFIX)/bin

depend:
	$(CC) -MM $(CFLAGS) $(SRCS) 1>.depend
	for s in $(SUBDIRS); do \
	  $(MAKE) -C $$s depend; \
	done

.phony: clean depend

#
# include dependency files if they exist
#
ifneq ($(wildcard .depend),)
include .depend
endif
