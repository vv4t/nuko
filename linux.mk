.PHONY: all build init clean client server

C_CC=gcc

CODE_DIR=code
ASSETS_DIR=assets

COM_SRC=code/game/*.c code/common/*.c
COM_LIBS=-lm
COM_ASSETS=$(patsubst assets/%, build/linux/assets/%, $(wildcard assets/*/*.*))

CL_FLAGS=-O3 -g
CL_LIBS=-lSDL2 -lSDL2_image -lGL -lGLEW $(COM_LIBS)
CL_OUT=build/linux/cl_nuko
CL_SRC=code/client/*.c $(COM_SRC)

SV_FLAGS=-O3 -g
SV_LIBS=$(COM_LIBS)
SV_OUT=build/linux/sv_nuko
SV_SRC=code/server/*.c $(COM_SRC)

build: client server $(COM_ASSETS)

client: $(CL_OUT)

server: $(SV_OUT)

$(CL_OUT): $(CL_SRC)
	$(C_CC) $(CL_FLAGS) $(CL_LIBS) $(CL_SRC) -o $@

$(SV_OUT): $(SV_SRC)
	$(C_CC) $(SV_FLAGS) $(SV_LIBS) $(SV_SRC) -o $@

build/linux/assets/%: assets/%
	cp $< $@

init:
	-mkdir build
	-mkdir build/linux
	-mkdir build/linux/assets
	-mkdir build/linux/assets/map
	-mkdir build/linux/assets/mdl
	-mkdir build/linux/assets/mtl
	-mkdir build/linux/assets/tex

clean:
	rm -rf build/linux/assets/map/*
	rm -rf build/linux/assets/mdl/*
	rm -rf build/linux/assets/mtl/*
	rm -rf build/linux/assets/tex/*
	rm -rf build/linux/*
