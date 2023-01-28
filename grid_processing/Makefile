
CC = gcc
CPPFLAGS = -MMD
CFLAGS = -Wall -Wextra -O3 `pkg-config --cflags sdl2 SDL2_image`
LDFLAGS =
LDLIBS = `pkg-config --libs sdl2 SDL2_image`

SRC = hough_transform.c doubles_lists.c basic_utilities.c SDL_utilities.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

hough_transform: ${OBJ}

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} hough_transform
