## Process this file with automake to produce Makefile.in
#
# Author: B.Irby (NASA/GSFC), based on the previous CFITSIO Makefile.in
#

lib_LTLIBRARIES = libcfitsio.la

libcfitsio_ladir = $(pkgincludedir)
libcfitsio_la_HEADERS =	fitsio.h fitsio2.h longnam.h drvrsmem.h \
	cfortran.h f77_wrap.h region.h

if !NOFORTRAN
F77_WRAPPERS = f77_wrap1.c f77_wrap2.c f77_wrap3.c f77_wrap4.c
else
F77_WRAPPERS = 
endif

if !NOGSIFTP
GSIFTP_SRC = drvrgsiftp.c
else
GSIFTP_SRC =
endif

LIBS = @LIBS@
LIBS_CURL = @LIBS_CURL@

libcfitsio_la_SOURCES =	\
	buffers.c \
	cfileio.c \
	checksum.c \
	drvrfile.c \
	drvrmem.c \
	drvrnet.c \
	drvrsmem.c \
	editcol.c \
	edithdu.c \
	eval_l.c \
	eval_y.c \
	eval_f.c \
	fitscore.c \
	getcol.c \
	getcolb.c \
	getcold.c \
	getcole.c \
	getcoli.c \
	getcolj.c \
	getcolk.c \
	getcoll.c \
	getcols.c \
	getcolsb.c \
	getcoluk.c \
	getcolui.c \
	getcoluj.c \
	getkey.c \
	group.c \
	grparser.c \
	histo.c \
	iraffits.c \
	modkey.c \
	putcol.c \
	putcolb.c \
	putcold.c \
	putcole.c \
	putcoli.c \
	putcolj.c \
	putcolk.c \
	putcoluk.c \
	putcoll.c \
	putcols.c \
	putcolsb.c \
	putcolu.c \
	putcolui.c \
	putcoluj.c \
	putkey.c \
	region.c \
	scalnull.c \
	swapproc.c \
	wcssub.c \
	wcsutil.c \
	imcompress.c \
	quantize.c \
	ricecomp.c \
	pliocomp.c \
	fits_hcompress.c \
	fits_hdecompress.c \
	simplerng.c \
	zcompress.c \
	zuncompress.c \
	${F77_WRAPPERS} ${GSIFTP_SRC}

libcfitsio_la_CFLAGS = $(AM_CFLAGS) @DEFS@
libcfitsio_swapproc_la_CFLAGS = $(libcfitsio_la_CFLAGS) @SSE_FLAGS@
libcfitsio_la_LIBADD = -lm ${LIBS_CURL} ${LIBS}
libcfitsio_la_LDFLAGS = -version-info @version_info@

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = cfitsio.pc

# UTILITIES: =============================================================

#SUBDIRS = utilities

AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = \
	cookbook \
	fitscopy \
	fitsverify \
	fpack \
	funpack \
	imcopy \
	smem \
	speed

#if !NOFORTRAN
#noinst_PROGRAMS = testprog testf77
#else
noinst_PROGRAMS = testprog
#endif

cookbook_SOURCES = utilities/cookbook.c

fitscopy_SOURCES = utilities/fitscopy.c

fitsverify_CFLAGS = -DSTANDALONE
fitsverify_SOURCES = utilities/ftverify.c utilities/fvrf_data.c \
		     utilities/fvrf_file.c utilities/fvrf_head.c \
		     utilities/fvrf_key.c utilities/fvrf_misc.c

fpack_SOURCES = utilities/fpack.c utilities/fpackutil.c

funpack_SOURCES = utilities/funpack.c utilities/fpackutil.c

imcopy_SOURCES = utilities/imcopy.c

smem_SOURCES = utilities/smem.c

speed_SOURCES = utilities/speed.c

testprog_SOURCES = utilities/testprog.c

#testf77_SOURCES = utilities/testf77.f

LDADD = libcfitsio.la ${LIBS}

# eval: ==================================================================

FLEX =		flex
BISON =		bison

# Tested with bison 3.8 and flex 2.6.4
eval:		# Rebuild eval_* files from flex/bison source
		${FLEX} -o eval_l.c eval.l 
		${BISON} -o eval_y.c --header=eval_tab.h eval.y

# Distribution: ==========================================================

EXTRA_DIST = docs licenses \
	README.md README.MacOS README.win \
	cfitsio.xcodeproj \
	CMakeLists.txt \
	eval.l eval.y \
	iter_a.f iter_a.fit iter_b.f iter_b.fit iter_c.f iter_c.fit \
	sample.tpl \
	testf77.out testf77.std \
	testprog.out testprog.std testprog.tpt
# ?:
# cfitsio-config.cmake.in
# cfitsio-config-version.cmake.in
# cfitsio.pc.cmake
# cfitsio.pc.in
# windumpexts.c
# winDumpExts.mak

# clean: =================================================================

CLEANFILES = testprog.fit testf77.fit y.output

# HEASoft packages fitsTcl and POW need a list of all .o files: ==========

#cfitsioLibObjs:
#	@echo ${CORE_OBJECTS}
