CC = gcc
CFLAGS = -Wall -std=c17 -g

.PHONY: all test clean

all: countdown

countdown:
	$(CC) $(CFLAGS) -c countdown.c
	$(CC) $(CFLAGS) countdown.o -o countdown

test: all
	./countdown

clean:
	rm -f *.o countdown