GCC="$(PATH_CROSS_CC)gcc"
CC="$(PATH_CROSS_CC)gcc"
CFLAGS=$(CROSS_CFLAGS)

default: linux

SUN_OBJS=common.o SunOS.o
LINUX_OBJS=common.o Linux.o

linux-test: linux
	./slowdown -v 0.1 /bin/ls -l
	#./slowdown 2.1 ./cmd
	#./cmd > /dev/null 2>&1 & ./slowdown 2.1 $$!

sun-test: sun
	#./slowdown 0.1 /bin/ls -l
	#./slowdown 2.1 ./cmd
	./cmd > /dev/null 2>&1 & ./slowdown 2.1 $$!

sun: $(SUN_OBJS)
	$(CC) -o slowdown $(SUN_OBJS)

linux: $(LINUX_OBJS)
	$(CC) -ansi -pedantic -Wall -o slowdown $(LINUX_OBJS)

clean:
	rm -f slowdown slowdown2 core *.o

web: clean
	mkdir -p $(HOME)/public_html/slowdown
	tar cvf - . | gzip > $(HOME)/public_html/slowdown/slowdown.tar.gz 

