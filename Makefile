CFLAGS = -g -Wall -pedantic -std=c99 

all: build/fs_emulator

build/fs_emulator:
	gcc $(CFLAGS) fs_emulator.c helper.c commands.c -o fs_emulator