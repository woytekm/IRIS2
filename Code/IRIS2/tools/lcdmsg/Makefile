include ../../Makefile.inc

INCLUDES = $(TOOLINCLUDES) -I ../../include


all:
	$(CC) WEH001602-lib.c -c $(INCLUDES)
	$(AR) rcs libWEH001602.a WEH001602-lib.o
	$(CC)  lcdmsg.c -o lcdmsg -lWEH001602 -lbcm2835 -L . -L $(PATH_FS)/usr/lib/

clean:
	rm -f *.o libWEH001602.a lcdmsg

