# Makefile

CC = gcc
CPPFLAGS =
CFLAGS = -Wall -Wextra -O3 `pkg-config --cflags sdl2 SDL2_image`
LDFLAGS =
LDLIBS = `pkg-config --libs sdl2 SDL2_image` -lm

all: main

SRC = main.c display.c filter.c rotation.c
OBJ = ${SRC:.c=.o}
EXE = ${SRC:.c=}

main: main.o display.o filter.o rotation.o

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${EXE}
	${RM} *~

# END
