CC = gcc
CFLAGS = -Wall -std=gnu11 -g

.PHONY: all clean

all:
	@make catcher
	@make sender

catcher:
	$(CC) $(CFLAGS) -c catcher.c
	$(CC) $(CFLAGS) catcher.o -o catcher

sender:
	$(CC) $(CFLAGS) -c sender.c
	$(CC) $(CFLAGS) sender.o -o sender

clean:
	rm -f *.o catcher sender