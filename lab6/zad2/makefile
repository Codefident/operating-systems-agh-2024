CC = gcc
CFLAGS = -Wall -std=c17 -g

.PHONY: all clean

all:
	@make clean
	@make main
	@make integral

main:
	$(CC) $(CFLAGS) -c main.c
	$(CC) $(CFLAGS) main.o -o main

integral:
	$(CC) $(CFLAGS) -c integral.c
	$(CC) $(CFLAGS) integral.o -o integral

clean:
	rm -f *.o main integral fifo
