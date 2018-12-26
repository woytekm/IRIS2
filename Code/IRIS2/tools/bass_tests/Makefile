include ../../Makefile.inc

INCLUDES = $(TOOLINCLUDES) -I ../../include
LIBS = -lbass -lbcm2835 -L../../../../bass/bass/libs/ -L../../../../bcm2835/bcm2835/src/

all:
	$(CC) bass_device_info.c -o bass_device_info $(CFLAGS) $(INCLUDES) $(LIBS)

clean:
	rm -f *.o bass_device_info
