CFLAGS = -O6 -pipe -fomit-frame-pointer -Wall $(PERSIST) -g -I/usr/src/linux/include
LDFLAGS = -s
CC=gcc
BINDIR=/sbin
MODCFLAGS := $(CFLAGS) -DMODULE -D__KERNEL__ -DLINUX -DEXPORT_SYMTAB

CHLOG_MAJOR=201

all: chlog.o /dev/chlog 

chlog.o:	chlog.c  /usr/include/linux/version.h
	$(CC) $(MODCFLAGS) -c chlog.c 

/dev/chlog:
	if [ ! -c /dev/chlog ]; then mknod /dev/chlog c $(CHLOG_MAJOR) 0; fi

clean:
	rm -f *.o /dev/chlog



