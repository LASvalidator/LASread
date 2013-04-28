# makefile for open source (LGPL) liblaslibrary.a
#
#COPTS    = -g -Wall
COPTS     = -O3 -Wall -Wno-deprecated -DNDEBUG -DUNORDERED
COMPILER  = g++
AR  = ar rc
#BITS     = -64

INCLUDE  = -I/usr/include/ -I../inc -I.

OBJ_LAS = lasheader.o laspoint.o lasreader.o lasreadopener.o laswaveformreader.o lasutility.o laszip.o lasreadpoint.o lasreaditemcompressed_v1.o lasreaditemcompressed_v2.o integerdecompressor.o arithmeticdecoder.o arithmeticmodel.o

all: liblaslibrary.a

liblaslibrary.a: ${OBJ_LAS}
	$(AR) $@ ${OBJ_LAS}
	cp -p $@ ../lib

.cpp.o:
	${COMPILER} ${BITS} -c ${COPTS} ${INCLUDE} $< -o $@

.c.o: 
	${COMPILER} ${BITS} -c ${COPTS} ${INCLUDE} $< -o $@

clean:
	rm -rf liblaslibrary.a
	rm -rf *.o

clobber:
	rm -rf liblaslibrary.a
	rm -rf *.o
	rm -rf *~
