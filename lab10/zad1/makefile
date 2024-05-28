CC = gcc
CFLAGS = -Wall -std=c17 -g

.PHONY: all test clean

all: santaClaus

santaClaus:
	$(CC) $(CFLAGS) -c santaClaus.c
	$(CC) $(CFLAGS) santaClaus.o -o santaClaus

test:
	./santaClaus

clean:
	rm -f *.o santaClaus