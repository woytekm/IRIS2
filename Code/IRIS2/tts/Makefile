include ../Makefile.inc

INCLUDES= $(TOPINCLUDES) -I../display -I ../include -I ../rss 

LIBSEARCH= -L . -L ../display
LIBS= -lxml2 -lz -lm -lWEH001602 -lbcm2835 -lcurl


all: librss-tts.a

librss-tts.a: tts-thread.c tts-indicator-thread.c time-to-speech.c
	$(CC) tts-thread.c -g -c $(INCLUDES)
	$(CC) tts-indicator-thread.c -g -c $(INCLUDES)
	$(CC) time-to-speech.c -g -c $(INCLUDES)
	$(AR) rcs librss-tts.a tts-thread.o tts-indicator-thread.o time-to-speech.o

clean:
	rm -f *.o librss-tts.a
