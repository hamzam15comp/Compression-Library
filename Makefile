CC = gcc
OBJECTS = main.o lz77.o tree_lz.o huff.o bitio.o
project: $(OBJECTS)
	$(CC) -g -o project $(OBJECTS) -lm
	rm *.o

main.o:
	$(CC) -c main.c -w -g

lz77.o:
	$(CC) -c lz77.c -g

tree_lz.o:
	$(CC) -c tree_lz.c -g

huff.o:
	$(CC) -c huff.c -g

bitio.o:
	$(CC) -c bitio.c -g

clean all: 
	rm *.o project


