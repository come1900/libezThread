# Copyright (C) 2012  WuJunjie(Joy.Woo@hotmail.com)

# platforms:
#  linux-x86-32
#  uclinux-s3c2510

PLATFORM = linux
#PLATFORM = linux-x86-32
#PLATFORM = linux-hi3515
#PLATFORM = linux-arm

VERSION =	1.0
DIFF_VERSION =	0.9

DIST =		Thread \
			ThreadMsg \
			SemaphoreMutexLock	\
			MyObserver

# need libsigc++
# DIST =		Observer


CFLAGS =	-Wall -g -O2 -MD
CFLAGS += -D_DEBUG #debugme

include         ../../lazaru/Makefile.Defines/Makefile.Defines.$(PLATFORM)

CFLAGS += -I$(HOME)/libs/include/ezThread

# libsigc
CFLAGS += -I$(HOME)/libs/libsigc++-$(PLATFORM)/lib/sigc++-2.0/include
CFLAGS += -I$(HOME)/libs/libsigc++-$(PLATFORM)/include/sigc++-2.0

CLEAN_FILES = $(DIST) *.o *.gdb *.d *.cfbk *.tgz
CLEAN_FILES += *actual* *rewrite* a.txt

#特殊功能区
#

CPPFLAGS =	$(CFLAGS) 

#LIBS =          -L$(HOME)/libs/libsigc++-$(PLATFORM)/lib/ -lsigc-2.0
LIBS +=		-L$(HOME)/libs/lib -lezThread-$(PLATFORM) -lpthread

Thread:	Thread.o
	$(CPP) -o $@ $^ $(LIBS)

ThreadMsg:ThreadMsg.o
	$(CPP) -o $@ $^ $(LIBS)

SemaphoreMutexLock:	SemaphoreMutexLock.o
	$(CPP) -o $@ $^ $(LIBS)

Observer:Observer.o
	$(CPP) -o $@ $^ -L$(HOME)/libs/libsigc++-$(PLATFORM)/lib/ -lsigc-2.0 $(LIBS)
MyObserver:MyObserver.o
	$(CPP) -o $@ $^ $(LIBS)
	
FILES =		*.h *.cpp Makefile*

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

.cpp.o:
	$(CPP) -c $(CFLAGS) $< -o $@

all: $(DIST)

tar:	clean
		tar -czf $(DIST).tgz $(FILES)

clean: 
	rm -f $(CLEAN_FILES)

-include	*.d
