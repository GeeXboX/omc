SRCDIR := ..
SUBDIRS := exml screens widgets

BIN_NAME := omc

SRCS := \
	omc.c \
	cfgparser.c \
	langinfo.c \
	lang_i18n.c \
	filter.c \
	infos.c \
	gfx.c \
	avplayer.c \
	av_demuxer.c \
	amazon.c \
	
DEP_LIBS := \
	exml/exml.a \
	screens/screens.a \
	widgets/widgets.a \
	
include $(SRCDIR)/Makefile.common

EXTRALIBS += $(DEP_LIBS)

all:: depend $(BIN_NAME)

$(DEP_LIBS)::
	$(MAKE) -C $(shell echo $@ | cut -f1 -d/) $(shell echo $@ | cut -f2 -d/)

$(BIN_NAME): $(DEP_LIBS) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $(BIN_NAME) $(EXTRALIBS)

clean::
	$(RM) $(BIN_NAME)

install:: $(BIN_NAME)
	$(INSTALL) -d "$(bindir)"
	$(INSTALL) -c $(INSTALLSTRIP) -m 755 $(BIN_NAME) "$(bindir)"

uninstall::
	$(RM) "$(bindir)/$(BIN_NAME)"
