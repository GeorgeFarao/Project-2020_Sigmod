CC = gcc -g

ex: main.o RBtree.o list.o HashTable.o
	$(CC) -o ex main.o  RBtree.o list.o HashTable.o

main.o: main.c
	$(CC) -c main.c

RBtree.o: RBtree.c
	$(CC) -c RBtree.c

list.o: list.c
	$(CC) -c list.c

HashTable.o: HashTable.c
	$(CC) -c HashTable.c


.PHONY: clean

clean:
	rm -f ex main.o RBtree.o list.o HashTable.o
	clear
