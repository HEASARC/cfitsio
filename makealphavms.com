$ ! Command file to build the CFITSIO library on an Alpha VMS system
$ set verify
$ cc cfileio.c
$ cc convert.c
$ cc fitscore.c
$ cc getcol.c
$ cc getkey.c
$ cc modkey.c
$ cc putcol.c
$ cc putkey.c
$ cc utilproc.c
$ lib/create cfitsio cfileio,convert,fitscore,getcol,getkey,modkey
$ lib/insert cfitsio putcol,putkey,utilproc
$ set noverify

