SRCDIR := ../..
SUBDIRS :=

LIBNAME := screens.a

SRCS := \
	screen.c \
	screen_main.c \
	screen_audio.c \
	screen_video.c \
	screen_tv.c \
	screen_image.c \
	screen_settings.c \
	screen_viewer.c \
	screen_aplayer.c \
	
include $(SRCDIR)/Makefile.common

CFLAGS += -I..

all:: depend $(LIBNAME)

clean::
	$(RM) $(LIBNAME)
