include ../Makefile.inc

INCLUDES= $(TOPINCLUDES) -I../display -I ../include -I ../tts

LIBSEARCH= -L . -L ../display

all: libalarm.a

libalarm.a: alarm-thread.c
	$(CC) alarm-thread.c -g -c $(INCLUDES)
	$(AR) rcs libalarm.a alarm-thread.o

clean:
	rm -f *.o libalarm.a

