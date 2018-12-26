include ../../Makefile.inc

INCLUDES = $(TOOLINCLUDES) -I ../../include -I /usr/local/include -I /usr/include -I /usr/include/libxml2/

all:
	$(CC) tell_iris.c -o tell_iris $(CFLAGS) $(INCLUDES) 

clean:
	rm -f *.o tell_iris
