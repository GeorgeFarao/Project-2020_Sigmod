CC = gcc -g

ex: main.o RBtree.o list.o
	$(CC) -o ex main.o  RBtree.o list.o 

main.o: main.c
	$(CC) -c main.c

RBtree.o: RBtree.c
	$(CC) -c RBtree.c

list.o: list.c
	$(CC) -c list.c


.PHONY: clean

clean:
	rm -f ex main.o RBtree.o list.o
	clear
