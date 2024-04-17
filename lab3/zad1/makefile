CC = gcc
CFLAGS = -Wall -std=c17 -g
IF = input_files
OF = output_files

.PHONY: all reverse_single_short reverse_block_short \
reverse_single_long reverse_block_long \
reverse_compare_block_binary test_reverse_types \
test_tac_rev_binary test_tac_rev_long \
test_tac_rev_short clean

all:
	make clean
	make main

main:
	$(CC) $(CFLAGS) -c main.c
	$(CC) $(CFLAGS) main.o -o main


## short.txt ##

reverse_single_short: main
	./main s ./$(IF)/short.txt ./$(OF)/reversed_short.txt

reverse_block_short: main
	./main b ./$(IF)/short.txt ./$(OF)/reversed_short.txt


## long.txt ##

reverse_single_long: main
	./main s ./$(IF)/long.txt ./$(OF)/reversed_long.txt

reverse_block_long: main
	./main b ./$(IF)/long.txt ./$(OF)/reversed_long.txt


## binary ##

reverse_block_binary: main
	./main b ./main.o ./$(OF)/reversed_binary.txt

reverse_compare_block_binary: main
	./main b ./main.o ./$(OF)/tmp1
	./main b ./$(OF)/tmp1 ./$(OF)/tmp2
	diff -s ./$(OF)/tmp2 ./main.o


## tests ##

test_reverse_types: main
	./main s ./$(IF)/long.txt ./$(OF)/reversed_long_single.txt
	./main b ./$(IF)/long.txt ./$(OF)/reversed_long_block.txt
	diff -s ./$(OF)/reversed_long_single.txt ./$(OF)/reversed_long_block.txt

test_tac_rev_long: reverse_block_long
	tac < ./$(IF)/long.txt | rev > ./$(OF)/rev_long.txt
	diff -s ./$(OF)/rev_long.txt ./$(OF)/reversed_long.txt

test_tac_rev_short: reverse_block_short
	tac < ./$(IF)/short.txt | rev > ./$(OF)/rev_short.txt
	diff -s ./$(OF)/rev_short.txt ./$(OF)/reversed_short.txt


## clean ##

clean:
	rm -f *.o main