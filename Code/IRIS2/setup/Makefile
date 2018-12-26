include ../Makefile.inc

INCLUDES= $(TOPINCLUDES) -I ../include -I ../display -I ../keyboard -I ../config

LIBSEARCH= -L . -L ../clock -L ../display


all: libsetup.a


libsetup.a:
	$(CC) setup.c -g -c $(INCLUDES)
	$(AR) rcs libsetup.a setup.o

clean:
	rm -f *.o libsetup.a
