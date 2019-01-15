include ../Makefile.inc

INCLUDES= $(TOPINCLUDES) -I../display -I ../include -I ../rss 

LIBSEARCH= -L . -L ../display

all: libwifi.a

libwifi.a: wifi_signal.c
	$(CC) wifi_signal.c -c $(CFLAGS) $(INCLUDES)
	$(AR) rcs libwifi.a wifi_signal.o

clean:
	rm -f *.o libwifi.a
