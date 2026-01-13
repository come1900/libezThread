#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
#
# Makefile - \library\libezthread\Build\
#
# Copyright (C) 2011 ezlibs.com, All Rights Reserved.
#
# $Id: Makefile 1 2011-10-22 08:14:10 WuJunjie Exp $
#
# explain
#      exp
#
# Update:
#    2011-10-22 08:14 WuJunjie Create
#
#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
PUB_MAKE_FILE_PREFIX =	../Makefile.Defines
include		$(PUB_MAKE_FILE_PREFIX)/Makefile.Batch.Build

# define default platform
ifndef PLATFORM
PLATFORM = linux-x86-32
endif

VERSION="1.0.0"

# CPP, CFLAGS, LIBS
include		$(PUB_MAKE_FILE_PREFIX)/Makefile.Defines.$(PLATFORM)

#for LIB_OBJS
PREFIX_BASEDIR_SRC=./src
include $(PREFIX_BASEDIR_SRC)/Makefile.SrcLists

PREFIX =	$(HOME)/libs
INS_DIR_LIB = $(PREFIX)/lib
INS_DIR_INC = $(PREFIX)/include/ezThread

LIB_TARGET = libezThread-$(PLATFORM).a

TARGET	= $(LIB_TARGET)
all: $(TARGET)
	#cp $(TARGET) ./lib
	#cp ./src/*.h ./include

$(LIB_TARGET): $(LIB_OBJS)
	#mkdir -p $(LIBDIR)
	$(RM) $@;
	$(AR) $(AR_FLAG) $@ $^
	$(RANLIB) $@

install:	all 
		@mkdir -p $(INS_DIR_LIB)
		cp $(LIB_TARGET) $(INS_DIR_LIB)
		@mkdir -p $(INS_DIR_INC)
		cp ./src/*.h $(INS_DIR_INC)
.c.o:
	$(CC) -c $(CFLAGS) $^ -o $@

.cpp.o:
	$(CPP) -c $(CFLAGS) $^ -o $@

clean:
	$(RM) $(TARGET) $(LIB_OBJS) *.a *.cfbk ./lib/* ./include/*
	find . -name "*.d" |xargs rm -f
	find . -name "*.cfbk" |xargs rm -f
	make -C tutorial_libezthread clean
