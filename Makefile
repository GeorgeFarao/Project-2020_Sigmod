CC = gcc -g -Wall

all: find_commons unit_test 

find_commons: main.o RBtree.o list.o HashTable.o jsonParser.o helpFunctions.o dataList.o logistic_regression.o
	$(CC) -o find_commons main.o  RBtree.o list.o HashTable.o jsonParser.o helpFunctions.o dataList.o logistic_regression.o -lm

main.o: main.c
	$(CC) -c main.c

RBtree.o: RBtree.c
	$(CC) -c RBtree.c -lm

list.o: list.c
	$(CC) -c list.c

HashTable.o: HashTable.c
	$(CC) -c HashTable.c

jsonParser.o: jsonParser.c
	$(CC) -c jsonParser.c

helpFunctions.o: helpFunctions.c
	$(CC) -c helpFunctions.c

dataList.o: dataList.c
	$(CC) -c dataList.c

logistic_regression.o: logistic_regression.c
	$(CC) -c logistic_regression.c

unit_test: unit_test.o RBtree.o list.o HashTable.o jsonParser.o helpFunctions.o dataList.o logistic_regression.o
	$(CC) -o unit_test unit_test.o  RBtree.o list.o HashTable.o jsonParser.o helpFunctions.o dataList.o logistic_regression.o -lm

.PHONY: clean

clean:
	rm -f find_commons unit_test main.o RBtree.o list.o HashTable.o jsonParser.o helpFunctions.o unit_test.o dataList.o logistic_regression.o output.csv different.csv
	clear
