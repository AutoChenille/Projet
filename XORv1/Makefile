# Makefile

CC = gcc
CPPFLAGS = -MMD
CFLAGS = -Wall -Wextra
LDFLAGS =
LDLIBS = -lm

SRC = matrix.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

matrix: ${OBJ}

-include ${DEP}

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} matrix

# END
