CFLAGS = -std=c99 -O3 -Wall -g
LDFLAGS = -lm -lSDL2 -lSDL2_image
CC = gcc

SRCS = $(wildcard src/*.c)

bored: $(SRCS)
	$(LINK.c) -o $@ $^

run: bored
	./bored

clean:
	rm -fv bored
