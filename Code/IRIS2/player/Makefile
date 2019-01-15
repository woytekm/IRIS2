include ../Makefile.inc

INCLUDES= $(TOPINCLUDES) -I../display -I ../include -I ../tts -I ../matrix 

LIBSEARCH= -L . -L ../display

all: libplayer.a

libplayer.a: player-thread.c
	$(CC) player-thread.c -c $(CFLAGS) $(INCLUDES)
	$(AR) rcs libplayer.a player-thread.o

clean:
	rm -f *.o rss-reader libplayer.a
