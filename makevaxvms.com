$ ! Command file to build the CFITSIO library on a VAX VMS system
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
$ macro vmsieeer.mar
$ macro vmsieeed.mar
$ lib/create cfitsio cfileio,convert,fitscore,getcol,getkey,modkey
$ lib/insert cfitsio putcol,putkey,utilproc,vmsieeer,vmsieeed
$ set noverify

