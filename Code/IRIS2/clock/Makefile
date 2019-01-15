include ../Makefile.inc

INCLUDES = $(TOPINCLUDES) -I ../include  -I ../display  -I ../tts

all: libplclock.a

libplclock.a: plclock.c snooze-thread.c
	$(CC) -c plclock.c $(CFLAGS) $(INCLUDES) 
	$(CC) -c snooze-thread.c  $(CFLAGS) $(INCLUDES) 
	$(AR) rcs libplclock.a plclock.o snooze-thread.o

clean:
	rm -f *.o libplclock.a

