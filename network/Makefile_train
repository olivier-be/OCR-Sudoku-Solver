# Makefile

CC = gcc
CPPFLAGS = -MMD
CFLAGS = -Wall -Wextra `pkg-config --cflags sdl2` -g3 -fsanitize=address
LDLIBS = 
LDFLAGS = -fsanitize=address
LDLIBS = `pkg-config --libs sdl2` -lSDL2_image -lm


SRC = network.c load_picture.c train_network.c save_network.c main.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

all: main
main : ${OBJ}

-include ${DEP}

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} load train save main
# END
