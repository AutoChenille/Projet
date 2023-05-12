CC = gcc
CPPFLAGS = -MMD
CFLAGS = -Wall -Wextra -O3 `pkg-config --cflags --libs gtk+-3.0`
LDFLAGS =
LDLIBS = `pkg-config --libs sdl2 SDL2_image gtk+-3.0` -lm

SRC = main.c sudoku_solver/solver9.c sudoku_solver/solver16.c sudoku_solver/useful.c Preprocessing/filter.c Preprocessing/dilate.c Preprocessing/median.c Preprocessing/invert.c Preprocessing/threshold.c Preprocessing/canny.c Preprocessing/otsu.c Preprocessing/rotation.c Preprocessing/utils.c Preprocessing/contrast.c Preprocessing/gaussian_blur.c Preprocessing/perspective.c Preprocessing/sauvola.c Processing/basic_utilities.c Processing/hough_transform.c Processing/crop.c Processing/grid_detection.c Processing/SDL_utilities.c Processing/contour_manager.c Processing/doubles_lists.c Processing/img_upgrade.c NeuronalNetwork/mainNN.c Ressources/neuronalNetwork.c Ressources/saveParams.c Ressources/matrix.c NeuronalNetwork_Hexa/mainNNH.c Ressources/BuildDataImage.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

all: main

main: ${OBJ}

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} main
