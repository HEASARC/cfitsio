$ ! Command file to build the CFITSIO library on a VMS systems (VAX or Alpha)
$ set verify
$ cc/float=d_float buffers.c
$ cc/float=d_float cfileio.c
$ cc/float=d_float checksum.c
$ cc/float=d_float compress.c
$ cc/float=d_float f77_wrap1.c
$ cc/float=d_float f77_wrap2.c
$ cc/float=d_float fitscore.c
$ cc/float=d_float getcol.c
$ cc/float=d_float getcolb.c
$ cc/float=d_float getcoli.c
$ cc/float=d_float getcolj.c
$ cc/float=d_float getcolui.c
$ cc/float=d_float getcoluj.c
$ cc/float=d_float getcolk.c
$ cc/float=d_float getcole.c
$ cc/float=d_float getcold.c
$ cc/float=d_float getcoll.c
$ cc/float=d_float getcols.c
$ cc/float=d_float editcol.c
$ cc/float=d_float edithdu.c
$ cc/float=d_float getkey.c
$ cc/float=d_float modkey.c
$ cc/float=d_float putcol.c
$ cc/float=d_float putcolb.c
$ cc/float=d_float putcoli.c
$ cc/float=d_float putcolj.c
$ cc/float=d_float putcolui.c
$ cc/float=d_float putcoluj.c
$ cc/float=d_float putcolk.c
$ cc/float=d_float putcole.c
$ cc/float=d_float putcold.c
$ cc/float=d_float putcols.c
$ cc/float=d_float putcoll.c
$ cc/float=d_float putcolu.c
$ cc/float=d_float putkey.c
$ cc/float=d_float scalnull.c
$ cc/float=d_float swapproc.c
$ cc/float=d_float wcsutil.c
$ lib/create cfitsio buffers,cfileio,checksum,compress,f77_wrap1,f77_wrap2,fitscore
$ lib/insert cfitsio getcol,getcolb,getcoli,getcolj,getcolk,getcole,getcold
$ lib/insert cfitsio getcoll,getcols,editcol,edithdu,getkey,modkey,putcol
$ lib/insert cfitsio putcolb,putcoli,putcolj,putcolk,putcole,putcold
$ lib/insert cfitsio getcolui,getcoluj,putcolui,putcoluj
$ lib/insert cfitsio putcols,putcoll,putcolu,putkey,scalnull,swapproc,wcsutil
$ ! 
$ if (F$GETSYI("ARCH_NAME") .eqs. "VAX") then goto VAX
$ ! add C routine needed on Alpha to do D_FLOAT conversions
$   cc/float=d_float vmsieee.c
$   lib/insert cfitsio vmsieee
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
