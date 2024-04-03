CC = gcc
CFLAGS = -Wall -std=c17 -g

.PHONY: test clean

main:
	$(CC) $(CFLAGS) -c main.c
	$(CC) $(CFLAGS) main.o -o main

test:
	make clean
	make main
	./main 5

clean:
	rm -f *.o main