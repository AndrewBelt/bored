CFLAGS = -Wall -O3 -std=c99
LDFLAGS = -lSDL2 -lSDL2_image
CC = gcc

bored: bored.c
	$(LINK.c) -o $@ $^

run: bored
	./bored

clean:
	rm -fv bored
