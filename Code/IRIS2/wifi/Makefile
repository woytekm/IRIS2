include ../Makefile.inc

INCLUDES= $(TOPINCLUDES) -I../display -I ../include -I ../rss 

LIBSEARCH= -L . -L ../display

all:
	$(CC) wifi_signal.c -g -c $(INCLUDES)
	$(AR) rcs libwifi.a wifi_signal.o

clean:
	rm -f *.o libwifi.a
