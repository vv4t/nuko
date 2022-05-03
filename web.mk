.PHONY: all build server client

EM_CC=emcc

COM_SRC=code/common/*.c code/game/*.c
COM_CFLAGS=-s WASM=1 -O3
COM_LDFLAGS=--pre-js web/lib/net.js

CL_SRC=$(wildcard code/client/*.c $(COM_SRC)) code/sys/sys_sdl.c
CL_OUT=build/web/client/index.html
CL_CFLAGS=$(COM_CLFAGS) --shell-file web/client/index.html
CL_LDFLAGS=$(COM_LDFLAGS) -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_WEBGL2=1 --js-library web/client/library.js --post-js web/client/main.js
CL_ASSETS_SRC=$(wildcard assets/*/*)
CL_ASSETS=$(patsubst %, --preload-file %, $(CL_ASSETS_SRC)) --use-preload-plugins

SV_SRC=$(wildcard code/server/*.c $(COM_SRC)) code/sys/sys_dedicated.c
SV_OUT=build/web/server.cjs
SV_CFLAGS=$(COM_CLFAGS)
SV_LDFLAGS=$(COM_LDFLAGS) --js-library web/server/library.js
SV_ASSETS_SRC=$(wildcard assets/map/*)
SV_ASSETS=$(patsubst %, --preload-file %, $(SV_ASSETS_SRC)) --use-preload-plugins

all: init build

build: server client

server: $(SV_OUT)

client: $(CL_OUT)

$(CL_OUT): $(CL_SRC) $(CL_ASSETS_SRC) web/client/index.html web/client/library.js web/client/main.js web/lib/net.js
	$(EM_CC) $(CL_SRC) $(CL_CFLAGS) $(CL_LDFLAGS) $(CL_ASSETS) -o $(CL_OUT)

$(SV_OUT): $(SV_SRC) $(SV_ASSETS_SRC) build/web/index.js build/web/package.json web/client/library.js web/client/main.js web/lib/net.js
	$(EM_CC) $(SV_SRC) $(SV_CFLAGS) $(SV_LDFLAGS) $(SV_ASSETS) -o $(SV_OUT)

build/web/index.js: web/server/index.js
	cp $< $@

build/web/package.json: web/server/package.json
	cp $< $@

init:
	-mkdir build
	-mkdir build/web
	-mkdir build/web/client

clean:
	-rm build/web/*
