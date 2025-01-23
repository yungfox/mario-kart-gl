PKGS = sdl2 glew
CFLAGS = -Wall -Wextra -pedantic -ggdb

UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
	CFLAGS += -framework OpenGL
endif

all:
	gcc $(CFLAGS) $(shell pkg-config --cflags $(PKGS)) $(shell pkg-config --libs $(PKGS)) -o mario-kart-gl main.c

clean:
	rm -rf mario-kart-gl *.dSYM
