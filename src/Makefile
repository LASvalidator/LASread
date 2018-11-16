# makefile for open source (LGPL) liblasread.a
#
#COPTS    = -g -Wall
COPTS     = -O3 -Wall -Wno-deprecated -DNDEBUG -DUNORDERED -DLASZIP_ENABLED
COMPILER  = g++
AR  = ar rc
#BITS     = -64

INCLUDE  = -I/usr/include/ -I../inc -I.

OBJ = lasheader.o laspoint.o lasreader.o lasreadopener.o laswaveformreader.o lasutility.o laszip.o lasreadpoint.o lasreaditemcompressed_v1.o lasreaditemcompressed_v2.o lasreaditemcompressed_v3.o lasreaditemcompressed_v4.o integerdecompressor.o arithmeticdecoder.o arithmeticmodel.o

all: liblasread.a

liblasread.a: ${OBJ}
	$(AR) $@ ${OBJ}
	cp -p $@ ../lib

.cpp.o:
	${COMPILER} ${BITS} -c ${COPTS} ${INCLUDE} $< -o $@

.c.o:
	${COMPILER} ${BITS} -c ${COPTS} ${INCLUDE} $< -o $@

clean:
	rm -rf liblasread.a
	rm -rf *.o

clobber:
	rm -rf liblasread.a
	rm -rf *.o
	rm -rf *~
