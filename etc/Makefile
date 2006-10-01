SRCDIR := ..
SUBDIRS :=

include $(SRCDIR)/Makefile.common

LANGS := en fr

LANG_FILES := \
	langinfo.xml \
	strings.xml \
	

install::
	$(INSTALL) -d "$(CFG_DIR)/langs"
	$(INSTALL) -c -m 644 omc.conf "$(CFG_DIR)"
	for l in $(LANGS); do \
	  $(INSTALL) -d "$(CFG_DIR)/langs/$$l"; \
	  for f in $(LANG_FILES); do \
	    $(INSTALL) -c -m 644 langs/$$l/$$f "$(CFG_DIR)/langs/$$l"; \
	  done \
	done

uninstall::
	$(RM) "$(CFG_DIR)/omc.conf"
	for l in $(LANGS); do \
	  for f in $(LANG_FILES); do \
	    $(RM) "$(CFG_DIR)/langs/$$l/$$f"; \
	  done \
	done
