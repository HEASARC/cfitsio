$ ! Command file to build the CFITSIO library on a VAX VMS system
$ set verify
$ cc buffers.c
$ cc cfileio.c
$ cc checksum.c
$ cc fitscore.c
$ cc getcol.c
$ cc getcolb.c
$ cc getcoli.c
$ cc getcolj.c
$ cc getcolk.c
$ cc getcole.c
$ cc getcold.c
$ cc getcoll.c
$ cc getcols.c
$ cc editcol.c
$ cc edithdu.c
$ cc getkey.c
$ cc modkey.c
$ cc putcol.c
$ cc putcolb.c
$ cc putcoli.c
$ cc putcolj.c
$ cc putcolk.c
$ cc putcole.c
$ cc putcold.c
$ cc putcols.c
$ cc putcoll.c
$ cc putcolu.c
$ cc putkey.c
$ cc scalnull.c
$ cc swapproc.c
$cpu = f$edit(f$getsyi("arch_name"),"upcase")
$!
$IF cpu .eqs. "ALPHA"
$then 
$!
$  write sys$output "AlphaVMS version" 
$!
$ lib/create cfitsio buffers,cfileio,checksum,fitscore,getcol,getcolb
$ lib/insert cfitsio getcoli,getcolj,getcolk,getcole,getcold,getcoll
$ lib/insert cfitsio getcols,editcol,edithdu,getkey,modkey,putcol
$ lib/insert cfitsio putcolb,putcoli,putcolj,putcolk,putcole,putcold
$ lib/insert cfitsio putcols,putcoll,putcolu,putkey,scalnull,swapproc
$!
$else
$!
$! if not an alpha, assume vax and go on...
$  write sys$output "VAX VMS version"
$  macro vmsieeer.mar
$  macro vmsieeed.mar
$ lib/create cfitsio buffers,cfileio,checksum,fitscore,getcol,getcolb
$ lib/insert cfitsio getcoli,getcolj,getcolk,getcole,getcold,getcoll
$ lib/insert cfitsio getcols,editcol,edithdu,getkey,modkey,putcol
$ lib/insert cfitsio putcolb,putcoli,putcolj,putcolk,putcole,putcold
$ lib/insert cfitsio putcols,putcoll,putcolu,putkey,scalnull,swapproc
$ lib/insert cfitsio vmsieeer,vmsieeed
$!
$endif
$!
$ set noverify
