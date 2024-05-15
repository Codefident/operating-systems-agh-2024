CC = gcc
CFLAGS = -Wall -std=c17 -g

TARGETS = buffer user printer

.PHONY: all clean

all: buffer user printer

buffer: buffer.o semaphore.o
	$(CC) $(CFLAGS) -o buffer buffer.o semaphore.o

user: user.o semaphore.o
	$(CC) $(CFLAGS) -o user user.o semaphore.o

printer: printer.o semaphore.o
	$(CC) $(CFLAGS) -o printer printer.o semaphore.o

buffer.o: buffer.c print_system.h
	$(CC) $(CFLAGS) -c buffer.c

user.o: user.c print_system.h
	$(CC) $(CFLAGS) -c user.c

printer.o: printer.c print_system.h
	$(CC) $(CFLAGS) -c printer.c

semaphore.o: semaphore.c print_system.h
	$(CC) $(CFLAGS) -c semaphore.c

clean:
	rm -f *.o buffer user printer
