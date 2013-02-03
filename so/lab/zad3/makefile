CFLAGS = -O6 -pipe -fomit-frame-pointer -Wall $(PERSIST) -g -I/usr/src/linux/include
LDFLAGS = -s
CC=gcc
BINDIR=/sbin
MODCFLAGS := $(CFLAGS) -DMODULE -D__KERNEL__ -DLINUX -DEXPORT_SYMTAB

ALL_READ_MAJOR=42
ALL_WRITE_MAJOR=43
USER_READ_MAJOR=44
USER_WRITE_MAJOR=45

all: kol_kom.o kol_kom_test /dev/all_read /dev/all_write /dev/user_read /dev/user_write 

kol_kom.o:	kol_kom.c  /usr/include/linux/version.h
	$(CC) $(MODCFLAGS) -c kol_kom.c 

kol_kom_test:	kol_kom_test.c
	$(CC) -Wall -o kol_kom_test kol_kom_test.c

/dev/all_read:
	if [ ! -c /dev/all_read ]; then mknod /dev/all_read c $(ALL_READ_MAJOR) 0; fi

/dev/all_write:
	if [ ! -c /dev/all_write ]; then mknod /dev/all_write c $(ALL_WRITE_MAJOR) 0; chmod 722 /dev/all_write; fi

/dev/user_read:
	if [ ! -c /dev/user_read ]; then mknod /dev/user_read c $(USER_READ_MAJOR) 0;fi

/dev/user_write:
	if [ ! -c /dev/user_write ]; then mknod /dev/user_write c $(USER_WRITE_MAJOR) 0; chmod 722 /dev/user_write; fi

clean:
	rm -f *.o kol_kom_test /dev/all_read /dev/all_write /dev/user_read /dev/user_write



