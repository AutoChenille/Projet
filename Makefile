CC = gcc
CPPFLAGS = -MMD
CFLAGS = -Wall -Wextra -O3 `pkg-config --cflags sdl2 SDL2_image gtk+-3.0` -lm
LDFLAGS =
LDLIBS = `pkg-config --libs sdl2 SDL2_image` -lm

SOURCES := $(shell find $(SRCDIR) -type f -name '*.c')
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

all: main

main: ${OBJ}

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} main
