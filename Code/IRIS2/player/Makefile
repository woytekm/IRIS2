include ../Makefile.inc

INCLUDES= $(TOPINCLUDES) -I../display -I ../include -I ../tts

LIBSEARCH= -L . -L ../display

all:
	$(CC) player-thread.c -g -c $(INCLUDES)
	$(AR) rcs libplayer.a player-thread.o

reader:
	$(CC) newslist.c -g -c $(INCLUDES)
	$(CC) curlget.c -g -c $(INCLUDES)
	$(CC) rss-parser.c -g -c $(INCLUDES)
	$(CC) rss-reader.c -g -c $(INCLUDES)
	$(CC) rss-reader.o rss-parser.o curlget.o newslist.o -g -o rss-reader $(LIBS) $(LIBSEARCH)

clean:
	rm -f *.o rss-reader libplayer.a
