TOPINCLUDES = -I bluetooth -I player -I include -I clock -I display -I rss -I keyboard -I setup -I tts -I /usr/include/libxml2
CC = gcc
MAKE = make
LIBS = -lm -ljansson -lpthread -lplclock -lrss-reader -lplayer -lkeyboard -lconfig -lWEH001602 -lsetup -lrss-tts -lsystem -lalarm -lwifi -lbcm2835  -lxml2 -lcurl -lbass -lbassflac -lbass_aac -lcrypt -lmicrohttpd -lmatrix -laudio -lbluetooth -lweather
LIBSEARCH = -L ./clock -L ./display -L ./rss -L ./player -L ./keyboard -L ./config -L ./setup -L ./tts -L ./system -L ./alarm -L ./wifi -L ./matrix -L ./audio -L ./bluetooth -L ./weather
LIBSEARCH_MAIN = -L ../clock -L ../display -L ../rss -L ../player -L ../keyboard -L ../config -L ../setup -L ../tts -L ../system -L ../alarm -L ../wifi -L ../matrix -L ../audio -L ../bluetooth -L ../weather
INCLUDES = -I wifi -I alarm -I config -I system -I player -I include -I clock -I display -I rss -I keyboard -I setup -I tts -I audio -I bluetooth -I /usr/include/libxml2 -I weather
MAIN_EXE = Iris
MAIN_EXE_STATIC = Iris-static
EXEDEPS = ../clock/libplclock.a ../display/libWEH001602.a ../rss/librss-reader.a ../player/libplayer.a ../keyboard/libkeyboard.a ../config/libconfig.a ../setup/libsetup.a ../tts/librss-tts.a ../system/libsystem.a ../alarm/libalarm.a ../wifi/libwifi.a ../matrix/libmatrix.a ../audio/libaudio.a ../bluetooth/libbluetooth.a ../weather/libweather.a
CFLAGS =  -Wno-implicit-function-declaration -Wno-int-conversion -Wno-implicit-int -Wno-incompatible-pointer-types -g
