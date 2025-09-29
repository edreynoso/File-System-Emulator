CFLAGS = -g -Wall -pedantic -std=c99 

all: build/fs_emulator

build/fs_emulator:
	gcc $(CFLAGS) ./src/fs_emulator.c ./src/helper.c ./src/commands.c -o fs_emulator

remove:
	rm -f fs_emulator