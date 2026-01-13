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

DIST =		t_ezPacket 

CFLAGS =	-Wall -g -O2 -MD
CFLAGS += -D_DEBUG #debugme

include         ../../../Makefile.Defines/Makefile.Defines.$(PLATFORM)

CFLAGS += -I$(HOME)/libs/include/ezThread


CLEAN_FILES += *actual* *rewrite* a.txt

#特殊功能区
#

CPPFLAGS =	$(CFLAGS) 

#LIBS =          -L$(HOME)/libs/libsigc++-$(PLATFORM)/lib/ -lsigc-2.0
LIBS +=		-L$(HOME)/libs/lib -lezThread-$(PLATFORM) -lpthread

t_ezPacket:t_ezPacket.o Packet.o
	$(CPP) -o $@ $^ $(LIBS)

FILES =		*.h *.cpp Makefile*

-include	*.d

CLEAN_FILES = $(DIST) *.o *.gdb *.d *.cfbk *.tgz

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

.cpp.o:
	$(CPP) -c $(CFLAGS) $< -o $@

all: $(DIST)

clean: 
	rm -f $(CLEAN_FILES)

