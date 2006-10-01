SRCDIR := .
SUBDIRS := src etc data

include $(SRCDIR)/Makefile.common

distclean::
	$(RM) config.mak config.h config.log
