CC = gcc
CFLAGS = -Wall -std=c17 -g

.PHONY: clean

main:
	$(CC) $(CFLAGS) -c main.c
	$(CC) $(CFLAGS) main.o -o main

test: main
	./main

clean:
	rm -f *.o main