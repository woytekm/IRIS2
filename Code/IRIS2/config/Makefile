include ../Makefile.inc

INCLUDES= $(TOPINCLUDES) -I ../include

LIBSEARCH= -L . -L ../clock -L ../display

LIBS = -lplclock -lWEH001602 -lbcm2835


all: libconfig.a


libconfig.a:
	$(CC) config.c -g -c $(INCLUDES)
	$(CC) persistence-thread.c -g -c $(INCLUDES)
	$(AR) rcs libconfig.a config.o persistence-thread.o

test:
	make libconfig.a
	$(CC) config-test.c -g -c $(INCLUDES)
	$(CC) config-test.o config.o -o config-test $(LIBS) $(LIBSEARCH)

clean:
	rm -f *.o config-test libconfig.a
