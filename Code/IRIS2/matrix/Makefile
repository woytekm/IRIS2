include ../Makefile.inc

INCLUDES = $(TOPINCLUDES) -I ../include

all: libmatrix.a

libmatrix.a: matrixlib.c
	$(CC) matrixlib.c -c $(INCLUDES)
	$(AR) rcs libmatrix.a matrixlib.o

clean:
	rm -f *.o *.a 

