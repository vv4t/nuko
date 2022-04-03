EM_CC=em++
EM_CFLAGS=-s WASM=1 -O3 -s EXPORTED_RUNTIME_METHODS=cwrap 
EM_LDFLAGS=-s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_WEBGL2=1
EM_SRC=../../client/*.cpp ../../client/cgame/*.cpp ../../client/renderer/*.cpp
EM_OUT=bin/index.html

build:
	$(EM_CC) $(EM_SRC) $(EM_CFLAGS) $(EM_LDFLAGS) -o $(EM_OUT) --shell-file nuko.html
