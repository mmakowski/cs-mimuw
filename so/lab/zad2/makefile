CC=gcc
ALL=vscan

all: $(ALL)

vscan: vscan.o err.o
	$(CC) -o vscan -lpam -lpam_misc vscan.o err.o

clean:
	rm -f *.o $(ALL)
 
