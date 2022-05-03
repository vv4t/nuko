.PHONY=build

C_CC=gcc
C_FLAGS=-O3 -g

OUT=../../build/linux/bin/game
SRC=$(wildcard *.c)
OBJ=$(patsubst %.c, $(OUT)/%.o, $(SRC))

build: $(OBJ)

bgame.h: bsp.h edict.h map_file.h ../common/nk_math.h
bsp.h: ../common/nk_math.h
map_file.h: bsp.h ../common/lump.h ../common/nk_math.h
protocol.h: bgame.h

bg_clip.c: bg_local.h
bg_main.c: bg_local.h
bg_motion.c: bg_local.h
bg_pmove.c: bg_local.h
edict.c: edict.h
map_file.c: map_file.h

$(OUT)/%.o: %.c
	$(C_CC) $(C_FLAGS) -c -o $@ $<
