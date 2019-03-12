include ../Makefile.inc

INCLUDES = $(TOPINCLUDES) -I ../include  

all: libbluetooth.a 

libbluetooth.a: btplayer-proxy.c btctl-proxy.c bluetooth-threads.c btctl-proxy.h
	$(CC) btctl-proxy.c -c $(CFLAGS) $(INCLUDES)
	$(CC) btplayer-proxy.c -c $(CFLAGS) $(INCLUDES)
	$(CC) -c bluetooth-threads.c -c $(CFLAGS) $(INCLUDES)
	$(AR) rcs libbluetooth.a btctl-proxy.o btplayer-proxy.o bluetooth-threads.o

clean:
	rm -f *.o libbluetooth.a bluetooth-test

