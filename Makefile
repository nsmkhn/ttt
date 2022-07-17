CC=cc
PKGS=sdl2 sdl2_gfx
LDFLAGS=$(shell pkg-config --libs $(PKGS))
CFLAGS=-Wall -Wextra -std=c11 $(shell pkg-config --cflags $(PKGS))
CFLAGS_DEBUG=$(CFLAGS) -O0 -ggdb
CFLAGS_RELEASE=$(CFLAGS) -O3

all: ttt ttt.debug

ttt.debug: main.c
	$(CC) $(CFLAGS_DEBUG) $< -o $@ $(LDFLAGS)

ttt: main.c
	$(CC) $(CFLAGS_RELEASE) $< -o $@ $(LDFLAGS)

clean:
	rm -rf ttt*
