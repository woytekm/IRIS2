include Makefile.inc

GCC="$(PATH_CROSS_CC)gcc"
CC="$(PATH_CROSS_CC)gcc"
CFLAGS=$(CROSS_CFLAGS)

MAKE = make
LIBS = -lpthread -lsystem -lplclock -lrss-reader -lplayer -lkeyboard -lconfig -lWEH001602 -lsetup -lrss-tts -lalarm -lwifi -lbcm2835  -lxml2 -lcurl -lbass -lbassflac -lbass_aac -lmicrohttpd -lnettle -lhogweed -lgnutls -lgmp -lgpg-error -lgcrypt -lcrypt
LIBSEARCH = -L ./system -L ./clock -L ./display -L ./rss -L ./player -L ./keyboard -L ./config -L ./setup -L ./tts -L ./alarm -L ./wifi -L $(PATH_FS)/usr/lib/
INCLUDES = $(MAININCLUDES) -I system -I player -I include -I clock -I display -I rss -I keyboard -I setup -I tts -I config -I alarm -I wifi
MAIN_EXE = Iris
TELL_IRIS = tell_iris
SLOWDOWN_EXE = slowdown
LCDMSG_EXE = lcdmsg
WIFI_QUAL_EXE = quality
GRAPHIC_TST = graphic_tst
BASS_TEST_01 = bass_device_info
VERSION_H = version.h
MAIN_EXE_STATIC = Iris-static
PLLIBS = libsystem.a libplclock.a libWEH001602.a librss-reader.a libplayer.a libkeyboard.a libconfig.a libsetup.a librss-tts.a libalarm.a libwifi.a
DIRS = include system clock display player config rss keyboard setup tts alarm wifi tools/lcdmsg tools/slowdown tools/bass_tests tools/tell_iris tools/graphics


all: $(VERSION_H) $(PLLIBS) $(MAIN_EXE) $(TELL_IRIS) $(SLOWDOWN_EXE) $(LCDMSG_EXE) $(WIFI_QUAL_EXE) $(BASS_TEST_01) $(GRAPHIC_TST)

all_static:	$(PLLIBS) $(MAIN_EXE_STATIC)

$(MAIN_EXE):
	$(CC) -c main.c $(INCLUDES)
	$(CC) -o $(MAIN_EXE) main.o $(LIBS) $(LIBSEARCH)

$(GRAPHIC_TST):
	cd tools/graphics; $(MAKE)

$(TELL_IRIS):
	cd tools/tell_iris; $(MAKE)

$(SLOWDOWN_EXE):
	cd tools/slowdown; $(MAKE)

$(LCDMSG_EXE):
	cd tools/lcdmsg; $(MAKE)

$(WIFI_QUAL_EXE):
	cd tools/wifi_stats; $(MAKE)

$(BASS_TEST_01):
	cd tools/bass_tests; $(MAKE)

$(MAIN_EXE_STATIC):
	$(CC) -c main.c $(INCLUDES)
	$(CC) -static -o $(MAIN_EXE) main.o $(LIBS) $(LIBSEARCH)

$(VERSION_H):
	cd include; $(MAKE) $(VERSION_H)

libsystem.a:
	cd system; $(MAKE) 

libplclock.a:
	cd clock; $(MAKE)

libWEH001602.a:
	cd display; $(MAKE)

librss-reader.a:
	cd rss; $(MAKE)

libplayer.a:
	cd player; $(MAKE)

libkeyboard.a:
	cd keyboard; $(MAKE)

libconfig.a:
	cd config; $(MAKE)

libsetup.a:
	cd setup; $(MAKE)

librss-tts.a:
	cd tts; $(MAKE)

libalarm.a:	
	cd alarm; $(MAKE)

libwifi.a:
	cd wifi; $(MAKE)

install:
	mv $(MAIN_EXE) /usr/local/bin

clean:
	rm -f *.o *.a core $(MAIN_EXE)
	for d in $(DIRS); do (cd $$d; $(MAKE) clean ); done
