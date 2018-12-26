include ../Makefile.inc

INCLUDES= $(TOPINCLUDES) -I../display -I ../include -I ../tts

LIBSEARCH= -L . -L ../display

all:
	$(CC) alarm-thread.c -g -c $(INCLUDES)
	$(AR) rcs libalarm.a alarm-thread.o

clean:
	rm -f *.o libalarm.a

