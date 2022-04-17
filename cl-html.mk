SRC_DIR=code
ASSET_SRC=$(wildcard assets/*/*)

EM_CC=emcc
EM_ASSETS=$(patsubst %, --preload-file %, $(ASSET_SRC)) --use-preload-plugins
EM_CFLAGS=-s WASM=1 -O3 -s EXPORTED_RUNTIME_METHODS=cwrap -s --js-library html/library.js
EM_LDFLAGS=-s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_WEBGL2=1
EM_SRC=$(SRC_DIR)/client/*.c $(SRC_DIR)/cgame/*.c $(SRC_DIR)/game/*.c $(SRC_DIR)/common/*.c $(SRC_DIR)/renderer/*.c
EM_OUT=bin/html/index.html
EM_SHELL_FILE=--shell-file html/index.html

build: $(ASSET_SRC)
	$(EM_CC) $(EM_SRC) $(EM_CFLAGS) $(EM_LDFLAGS) $(EM_ASSETS) -o $(EM_OUT) $(EM_SHELL) $(EM_SHELL_FILE)
