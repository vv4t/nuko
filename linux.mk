.PHONY: default clean

C_CC=gcc
C_FLAGS=-O3 -g

HEADERS=$(wildcard */*/*.h)
ASSETS=$(patsubst assets/%, build/linux/assets/%, $(wildcard assets/*/*.*))

CL_SRC=$(wildcard code/common/*.c code/client/*.c code/game/*.c)
CL_OBJ=$(patsubst code/%.c, build/linux/bin/%.o, $(CL_SRC))
CL_LIBS=-lSDL2 -lSDL2_image -lGL -lGLEW -lm 

SV_SRC=$(wildcard code/common/*.c code/server/*.c code/game/*.c)
SV_OBJ=$(patsubst code/%.c, build/linux/bin/%.o, $(SV_SRC))
SV_LIBS=-lm 

default: build/linux $(ASSETS) build/linux/nuko build/linux/nuko_dedicated

build/linux/nuko: $(CL_OBJ) code/sys/sys_sdl.c
	$(C_CC) $(C_FLAGS) $(CL_LIBS) $^ -o $@

build/linux/nuko_dedicated: $(SV_OBJ) code/sys/sys_dedicated.c
	$(C_CC) $(C_FLAGS) $(SV_LIBS) $^ -o $@

build/linux/bin/%.o: code/%.c $(HEADERS) build/linux/bin
	$(C_CC) $(C_FLAGS) -c -o $@ $<

build/linux/assets/%: assets/% | build/linux/assets
	cp $< $@

build/linux/bin: | build/linux
	-mkdir $@
	-mkdir $@/server
	-mkdir $@/client
	-mkdir $@/game
	-mkdir $@/common

build/linux/assets: | build/linux
	-mkdir $@
	-mkdir $@/map
	-mkdir $@/mdl
	-mkdir $@/mtl
	-mkdir $@/shader
	-mkdir $@/skybox

build/linux: | build
	-mkdir $@

build:
	-mkdir $@

clean:
	rm -rf build/linux/*
