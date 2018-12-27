include ../Makefile.inc

INCLUDES= $(TOPINCLUDES) -I ../include  -I ../display -I ../tts

LIBSEARCH= -L . 

all: libsystem.a

libsystem.a: system.c indicators.c commands.c websrv.c REST.c webadmin.c
	$(CC) system.c -g -c $(INCLUDES)
	$(CC) indicators.c -g -c $(INCLUDES)
	$(CC) commands.c -g -c $(INCLUDES)
	$(CC) websrv.c -g -c $(INCLUDES)
	$(CC) REST.c -g -c $(INCLUDES)
	$(CC) webadmin.c -g -c $(INCLUDES)
	$(AR) rcs libsystem.a system.o indicators.o commands.o websrv.o REST.o webadmin.o

clean:
	rm -f *.o libsystem.a
