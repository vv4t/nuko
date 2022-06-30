.PHONY: default clean

EM_CC=emcc

RESOURCES=$(patsubst %, build/web/client/%, style.css nuko.css main.js index.html credits.html help.html leaderboard.html)

COM_SRC=code/common/*.c code/game/*.c
COM_CFLAGS=-s WASM=1 -O3
COM_LDFLAGS=--pre-js web/lib/net.js

CL_SRC=$(wildcard code/client/*.c $(COM_SRC)) code/sys/sys_sdl.c
CL_OUT=build/web/client/index.html
CL_CFLAGS=$(COM_CLFAGS) --shell-file web/client/index.html
CL_LDFLAGS=$(COM_LDFLAGS) -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_WEBGL2=1 --js-library web/client/library.js
CL_ASSETS_SRC=$(wildcard assets/*/*)
CL_ASSETS=$(patsubst %, --preload-file %, $(CL_ASSETS_SRC)) --use-preload-plugins

SV_SRC=$(wildcard code/server/*.c $(COM_SRC)) code/sys/sys_dedicated.c
SV_OUT=build/web/server.cjs
SV_CFLAGS=$(COM_CLFAGS)
SV_LDFLAGS=$(COM_LDFLAGS) --js-library web/server/library.js
SV_ASSETS_SRC=$(wildcard assets/map/*)
SV_ASSETS=$(patsubst %, --preload-file %, $(SV_ASSETS_SRC)) --use-preload-plugins

default: $(CL_OUT) $(SV_OUT)

$(CL_OUT): $(CL_SRC) $(CL_ASSETS_SRC) $(RESOURCES) web/client/index.html web/client/library.js web/lib/net.js
	$(EM_CC) $(CL_SRC) $(CL_CFLAGS) $(CL_LDFLAGS) $(CL_ASSETS) -o $(CL_OUT)

$(SV_OUT): $(SV_SRC) $(SV_ASSETS_SRC) build/web/index.js build/web/package.json web/lib/net.js build/web
	$(EM_CC) $(SV_SRC) $(SV_CFLAGS) $(SV_LDFLAGS) $(SV_ASSETS) -o $(SV_OUT)

build/web/client/main.js: web/client/main.js | build/web/client
	cp $< $@

build/web/client/credits.html: web/client/credits.html | build/web/client
	cp $< $@

build/web/client/leaderboard.html: web/client/leaderboard.html | build/web/client
	cp $< $@

build/web/client/help.html: web/client/help.html | build/web/client
	cp $< $@

build/web/client/style.css: web/client/style.css | build/web/client
	cp $< $@

build/web/client/nuko.css: web/client/nuko.css | build/web/client
	cp $< $@

build/web/index.js: web/server/index.js build/web
	cp $< $@

build/web/package.json: web/server/package.json build/web
	cp $< $@

build/web/client: | build/web
	mkdir $@

build/web: | build
	mkdir $@

build:
	mkdir $@
