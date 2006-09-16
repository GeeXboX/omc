include ../config.mak

LANGS = en fr

LANG_FILES = \
	langinfo.xml \
	strings.xml

install: $(FONTS) $(PIXMAPS)
	$(INSTALL) -c -m 644 omc.conf $(CFG)/omc
	for l in $(LANGS); do \
	  for f in $(LANG_FILES); do \
	    $(INSTALL) -c -m 644 langs/$$l/$$f $(CFG)/omc/langs/$$l; \
	  done \
	done

.phony: install
