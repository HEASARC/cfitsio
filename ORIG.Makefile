#
# Makefile for cfitsio library:
#       libcfits.a
#
#       JDD
#       NASA GSFC
#       March, 1996
#

SHELL = /bin/sh

# define which compiler to use with setenv prior to running this
#  (e.g.,   setenv CC gcc )

all:	
	@echo "#ifdef __STDC__" > $$$$.ansi.c ; \
	echo "ANSI_Yes ;"     >> $$$$.ansi.c ; \
	echo "#else "         >> $$$$.ansi.c ; \
	echo "ANSI_No ;"      >> $$$$.ansi.c ; \
	echo "#endif"         >> $$$$.ansi.c ; \
	if test -n "${RANLIB}" ; \
	then \
	  echo "cfitsio1: Using ${RANLIB} for ranlib..."; \
	  RANLIB=${RANLIB}; \
	else \
	  old_IFS="$$IFS" ; IFS="$$IFS:" ; \
	  for trial in $$PATH ;\
	  do \
	    if test -f $$trial/ranlib; \
	    then \
		RANLIB="$$trial/ranlib";\
		echo "cfitsio2: Using $$RANLIB for ranlib" ;\
	    fi; \
	  done; \
	  IFS="$$old_IFS"; \
	  if test -z "$$RANLIB"; \
	  then \
	    echo "cfitsio: Using ar ts instead of ranlib..."; \
	    RANLIB="ar ts > /dev/null" ; \
	  fi; \
	fi ; \
	if ${CC} ${CFLAGS} -E $$$$.ansi.c | grep "ANSI_Yes" > /dev/null; \
	then \
	  echo "cfitsio: CC defined and ANSI. Using ${CC}. " ; \
	  rm $$$$.ansi.c; \
	  if test -d ../host ; \
	  then \
	    echo "cfitsio: installing library now... "; \
	    ${MAKE} install "CC = ${CC}" "CFLAGS = ${CFLAGS}" "RANLIB = $${RANLIB}"; \
	  else \
	    echo "cfitsio: making standalone library now... "; \
	    ${MAKE} stand_alone "CC = ${CC}" "CFLAGS = ${CFLAGS}" "RANLIB = $${RANLIB}"; \
	  fi ; \
	else \
	  echo "cfitsio: ${CC} compiler is not ANSI compliant." ; \
	  echo "cfitsio: Please define an ANSI compiler and try again."; \
	  echo "cfitsio: e.g., type: setenv CC gcc"; \
	  rm $$$$.ansi.c; \
	fi
 

stand_alone:	libcfitsio.a

libcfitsio.a:   libcfitsio.o
		ar rv libcfitsio.a cfileio.o convert.o fitscore.o getcol.o \
		getkey.o modkey.o putcol.o putkey.o utilproc.o
		$(RANLIB) libcfitsio.a

libcfitsio.o:   
		$(CC) -c $(CFLAGS) cfileio.c convert.c fitscore.c getcol.c \
		getkey.c modkey.c putcol.c putkey.c utilproc.c 

install:        libcfitsio.a
		mv libcfitsio.a ../host/.
        
clean:          
		rm -f cfileio.o fitscore.o convert.o getcol.o getkey.o \
		modkey.o putcol.o putkey.o utilproc.o
