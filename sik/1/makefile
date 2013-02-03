CC = gcc
CFLAGS = -Wall
LDFLAGS = -lpthread
ALL = tftp_serv tftp_clnt

all: $(ALL)

tftp_serv: ttcp.o tftp_serv.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o tftp_serv tftp_serv.o ttcp.o

tftp_clnt: ttcp.o tftp_clnt.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o tftp_clnt tftp_clnt.o ttcp.o


clean:
	rm -f *.o $(ALL)




