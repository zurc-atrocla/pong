CC=gcc

all: clean ./src/main.c
	$(CC) ./src/main.c -lSDL2 -lSDL2_ttf -Wall -o ./bin/pong

clean:
	rm -rf ./bin/*
