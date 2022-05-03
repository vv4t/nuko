.PHONY: all build clean

ASSETS=$(patsubst assets/%, build/linux/assets/%, $(wildcard assets/*/*.*))

all: init build

build: $(ASSETS)
	$(MAKE) -C code

init:
	-mkdir build
	-mkdir build/linux
	-mkdir build/linux/bin
	-mkdir build/linux/bin/server
	-mkdir build/linux/bin/client
	-mkdir build/linux/bin/game
	-mkdir build/linux/bin/common
	-mkdir build/linux/assets
	-mkdir build/linux/assets/map
	-mkdir build/linux/assets/mdl
	-mkdir build/linux/assets/mtl
	-mkdir build/linux/assets/tex

build/linux/assets/%: assets/%
	cp $< $@

clean:
	rm -rf build/linux/*
