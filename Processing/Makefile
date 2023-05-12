
CC = gcc
CPPFLAGS = -MMD
CFLAGS = -Wall -Wextra -O3 `pkg-config --cflags sdl2 SDL2_image`
LDFLAGS =
LDLIBS = `pkg-config --libs sdl2 SDL2_image` -lm

SRC = main.c hough_transform.c doubles_lists.c basic_utilities.c SDL_utilities.c grid_detection.c contour_manager.c img_upgrade.c ../preprocessing/rotation.c ../preprocessing/filter.c ../preprocessing/gaussian_blur.c ../preprocessing/utils.c ../preprocessing/median.c ../preprocessing/perspective.c ../preprocessing/dilate.c ../preprocessing/contrast.c ../preprocessing/sauvola.c ../preprocessing/threshold.c ../preprocessing/otsu.c ../preprocessing/invert.c ../preprocessing/adaptive_threshold.c crop.c ../preprocessing/canny.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

all: main

main: ${OBJ}

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} main
