CC = gcc
CFLAGS = -Wall

ALL = prime prime_svc

all: $(ALL)

prime_svc: prime_svc.o prime_xdr.o prime_proc.o
	$(CC) $(CFLAGS) -o prime_svc prime_svc.o prime_xdr.o prime_proc.o

prime: prime.o prime_xdr.o
	$(CC) $(CFLAGS) -o prime prime.o prime_xdr.o

clean:
	rm -f $(ALL) *.o
