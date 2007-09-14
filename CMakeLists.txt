PROJECT(cfitsio)
SET(LIBNAME cfitsio)
SET(SOURCES buffers.c cfileio.c checksum.c compress.c drvrfile.c drvrgsiftp.c drvrmem.c drvrnet.c editcol.c edithdu.c eval_f.c eval_l.c eval_y.c f77_wrap1.c f77_wrap2.c f77_wrap3.c f77_wrap4.c fits_hcompress.c fits_hdecompress.c fitscore.c getcol.c getcolb.c getcold.c getcole.c getcoli.c getcolj.c getcolk.c getcoll.c getcols.c getcolsb.c getcolui.c getcoluj.c getcoluk.c getkey.c group.c grparser.c histo.c imcompress.c iraffits.c iter_var.c modkey.c pliocomp.c putcol.c putcolb.c putcold.c putcole.c putcoli.c putcolj.c putcolk.c putcoll.c putcols.c putcolsb.c putcolu.c putcolui.c putcoluj.c putcoluk.c putkey.c quantize.c region.c ricecomp.c scalnull.c swapproc.c wcssub.c wcsutil.c)
SET(HEADER_FILES fitsio.h fitsio2.h longnam.h drvrsmem.h)


INCLUDE (${CMAKE_SOURCE_DIR}/BUILD_DIR/BuildLib.cmake)
