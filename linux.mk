.PHONY: build init clean client server

C_CC=gcc

CODE_DIR=code
ASSETS_DIR=assets

COM_SRC=code/game/*.c code/common/*.c
COM_LIBS=-lm

CL_FLAGS=-O3 -g
CL_LIBS=-lSDL2 -lSDL2_image -lGL -lGLEW $(COM_LIBS)
CL_OUT=build/linux/client/cl_nuko
CL_SRC=$(wildcard code/client/*.c code/renderer/*.c $(COM_SRC))
CL_OBJ=$(patsubst code/%.c, build/linux/bin/%.o, $(CL_SRC))
CL_ASSETS=$(patsubst assets/%, build/linux/client/assets/%, $(wildcard assets/*/*.*))

SV_FLAGS=-O3 -g
SV_LIBS=$(COM_LIBS)
SV_OUT=build/linux/server/sv_nuko
SV_SRC=$(wildcard code/server/*.c $(COM_SRC))
SV_OBJ=$(patsubst code/%.c, build/linux/bin/%.o, $(SV_SRC))
SV_ASSETS=$(patsubst assets/map/%, build/linux/server/assets/map/%, $(wildcard assets/map/*.*))

init:
	-mkdir build/linux
	-mkdir build/linux/bin
	-mkdir build/linux/bin/client
	-mkdir build/linux/bin/common
	-mkdir build/linux/bin/game
	-mkdir build/linux/bin/renderer
	-mkdir build/linux/bin/server
	-mkdir build/linux/client
	-mkdir build/linux/client/assets
	-mkdir build/linux/client/assets/map
	-mkdir build/linux/client/assets/mdl
	-mkdir build/linux/client/assets/mtl
	-mkdir build/linux/client/assets/tex
	-mkdir build/linux/server
	-mkdir build/linux/server/assets
	-mkdir build/linux/server/assets/map

build: client server

client: $(CL_OUT) $(CL_ASSETS)

server: $(SV_OUT) $(SV_ASSETS)

$(CL_OUT): $(CL_OBJ)
	$(C_CC) $(CL_FLAGS) $(CL_LIBS) $(CL_OBJ) -o $(CL_OUT)

$(SV_OUT): $(SV_OBJ)
	$(C_CC) $(SV_FLAGS) $(SV_LIBS) $(SV_OBJ) -o $(SV_OUT)

build/linux/client/assets/%: assets/%
	cp $< $@

build/linux/server/assets/%: assets/%
	cp $< $@

build/linux/bin/%.o: code/%.c
	$(C_CC) $(CL_FLAGS) -c $< -o $@

clean:
	rm -rf build/linux/bin/client/*
	rm -rf build/linux/bin/server/*
	rm -rf build/linux/bin/game/*
	rm -rf build/linux/bin/common/*
	rm -rf build/linux/bin/renderer/*
	rm -rf build/linux/client/assets/map/*
	rm -rf build/linux/client/assets/mdl/*
	rm -rf build/linux/client/assets/mtl/*
	rm -rf build/linux/client/assets/tex/*
	rm -rf build/linux/client/*
	rm -rf build/linux/server/assets/map
	rm -rf build/linux/server/*
