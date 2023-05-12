# Makefile

CC = gcc
CPPFLAGS = -MMD
CFLAGS = -Wall -Wextra
LDFLAGS =
LDLIBS = `pkg-config --libs sdl2 SDL2_image gtk+-3.0` -lm

SRC = main.c \
	grid_processing/basic_utilities.c \
	grid_processing/contour_manager.c \
	grid_processing/doubles_lists.c \
	grid_processing/grid_detection.c \
	grid_processing/hough_transform.c \
	grid_processing/img_upgrade.c \
	grid_processing/SDL_utilities.c

OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

main: ${OBJ}

-include ${DEP}

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} main

# END
