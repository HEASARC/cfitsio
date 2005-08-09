PROJECT(cfitsio)
SET(LIBNAME cfitsio)
SET(SOURCES buffers.c cfileio.c checksum.c compress.c drvrfile.c drvrmem.c drvrnet.c drvrsmem.c editcol.c edithdu.c eval_l.c eval_y.c eval_f.c fitscore.c getcol.c getcolb.c getcold.c getcole.c getcoli.c getcolj.c getcolk.c getcoll.c getcols.c getcolsb.c getcoluk.c getcolui.c getcoluj.c getkey.c group.c grparser.c histo.c iraffits.c modkey.c putcol.c putcolb.c putcold.c putcole.c putcoli.c putcolj.c putcolk.c putcoluk.c putcoll.c putcols.c putcolsb.c putcolu.c putcolui.c putcoluj.c putkey.c region.c scalnull.c swapproc.c wcssub.c wcsutil.c imcompress.c quantize.c ricecomp.c pliocomp.c f77_wrap1.c f77_wrap2.c f77_wrap3.c f77_wrap4.c)
SET(HEADER_FILES fitsio.h fitsio2.h longnam.h drvrsmem.h)


INCLUDE (${CMAKE_SOURCE_DIR}/BUILD_DIR/BuildLib.cmake)
