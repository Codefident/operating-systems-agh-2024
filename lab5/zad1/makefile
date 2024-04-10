CC = gcc
CFLAGS = -Wall -std=c17 -g

.PHONY: test clean

main:
	$(CC) $(CFLAGS) -c main.c
	$(CC) $(CFLAGS) main.o -o main

test:
	./main ignore
	./main handler
	./main mask
	./main none

clean:
	rm -f *.o main