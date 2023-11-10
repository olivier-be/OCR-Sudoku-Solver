CC = gcc
CFLAGS = -Wall -Wextra -fsanitize=address -g -lm

all: test

test: test.o
	$(CC) $(CFLAGS) -o test test.o

test.o: network.c network.h mnist.h train_network.c save_network.c
	$(CC) $(CFLAGS) -c network.c -o test.o

.PHONY: clean

clean:
	$(RM) test test.o
