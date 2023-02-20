
CC = gcc
CPPFLAGS = -MMD
CFLAGS = -Wall -Wextra -O3 `pkg-config --cflags sdl2 SDL2_image`
LDFLAGS =
LDLIBS = `pkg-config --libs sdl2 SDL2_image` -lm

SRC = solver.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

solver: ${OBJ}

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} solver
