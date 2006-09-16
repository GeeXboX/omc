include config.mak

all: omc

omc:
	$(MAKE) -C src

clean:
	$(MAKE) -C src clean

install:
	$(MAKE) -C src install
	$(MAKE) -C etc install
	$(MAKE) -C data install

.phony: clean
