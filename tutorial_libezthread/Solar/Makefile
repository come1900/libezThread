# Copyright (C) 2012  ezlibs.com
#
# platforms:
PLATFORM = linux
#PLATFORM = linux-x86-32
#PLATFORM = linux-hi3515
#PLATFORM = linux-arm
#PLATFORM = linux-x86-64-centos5
#PLATFORM = linux-am335x

PROJECT_NAME=exaSolar
DEF_VERSION =	1.0.0
DIFF_VERSION =	0.0.0
DEF_CONFIG_FILENAME_MAIN = cfg-$(PROJECT_NAME).cfg
DEF_CONFIG_FILENAME_MAIN_DUPLICATE = cfg-$(PROJECT_NAME).cfg.duplicate
DEF_LOG_FILENAME = log-$(PROJECT_NAME).log

include         Makefile.Defines/Makefile.Defines.$(PLATFORM)

#CFLAGS =	-Wall -g -O2 -MD
#CFLAGS += -D_DEBUG #debugme


CFLAGS += -D_CONFIG_FILENAME_MAIN='"$(DEF_CONFIG_FILENAME_MAIN)"'
CFLAGS += -D_CONFIG_FILENAME_MAIN_DUPLICATE='"$(DEF_CONFIG_FILENAME_MAIN_DUPLICATE)"'
CFLAGS += -D_VERSION='"$(DEF_VERSION)"'
CFLAGS += -D_LOG_FILENAME='"$(DEF_LOG_FILENAME)"'

CFLAGS += -Wno-write-strings

##############################################################################
# 代码文件位置
SOLAR_BASEDIR_SRC=./

##############################################################################
#库文件位置
#使用自带库
#EZLIBS_BASEDIR_LIBS=../libs

##使用发布库
EZLIBS_BASEDIR_LIBS=$(HOME)/libs

OBJS_SOLAR = \
		$(SOLAR_BASEDIR_SRC)/Solar.o \
		$(SOLAR_BASEDIR_SRC)/SimpleThread.o \
		$(SOLAR_BASEDIR_SRC)/LocalConsole.o \
		$(SOLAR_BASEDIR_SRC)/Main.o


#use ezutil library for http_parse
#CFLAGS += -I${EZLIBS_BASEDIR_LIBS}/include/ezutil
#LIBS +=	-L${EZLIBS_BASEDIR_LIBS}/lib -lezutil-$(PLATFORM)
#ezUtilCPP
#CFLAGS += -I$(EZLIBS_BASEDIR_LIBS)/include/ezUtilCPP/
#LIBS +=	-L$(EZLIBS_BASEDIR_LIBS)/lib -lezUtilCPP-$(PLATFORM)

CFLAGS += -I${EZLIBS_BASEDIR_LIBS}/include/ezThread
LIBS +=	-L${EZLIBS_BASEDIR_LIBS}/lib -lezThread-$(PLATFORM)
LIBS +=	-lpthread

#多线程，用ezthread库
OBJS_MULTITHREAD_P += $(OBJS_SOLAR)
DIST_MULTITHREAD = $(PROJECT_NAME)-$(PLATFORM)

DIST =	$(DIST_MULTITHREAD)

all: $(DIST)


$(DIST_MULTITHREAD):$(OBJS_MULTITHREAD_P)
		$(CPP) -o $@ $^ $(LIBS)

tar:	clean
		tar -czf $(PROJECT_NAME).tgz *.h *.cpp Makefile*

-include *.d

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

.cpp.o:
	$(CPP) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(PROJECT_NAME)* #$(DIST)
	find $(SOLAR_BASEDIR_SRC) -name "*.o" |xargs rm -f
	find $(SOLAR_BASEDIR_SRC) -name "*.d" |xargs rm -f
	find $(SOLAR_BASEDIR_SRC) -name "*.cfbk" |xargs rm -f

