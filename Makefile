CFLAGS = -Wall -Wextra -ggdb
LIBS = -lX11 -lGL -lXrandr

ifeq ($(OS), Windows_NT)
	LIBS = -lopengl32 -lgdi32
else
	UNAME := $(shell uname)
	ifeq ($(UNAME), Darwin)
		LIBS = -framework OpenGL -framework Cocoa -framework IOKit
	endif
endif

all:
	gcc -o mario-kart-gl main.c include/glad/glad.c $(CFLAGS) $(LIBS)

clean:
	rm -rf mario-kart-gl *.dSYM *.exe
