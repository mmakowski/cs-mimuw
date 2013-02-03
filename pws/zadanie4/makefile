CC=cc
CFLAGS=-Wall -D_REENTRANT -D_LIBC_REENTRANT -DMK_KOPIUJ
LDFLAGS=-Wall -lpthread
ALL=sumth sumipc sumfifo obliczeniowy_fifo obliczeniowy_ipc 

all: watki fifo ipc

watki: err.o suma_watki.o suma_ascii.o
	$(CC) -Wall -DMK_WATKI -c sum.c
	$(CC) $(CFLAGS) -lpthread -o sumth sum.o err.o suma_watki.o suma_ascii.o

fifo: err.o suma_fifo.o obliczeniowy_fifo
	$(CC) -Wall -DMK_FIFO -c sum.c
	$(CC) $(CFLAGS) -o sumfifo sum.o err.o suma_fifo.o

ipc: err.o suma_ipc.o obliczeniowy_ipc
	$(CC) -Wall -DMK_IPC -c sum.c
	$(CC) $(CFLAGS) -o sumipc sum.o err.o suma_ipc.o
 
obliczeniowy_fifo: obliczeniowy_fifo.o err.o suma_ascii.o
	$(CC) -o obliczeniowy_fifo obliczeniowy_fifo.o err.o suma_ascii.o

obliczeniowy_ipc: obliczeniowy_ipc.o err.o suma_ascii.o
	$(CC) -o obliczeniowy_ipc obliczeniowy_ipc.o err.o suma_ascii.o

clean:
	rm -f *.o $(ALL)