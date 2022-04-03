defualt: build run

C_CC=g++
C_FLAGS=-O3
C_SRC=../../client/*.cpp ../../game/client/*.cpp ../../renderer/*.cpp
C_OUT=bin/nuko-app
C_LIBS=-lSDL2 -lGL -lm -lGLEW

build:
	$(C_CC) $(C_SRC) $(C_FLAGS) $(C_LIBS) -o $(C_OUT)

run:
	./$(C_OUT)
