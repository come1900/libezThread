#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
#
# Makefile - D:\Works\MySVN\ivideo\library\libXML\Build\
#
# Copyright (C) 2011 ezlibs.com, All Rights Reserved.
#
# $Id: Makefile 1 2011-10-22 08:14:10 WuJunjie Exp $
#
# explain
#      exp
#
# Update:Year-Month-Day  SVN-Author  Modification
#    2011-10-22 08:14 WuJunjie Create
#
#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-#

#CROSS = arm-linux-uclibc-

#CPP	= @echo " g++ $@"; $(CROSS)g++
#CC	= @echo " gcc $@"; $(CROSS)gcc
#LD	= @echo " ld  $@"; $(CROSS)ld
#AR  = @echo " ar  $@"; $(CROSS)ar
#RM	= @echo " RM	$@"; rm -f
#STRIP	= @echo " strip  $@"; $(CROSS)strip
CPP	= $(CROSS)g++
CC	= $(CROSS)gcc
LD	= $(CROSS)ld
AR  = $(CROSS)ar
RM	= rm -f
STRIP = $(CROSS)strip

SRC_PATH=../src

CFLAGS += -DLINUX
CFLAGS += -Wall -O2 -Os
CFLAGS += -D_REENTRANT -msoft-float
CFLAGS += -I$(SRC_PATH)

OBJS_GENERALAGENT = \
		$(SRC_PATH)/EZSemaphore.o \
		$(SRC_PATH)/EZMutex.o \
		$(SRC_PATH)/EZLock.o \
		$(SRC_PATH)/EZMsgQue.o \
		$(SRC_PATH)/EZPoolAllocator.o\
		$(SRC_PATH)/EZThread.o \
		$(SRC_PATH)/EZTimer.o

OBJS_GENERALAGENT += \
		$(SRC_PATH)/api_system.o    \
		$(SRC_PATH)/api_thread.o
		
LDFLAGS += -lpthread

OBJS = tt.o

TARGET = \
		tt \
		MyObserver \
		testThreadMsg \
		tSemaphoreMutexLock

all: $(TARGET)
	@#./tt
	@#./testThreadMsg

#OBJS_TT = tt.o
#OBJS_TT += $(OBJS_GENERALAGENT)

tt:  tt.o $(OBJS_GENERALAGENT)
	$(CPP) -o $@ $^ $(LDFLAGS)
	@#$(STRIP) $@

testThreadMsg:  testThreadMsg.o $(OBJS_GENERALAGENT)
	$(CPP) -o $@ $^ $(LDFLAGS)
	@#$(STRIP) $@

tSemaphoreMutexLock:  tSemaphoreMutexLock.o $(OBJS_GENERALAGENT)
	$(CPP) -o $@ $^ $(LDFLAGS)
	@#$(STRIP) $@
	
tSignal:  tSignal.o $(OBJS_GENERALAGENT)
	$(CPP) -o $@ $^ $(LDFLAGS)
	@#$(STRIP) $@
tObserver:tObserver.o $(OBJS_GENERALAGENT)
	$(CPP) -o $@ $^ $(LDFLAGS)

MyObserver:MyObserver.o $(OBJS_GENERALAGENT)
	$(CPP) -o $@ $^ $(LDFLAGS)

.c.o:
	$(CC) -c $(CFLAGS) $^ -o $@

.cpp.o:
	$(CPP) -c $(CFLAGS) $^ -o $@

clean:
	$(RM) $(OBJS_GENERALAGENT) $(TARGET) $(OBJS) *.o *.cfbk *.d;
