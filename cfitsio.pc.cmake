prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${prefix}/include

Name: cfitsio
Description: FITS File Subroutine Library
URL: https://heasarc.gsfc.nasa.gov/fitsio/
Version: @CFITSIO_MAJOR@.@CFITSIO_MINOR@.@CFITSIO_MICRO@
Libs: -L${libdir} -lcfitsio
Libs.private: @PKG_CONFIG_LIBS@ -lm
Cflags: -I${includedir}
