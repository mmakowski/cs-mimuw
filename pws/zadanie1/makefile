CC=cc
CFLAGS=-Wall
ALL=wezel bst

all: $(ALL)
 
wezel: wezel.o err.o drzewo.o
	$(CC) -o wezel wezel.o err.o drzewo.o

bst: bst.o err.o drzewo.o
	$(CC) -o bst bst.o err.o drzewo.o

clean:
	rm -f *.o $(ALL)