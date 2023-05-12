# Makefile

CC = gcc
CPPFLAGS = -MMD
CFLAGS = -Wall -Wextra
LDFLAGS =
LDLIBS = `pkg-config --libs sdl2 SDL2_image` -lm

<<<<<<<< HEAD:NeuronalNetwork_Hexa/Makefile
SRC = main.c matrix.c neuronalNetwork.c BuildDataImage.c saveParams.c
========
SRC = GenDigits.c GenDigits.h
>>>>>>>> 993b6bc7b0e5e1f06751ec2cd0938ca4860c88f1:GenerateNumbers/Makefile
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
