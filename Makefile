CFLAGS = -std=c99 -O3 -Wall -g
LDFLAGS = -lSDL2 -lSDL2_image
CC = gcc

SRCS = \
	src/bored.c \
	src/priq.c

bored: $(SRCS)
	$(LINK.c) -o $@ $^

run: bored
	./bored

clean:
	rm -fv bored
