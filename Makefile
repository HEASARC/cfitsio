#
# Makefile for cfitsio library:
#       libcfits.a
#
#       JDD
#       NASA GSFC
#       March, 1996
#

SHELL = /bin/sh
RANLIB=ranlib
CC=gcc
CFLAGS=-g

SOURCES = buffers.c   getcol.c    getcoll.c   putcol.c    putcoll.c  \
	cfileio.c   getcolb.c   getcols.c   putcolb.c   putcols.c   swapproc.c \
	checksum.c  getcold.c   getcolui.c  putcold.c   putcolu.c  \
	getcole.c   getcoluj.c  putcole.c   putcolui.c  wcsutil.c \
	editcol.c   getcoli.c   getkey.c    putcoli.c   putcoluj.c \
	edithdu.c   getcolj.c   putcolj.c   putkey.c \
	fitscore.c  getcolk.c   modkey.c    putcolk.c   scalnull.c

OBJECTS = buffers.o   getcol.o    getcoll.o   putcol.o    putcoll.o  \
	cfileio.o   getcolb.o   getcols.o   putcolb.o   putcols.o   swapproc.o \
	checksum.o  getcold.o   getcolui.o  putcold.o   putcolu.o  \
	getcole.o   getcoluj.o  putcole.o   putcolui.o  wcsutil.o \
	editcol.o   getcoli.o   getkey.o    putcoli.o   putcoluj.o \
	edithdu.o   getcolj.o   putcolj.o   putkey.o \
	fitscore.o  getcolk.o   modkey.o    putcolk.o   scalnull.o

all:		stand_alone clean

stand_alone:	libcfitsio.a

libcfitsio.a: $(OBJECTS)
		ar rv libcfitsio.a ${OBJECTS}; \
		${RANLIB} libcfitsio.a; 

install:	libcfitsio.a
		mv libcfitsio.a ${FTOOLS_LIB}
		cp *.h ${FTOOLS_INCLUDE}/

clean:
		rm -f *.o 

distclean:	clean
		rm -f Makefile config.*

.c.o:
		${CC} ${CFLAGS} -c $*.c
