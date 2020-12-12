CC = gcc -g

all: find_commons unit_test 

find_commons: main.o RBtree.o list.o HashTable.o jsonParser.o helpFunctions.o
	$(CC) -o find_commons main.o  RBtree.o list.o HashTable.o jsonParser.o helpFunctions.o -lm

main.o: main.c
	$(CC) -c main.c

RBtree.o: RBtree.c
	$(CC) -c RBtree.c -lm

list.o: list.c
	$(CC) -c list.c

HashTable.o: HashTable.c
	$(CC) -c HashTable.c

jsonParser: jsonParser.c
	$(CC) -c jsonParser.c

helpFunctions: helpFunctions.c
	$(CC) -c helpFunctions.c

unit_test: unit_test.o RBtree.o list.o HashTable.o jsonParser.o helpFunctions.o
	$(CC) -o unit_test unit_test.o  RBtree.o list.o HashTable.o jsonParser.o helpFunctions.o -lm

.PHONY: clean

clean:
	rm -f find_commons unit_test main.o RBtree.o list.o HashTable.o jsonParser.o helpFunctions.o unit_test.o output.csv different.csv
	clear
