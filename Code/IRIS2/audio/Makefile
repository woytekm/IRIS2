include ../Makefile.inc

AMP_DRIVER = TAS5721

INCLUDES = $(TOPINCLUDES) -I ../include -I ../display -I ../tts 

all: libaudio.a 

libaudio.a: i2c.c audio-test.c AMP-control.c AMP-$(AMP_DRIVER).c AMP-$(AMP_DRIVER).h
	$(CC) AMP-control.c -g -c $(CFLAGS) $(INCLUDES)
	$(CC) AMP-$(AMP_DRIVER).c -g -c $(CFLAGS) $(INCLUDES)
	$(CC) i2c.c -g -c $(CFLAGS) $(INCLUDES)
	$(AR) rcs libaudio.a i2c.o AMP-control.o AMP-$(AMP_DRIVER).o

test:  libaudio.a
	$(CC) $(CFLAGS) audio-test.c -g -o audio-test -laudio -lbcm2835 -L .

clean:
	rm -f *.o libaudio.a audio-test

