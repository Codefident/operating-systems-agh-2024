CC = gcc
CFLAGS = -Wall -std=c17 -g

.PHONY: all test clean

all:
	make clean
	make main

main:
	$(CC) $(CFLAGS) -c main.c
	$(CC) $(CFLAGS) main.o -o main

test: all
	./main
	wc --bytes *

clean:
	rm -f *.o main