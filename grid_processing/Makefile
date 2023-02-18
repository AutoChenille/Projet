
CC = gcc
CPPFLAGS = -MMD
CFLAGS = -Wall -Wextra -O3 `pkg-config --cflags sdl2 SDL2_image`
LDFLAGS =
LDLIBS = `pkg-config --libs sdl2 SDL2_image` -lm

SRC = main.c hough_transform.c doubles_lists.c basic_utilities.c SDL_utilities.c grid_detection.c contour_manager.c img_upgrade.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

all: main

main: ${OBJ}

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} main
