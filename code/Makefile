.PHONY=all linux dedicated

C_CC=gcc
C_FLAGS=-O3 -g

OUT=../build/linux
OBJ=../build/linux/bin

all: linux dedicated

sys_sdl.c: client/client.h client/input.h client/gl.h common/cmd.h common/log.h common/string.h
sys_dedicated.c: server/server.h

linux: $(OUT)/nuko_linux
dedicated: $(OUT)/nuko_dedicated

$(OUT)/nuko_linux: sys/sys_sdl.c
	$(MAKE) -C client
	$(MAKE) -C common
	$(MAKE) -C game
	$(C_CC) $(C_FLAGS) -lSDL2 -lSDL2_image -lGL -lGLEW -lm $(OBJ)/common/*.o $(OBJ)/game/*.o $(OBJ)/client/*.o $< -o $(OUT)/nuko_linux

$(OUT)/nuko_dedicated: sys/sys_dedicated.c
	$(MAKE) -C server
	$(MAKE) -C common
	$(MAKE) -C game
	$(C_CC) $(C_FLAGS) -lm $(OBJ)/common/*.o $(OBJ)/game/*.o $(OBJ)/server/*.o $< -o $(OUT)/nuko_dedicated
