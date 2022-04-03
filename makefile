default: build

build: linux html

linux:
	make -C target/linux build

html:
	make -C target/html build
