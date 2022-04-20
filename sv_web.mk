SRC_DIR=code

EM_CC=emcc
EM_ASSETS=--preload-file assets/map/nk_construct.map --use-preload-plugins 
EM_CFLAGS=-s WASM=1 -O3 --js-library html/sv-library.js --pre-js html/net.js
EM_SRC=$(SRC_DIR)/server/*.c $(SRC_DIR)/common/*.c $(SRC_DIR)/game/*.c
EM_OUT=server/web/server.cjs

build:# $(ASSET_SRC)
	$(EM_CC) $(EM_SRC) $(EM_CFLAGS) $(EM_ASSETS) -o $(EM_OUT)
