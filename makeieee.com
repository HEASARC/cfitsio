$ ! Command file to build the CFITSIO library on a VMS systems (VAX or Alpha)
$ set verify
$ cc/float=ieee_float buffers.c
$ cc/float=ieee_float cfileio.c
$ cc/float=ieee_float checksum.c
$ cc/float=ieee_float compress.c
$ cc/float=ieee_float fitscore.c
$ cc/float=ieee_float f77_iter.c
$ cc/float=ieee_float f77_wrap.c
$ cc/float=ieee_float getcol.c
$ cc/float=ieee_float getcolb.c
$ cc/float=ieee_float getcoli.c
$ cc/float=ieee_float getcolj.c
$ cc/float=ieee_float getcolui.c
$ cc/float=ieee_float getcoluj.c
$ cc/float=ieee_float getcolk.c
$ cc/float=ieee_float getcole.c
$ cc/float=ieee_float getcold.c
$ cc/float=ieee_float getcoll.c
$ cc/float=ieee_float getcols.c
$ cc/float=ieee_float editcol.c
$ cc/float=ieee_float edithdu.c
$ cc/float=ieee_float getkey.c
$ cc/float=ieee_float modkey.c
$ cc/float=ieee_float putcol.c
$ cc/float=ieee_float putcolb.c
$ cc/float=ieee_float putcoli.c
$ cc/float=ieee_float putcolj.c
$ cc/float=ieee_float putcolui.c
$ cc/float=ieee_float putcoluj.c
$ cc/float=ieee_float putcolk.c
$ cc/float=ieee_float putcole.c
$ cc/float=ieee_float putcold.c
$ cc/float=ieee_float putcols.c
$ cc/float=ieee_float putcoll.c
$ cc/float=ieee_float putcolu.c
$ cc/float=ieee_float putkey.c
$ cc/float=ieee_float scalnull.c
$ cc/float=ieee_float swapproc.c
$ cc/float=ieee_float wcsutil.c
$ lib/create cfitsio buffers,cfileio,checksum,compress,f77_iter,f77_wrap,fitscore
$ lib/insert cfitsio getcol,getcolb,getcoli,getcolj,getcolk,getcole,getcold
$ lib/insert cfitsio getcoll,getcols,editcol,edithdu,getkey,modkey,putcol
$ lib/insert cfitsio putcolb,putcoli,putcolj,putcolk,putcole,putcold
$ lib/insert cfitsio getcolui,getcoluj,putcolui,putcoluj
$ lib/insert cfitsio putcols,putcoll,putcolu,putkey,scalnull,swapproc,wcsutil
$ ! 
$ if (F$GETSYI("ARCH_NAME") .eqs. "VAX") then goto VAX
$   set noverify
$   exit
$ !
$ VAX:
$ ! add macro routines not needed on Alpha and only used on VAX
$   macro vmsieeer.mar
$   macro vmsieeed.mar
$   lib/insert cfitsio vmsieeer,vmsieeed
$   set noverify
$   exit
