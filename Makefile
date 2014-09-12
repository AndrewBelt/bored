CFLAGS = -std=c99 -O2 -Wall -g
LDFLAGS = -lm -lSDL2 -lSDL2_image -lSDL2_ttf

SRCS = $(wildcard src/*.c)

bored: $(SRCS)
	$(LINK.c) -o $@ $^

run: bored
	./bored

clean:
	rm -fv bored
