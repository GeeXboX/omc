SRCDIR := ../..
SUBDIRS :=

LIBNAME := widgets.a

SRCS := \
	widget.c \
	image.c \
	color.c \
	text.c \
	textblock.c \
	notifier.c \
	border.c \
	menu.c \
	cover.c \
	clipper.c \
	item.c \
	browser.c \
	cwd.c \
	
include $(SRCDIR)/Makefile.common

CFLAGS += -I..

all:: depend $(LIBNAME)

clean::
	$(RM) $(LIBNAME)
