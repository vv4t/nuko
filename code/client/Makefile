.PHONY=build

C_CC=gcc
C_FLAGS=-O3 -g

OUT=../../build/linux/bin/client
SRC=$(wildcard *.c)
OBJ=$(patsubst %.c, $(OUT)/%.o, $(SRC))

build: $(OBJ)

client.h: ../game/edict.h ../game/bgame.h
cl_local.h: client.h input.h renderer.h ../game/protocol.h ../game/map_file.h ../common/net.h ../common/cmd.h ../common/log.h
gl.h: ../common/log.h
input.h: ../game/bgame.h ../common/string.h ../common/log.h ../common/cmd.h
mdl_file.h: ../common/log.h ../common/zone.h ../common/lump.h ../common/nk_math.h
r_local.h: renderer.h gl.h client.h mdl_file.h ../common/log.h ../common/zone.h ../game/map_file.h

cl_cgame.c: cl_local.h
cl_main.c: cl_local.h
cl_net.c: cl_local.h
gl.c: gl.h
input.c: input.h
mdl_file.c: mdl_file.h
r_main.c: r_local.h
r_map.c: r_local.h
r_mesh.c: r_local.h
r_model.c: r_local.h
sys_main.c: sys.h
sys_win.c: sys.h

$(OUT)/%.o: %.c
	$(C_CC) $(C_FLAGS) -c -o $@ $<
