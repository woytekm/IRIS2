include ../Makefile.inc

INCLUDES= $(TOPINCLUDES) -I../display -I ../include 

LIBSEARCH= -L . -L ../display
LIBS= -lxml2 -lz -lm -lWEH001602 -lbcm2835 -lcurl


all: librss-reader.a

librss-reader.a: newslist.c curlget.c rss-parser.c rss-reader-thread.c
	$(CC) newslist.c -g -c $(INCLUDES)
	$(CC) curlget.c -g -c $(INCLUDES)
	$(CC) rss-parser.c -g -c $(INCLUDES)
	$(CC) rss-reader-thread.c -g -c $(INCLUDES)
	$(AR) rcs librss-reader.a newslist.o curlget.o rss-parser.o rss-reader-thread.o

tts-reader: rss-parser.c curlget.c rss-tts-reader.c 
	$(CC) rss-parser.c -g -c $(INCLUDES)
	$(CC) curlget.c -g -c $(INCLUDES)
	$(CC) rss-tts-reader.c -g -c $(INCLUDES) 
	$(CC) rss-tts-reader.o rss-parser.o curlget.o -o rss-tts-reader $(LIBS) $(LIBSEARCH)

reader: newslist.c curlget.c rss-parser.c rss-reader.c 
	$(CC) newslist.c -g -c $(INCLUDES)
	$(CC) curlget.c -g -c $(INCLUDES)
	$(CC) rss-parser.c -g -c $(INCLUDES)
	$(CC) rss-reader.c -g -c $(INCLUDES)
	$(CC) rss-reader.o rss-parser.o curlget.o newslist.o -g -o rss-reader $(LIBS) $(LIBSEARCH)

clean:
	rm -f *.o rss-reader librss-reader.a
