include ../Makefile.inc

INCLUDES = $(TOPINCLUDES) -I ../include -I ../display -I ../tts -I ../system  -I ../clock -I ../setup -I ../rss  -I ../player -I ../keyboard -I ../config -I ../alarm -I ../wifi -I ../matrix -I ../audio -I ../bluetooth -I ../weather

all: $(MAIN_EXE)

clean:
	rm -f main.o $(MAIN_EXE) $(MAIN_EXE_STATIC)

$(MAIN_EXE): $(EXEDEPS) main.c
	$(CC) -c main.c $(CFLAGS) $(INCLUDES)
	$(CC) -o $(MAIN_EXE) main.o $(LIBS) $(LIBSEARCH_MAIN)

$(MAIN_EXE_STATIC): $(PLLIBS) main.c
	$(CC) -c main.c $(CFLAGS) $(INCLUDES)
	$(CC) -static -o $(MAIN_EXE) main.o $(LIBS) $(LIBSEARCH_MAIN)

