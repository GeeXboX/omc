SRCDIR := ../..
SUBDIRS :=

LIBNAME := exml.a

SRCS := \
	parser.c \
	lexer.c \
	
include $(SRCDIR)/Makefile.common

CFLAGS += -I..

all:: depend $(LIBNAME)

clean::
	$(RM) $(LIBNAME)
