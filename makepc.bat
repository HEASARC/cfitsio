rem:  this batch file builds the cfitsio library 
rem:  using the Borland C++ v4.5 compiler
rem:
bcc32 -c cfileio.c
bcc32 -c convert.c
bcc32 -c fitscore.c
bcc32 -c getcol.c
bcc32 -c getkey.c
bcc32 -c modkey.c
bcc32 -c putcol.c
bcc32 -c putkey.c
bcc32 -c utilproc.c
del cfitsio.lib
tlib cfitsio +cfileio +convert +fitscore +getcol +getkey
tlib cfitsio +modkey +putcol +putkey +utilproc
bcc32 -f testprog.c cfitsio.lib
bcc32 -f cookbook.c cfitsio.lib
