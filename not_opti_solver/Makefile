
CC = gcc
CPPFLAGS = -MMD
CFLAGS = -Wall -Wextra
<<<<<<< HEAD:Solveur/Makefile
LDFLAGS =
LDLIBS =

SRC = main.c solveur.c
=======
SRC = solver.c
>>>>>>> main:not_opti_solver/Makefile
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

solver: ${OBJ}

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} solver
