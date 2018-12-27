include ../Makefile.inc

INCLUDES = $(TOPINCLUDES) -I ../include

all: libWEH001602.a

libWEH001602.a: WEH001602-lib.c
	$(CC) WEH001602-lib.c -c $(INCLUDES)
	$(AR) rcs libWEH001602.a WEH001602-lib.o

test:
	$(CC)  WEH001602-test.c -o WEH001602-test -lWEH001602 -lbcm2835 -L .

clean:
	rm -f *.o libWEH001602.a WEH001602-test

