#
# Makefile for cfitsio library:
#       libcfits.a
#
#       JDD
#       NASA GSFC
#       March, 1996
#

SHELL = /bin/sh

all : 
	@echo "cfitsio: Please run configure...."

install :
	@echo "cfitsio: Looks like configure failed. "
	@echo "cfitsio: Please run configure again...."
        
clean :
	@echo "cfitsio: nothing to clean up...."

.c.o :
	${CC} ${CFLAGS} -c $<
