
CC = gcc
CPPFLAGS = -MMD
CFLAGS = -Wall -Wextra
SRC = solver.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

solver: ${OBJ}

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} solver
