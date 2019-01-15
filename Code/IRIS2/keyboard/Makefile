include ../Makefile.inc

INCLUDES = $(TOPINCLUDES) -I ../include -I ../display -I ../tts 

all: libkeyboard.a 

libkeyboard.a: keyboard-scan.c keyboard-thread.c keyboard.h
	$(CC) keyboard-scan.c -c $(CFLAGS) $(INCLUDES)
	$(CC) -c keyboard-thread.c -c $(CFLAGS) $(INCLUDES)
	$(AR) rcs libkeyboard.a keyboard-scan.o keyboard-thread.o

test:
	$(CC)  keyboard-test.c -o keyboard-test -lkeyboard -lbcm2835 -L .

clean:
	rm -f *.o libkeyboard.a keyboard-test

