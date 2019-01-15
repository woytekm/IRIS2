include ../Makefile.inc

INCLUDES = $(TOPINCLUDES) -I ../include

all: libmatrix.a

libmatrix.a: matrixlib.c matrix-thread.c
	$(CC) matrixlib.c -c $(CFLAGS) $(INCLUDES)
	$(CC) matrix-thread.c -c $(CFLAGS) $(INCLUDES)
	$(AR) rcs libmatrix.a matrixlib.o matrix-thread.o

clean:
	rm -f *.o *.a 

