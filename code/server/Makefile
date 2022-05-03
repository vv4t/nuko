.PHONY=build

C_CC=gcc
C_FLAGS=-O3 -g

OUT=../../build/linux/bin/server
SRC=$(wildcard *.c)
OBJ=$(patsubst %.c, $(OUT)/%.o, $(SRC))

build: $(OBJ)

sv_local.h: ../game/bgame.h ../game/protocol.h ../game/map_file.h ../common/net.h ../common/log.h
sys_main.h: server.h

sv_client.c: sv_local.h
sv_game.c: sv_local.h
sv_main.c: sv_local.h
sv_net.c: sv_local.h
sv_main.c: sv_local.h

$(OUT)/%.o: %.c
	$(C_CC) $(C_FLAGS) -c -o $@ $<
