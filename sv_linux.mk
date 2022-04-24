defualt: build run

BASE_DIR=code

C_CC=gcc
C_FLAGS=-O3 -g
C_SRC=$(BASE_DIR)/server/*.c $(BASE_DIR)/game/*.c $(BASE_DIR)/common/*.c
C_OUT=server/linux/nuko
C_LIBS=-lm

build:
	$(C_CC) $(C_SRC) $(C_FLAGS) $(C_LIBS) -o $(C_OUT)

run:
	./$(C_OUT)
