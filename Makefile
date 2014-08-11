CFLAGS = -std=c99 -O3 -Wall -g
LDFLAGS = -lSDL2 -lSDL2_image
CC = gcc

bored: bored.c queue.c
	$(LINK.c) -o $@ $^

run: bored
	./bored

clean:
	rm -fv bored
