CC = gcc
CFLAGS = -Wall -std=c17 -g

all:
	@make server
	@make client

server:
	$(CC) $(CFLAGS) -c server.c
	$(CC) $(CFLAGS) server.o -o server

client:
	$(CC) $(CFLAGS) -c client.c
	$(CC) $(CFLAGS) client.o -o client

clean:
	rm -f *.o server client
	