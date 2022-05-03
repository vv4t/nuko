.PHONY=build

C_CC=gcc
C_FLAGS=-O3 -g

OUT=../../build/linux/bin/common
SRC=$(wildcard *.c)
OBJ=$(patsubst %.c, $(OUT)/%.o, $(SRC))

build: $(OBJ)

cmd.h: log.h string.h
log.h: zone.h
string.h: zone.h
lump.h: zone.h

cmd.c: cmd.h
log.c: log.h
string.c: string.h
zone.c: zone.h
lump.c: lump.h

$(OUT)/%.o: %.c
	$(C_CC) $(C_FLAGS) -c -o $@ $<
