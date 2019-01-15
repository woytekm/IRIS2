include ../Makefile.inc

INCLUDES= $(TOPINCLUDES) -I ../include -I ../display -I ../keyboard -I ../config

LIBSEARCH= -L . -L ../clock -L ../display


all: libsetup.a


libsetup.a: setup.c
	$(CC) setup.c -c $(CFLAGS) $(INCLUDES)
	$(AR) rcs libsetup.a setup.o

clean:
	rm -f *.o libsetup.a
