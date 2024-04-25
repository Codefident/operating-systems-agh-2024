CC = gcc
CFLAGS = -Wall -std=c17 -g

.PHONY: clean

main:
	$(CC) $(CFLAGS) -c main.c
	$(CC) $(CFLAGS) main.o -o main

test: main
	./main 0.00001 4

clean:
	rm -f *.o main