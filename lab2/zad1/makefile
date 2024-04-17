CC = gcc
CFLAGS = -Wall -std=c17 -g -O0

.PHONY.: lib_static lib_shared main_static \
main_shared main_dynamic test test_static \
test_shared test_dynamic clean

main: clean
	$(CC) $(CFLAGS) -c main.c
	$(CC) $(CFLAGS) -c collatz.c
	$(CC) $(CFLAGS) main.o collatz.o -o main

# ==================
# === biblioteki ===
# ==================

# kompilacja jako biblioteka statyczna
## ar - tworzy biblioteke statyczna i wrzuca tam collatz.o
## opcje:
### -r - insert object files or replace existing ones
### -c - create a library
### -s - create/update an index (ranlib libcollatz.a)
lib_static:
	$(CC) $(CFLAGS) -c collatz.c
	ar -rcs libcollatz.a collatz.o

# kompilacja jako biblioteka wspoldzielona
### -fpic - position independent code
### -lc - check 
lib_shared:
	$(CC) $(CFLAGS) -fpic -c collatz.c
	$(CC) $(CFLAGS) -o libcollatz.so.1.0 collatz.o \
	-shared -Wl,-soname,libcollatz.so.1 -lc
	ln -s libcollatz.so.1.0 libcollatz.so.1
	ln -s libcollatz.so.1 libcollatz.so


# ============
# === main ===
# ============

main_static: clean lib_static
	$(CC) $(CFLAGS) -c main.c
	$(CC) $(CFLAGS) main.o libcollatz.a -o main

### -L ./ - szukanie biblioteki w katalogu ./ w trakcie linkowania
### -Wl,-rpath,. - szukanie biblioteki w . runtime search, dynamic linking
main_shared: clean lib_shared
	$(CC) $(CFLAGS) -c main.c
	$(CC) $(CFLAGS) main.o -lcollatz -L ./ -Wl,-rpath,. -o main

main_dynamic: clean lib_shared
	$(CC) $(CFLAGS) -c main.c
	$(CC) $(CFLAGS) -D LIB_DYNAMIC main.o -lcollatz -L ./ -Wl,-rpath,. -o main

test: main
	./main

test_static: main_static
	./main
	
test_shared: main_shared
	./main

test_dynamic: main_dynamic
	./main

clean:
	rm -f *.o *.a *.so *.so.[0-9]* *.so.[0-9]*.[0-9]* main
