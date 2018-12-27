include ../Makefile.inc

INCLUDES= $(TOPINCLUDES) -I../display -I ../include -I ../tts

LIBSEARCH= -L . -L ../display

all: libplayer.a

libplayer.a: player-thread.c
	$(CC) player-thread.c -g -c $(INCLUDES)
	$(AR) rcs libplayer.a player-thread.o

clean:
	rm -f *.o rss-reader libplayer.a
