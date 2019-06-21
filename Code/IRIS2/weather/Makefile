include ../Makefile.inc

INCLUDES = $(TOPINCLUDES) -I ../include -I ./weather-scenes

all: libweather.a

libweather.a: weather.h weather-api.c weather-scenery.c json-simple-parser.c
	$(CC) weather-api.c -c $(CFLAGS) $(INCLUDES)
	$(CC) weather-scenery.c -c $(CFLAGS) $(INCLUDES)
	$(CC) json-simple-parser.c -c $(CFLAGS) $(INCLUDES)
	$(AR) rcs libweather.a weather-api.o weather-scenery.o json-simple-parser.o

clean:
	rm -f *.o *.a 

