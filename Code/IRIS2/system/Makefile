include ../Makefile.inc

INCLUDES= $(TOPINCLUDES) -I ../include  -I ../display -I ../tts

LIBSEARCH= -L . 

all: libsystem.a

libsystem.a: system.c indicators.c commands.c websrv.c REST.c webadmin.c
	$(CC) system.c -c $(CFLAGS) $(INCLUDES)
	$(CC) indicators.c -c $(CFLAGS) $(INCLUDES)
	$(CC) commands.c -c $(CFLAGS) $(INCLUDES)
	$(CC) websrv.c -c $(CFLAGS) $(INCLUDES)
	$(CC) REST.c -c $(CFLAGS) $(INCLUDES)
	$(CC) webadmin.c -c $(CFLAGS) $(INCLUDES)
	$(AR) rcs libsystem.a system.o indicators.o commands.o websrv.o REST.o webadmin.o

clean:
	rm -f *.o libsystem.a
