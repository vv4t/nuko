defualt: build run

BASE_DIR=code

C_CC=gcc
C_FLAGS=-O3 -g
C_SRC=$(BASE_DIR)/client/*.c $(BASE_DIR)/cgame/*.c $(BASE_DIR)/game/*.c $(BASE_DIR)/common/*.c $(BASE_DIR)/renderer/*.c
C_OUT=bin/linux/nuko
C_LIBS=-lSDL2 -lSDL2_image -lGL -lm -lGLEW

build:
	$(C_CC) $(C_SRC) $(C_FLAGS) $(C_LIBS) -o $(C_OUT)

run:
	./$(C_OUT)
