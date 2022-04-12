defualt: build

BASE_DIR=../..
SRC_DIR=$(BASE_DIR)/code
ASSET_DIR=--preload-file ./assets

EM_CC=emcc
EM_CFLAGS=-s WASM=1 -O3 -s EXPORTED_RUNTIME_METHODS=cwrap 
EM_LDFLAGS=-s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_WEBGL2=1
EM_ASSETS=$(ASSET_DIR)/mtl/brick.png $(ASSET_DIR)/mtl/building.png $(ASSET_DIR)/mtl/concrete.png $(ASSET_DIR)/mtl/grass.png $(ASSET_DIR)/map/untitled.map --use-preload-plugins
EM_SRC=$(SRC_DIR)/client/*.c $(SRC_DIR)/cgame/*.c $(SRC_DIR)/game/*.c $(SRC_DIR)/common/*.c $(SRC_DIR)/renderer/*.c
EM_OUT=bin/index.html

build:
	$(EM_CC) $(EM_SRC) $(EM_CFLAGS) $(EM_LDFLAGS) $(EM_ASSETS) -o $(EM_OUT) --shell-file nuko.html
