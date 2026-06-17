#==============================================================================
# CFITSIO CMakeLists.txt
#==============================================================================

# Ensure that build directory is not the source directory:
IF(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
	message(FATAL_ERROR
		"Please create a separate directory for building CFITSIO."
		)
ENDIF()

CMAKE_MINIMUM_REQUIRED(VERSION 3.24)

# Suppress warning about WIN32 no longer being defined on Cygwin:
SET(CMAKE_LEGACY_CYGWIN_WIN32 0)

PROJECT(CFITSIO
	DESCRIPTION "Library of C and Fortran subroutines for reading and writing data files in FITS data format."
	LANGUAGES C CXX
	)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

# Define project version
SET(${PROJECT_NAME}_SONAME 10)
SET(${PROJECT_NAME}_MAJOR 4)
SET(${PROJECT_NAME}_MINOR 6)
SET(${PROJECT_NAME}_MICRO 4)
SET(${PROJECT_NAME}_VERSION ${${PROJECT_NAME}_MAJOR}.${${PROJECT_NAME}_MINOR}.${${PROJECT_NAME}_MICRO})
SET(LIB_NAME cfitsio)

INCLUDE(CheckSymbolExists)
INCLUDE(CheckSourceCompiles)
INCLUDE(GNUInstallDirs)
include(FetchContent)

#==============================================================================
# Build options:
#==============================================================================

# Allow the developer to select if Dynamic or Static libraries are built
OPTION(BUILD_SHARED_LIBS "Build Shared Libraries" ON)

# Allow curl dependency to be disabled with "-DUSE_CURL=OFF":
OPTION(USE_CURL "USE_CURL" ON)

OPTION(USE_PTHREADS "Thread-safe build (using pthreads)" OFF)

OPTION(TESTS "Build test programs Testprog and cookbook" ON)

OPTION(ITERPROGS "Build programs that test the CFITSIO iterator function" OFF)

OPTION(UTILS "Build helper programs [fpack, funpack, fitscopy, fitsverify, imcopy, smem, speed]" ON)

OPTION(USE_SSE2 "Enable use of instructions in the SSE2 extended instruction set" OFF)

OPTION(USE_SSSE3 "Enable use of instructions in the SSSE3 extended instruction set" OFF)

OPTION(USE_BZIP2 "Enable bzip2 support" OFF)

#==============================================================================

FetchContent_Declare(ZLIB
    URL https://www.zlib.net/zlib-1.3.2.tar.xz
    URL_HASH SHA256=d7a0654783a4da529d1bb793b7ad9c3318020af77667bcae35f95d0e42a792f3
    FIND_PACKAGE_ARGS
)

set(ZLIB_BUILD_TESTING OFF)

FetchContent_MakeAvailable(ZLIB)

IF(BUILD_SHARED_LIBS)
  SET (LIB_TYPE SHARED)
ELSE()
  SET (LIB_TYPE STATIC)
ENDIF(BUILD_SHARED_LIBS)

# Find curl library, for HTTPS support:
IF(USE_CURL)
   FIND_PACKAGE(CURL)
   IF(CURL_FOUND)
      add_compile_definitions(CFITSIO_HAVE_CURL)
   ENDIF()
ENDIF()

IF(USE_PTHREADS)
   FIND_PACKAGE(Threads REQUIRED)
   add_compile_definitions(_REENTRANT)
ENDIF(USE_PTHREADS)

IF(USE_BZIP2)
   FIND_PACKAGE(BZip2 REQUIRED)
   add_compile_definitions(HAVE_BZIP2=1)
ENDIF(USE_BZIP2)

# Math library (not available in MSVC or MINGW)
IF(MSVC OR MINGW)
   SET(M_LIB "")
ELSE()
   set(M_LIB m)
ENDIF()

# Support for remote file drivers is not implemented for native Windows:
IF (NOT MSVC)

    # Find library needed for gethostbyname:
    check_symbol_exists("gethostbyname" "netdb.h" CMAKE_HAVE_GETHOSTBYNAME)

    # Find library needed for connect:
    check_symbol_exists("connect" "sys/socket.h" CMAKE_HAVE_CONNECT)

    # Define HAVE_NET_SERVICES if gethostbyname & connect were found:
    IF (CMAKE_HAVE_GETHOSTBYNAME AND CMAKE_HAVE_CONNECT)
        add_compile_definitions(HAVE_NET_SERVICES)
        CHECK_SYMBOL_EXISTS(fmemopen "stdio.h" HAVE_FMEMOPEN)
        IF(HAVE_FMEMOPEN)
            add_compile_definitions(HAVE_FMEMOPEN)
        ENDIF()
    ENDIF()

ENDIF()

IF(MSVC OR BORLAND)
    # Define
    add_compile_definitions(_CRT_SECURE_NO_DEPRECATE)
    # Need an empty unistd.h in MSVC for flex-generated eval_l.c:
    file(TOUCH ${CFITSIO_BINARY_DIR}/unistd.h)
    INCLUDE_DIRECTORIES(${CFITSIO_BINARY_DIR})
ENDIF()

IF(BORLAND)
    # Suppress spurious Borland compiler warnings about "Suspicious
    # pointer arithmetic", "Possibly incorrect assignment", and
    # "Comparing signed and unsigned values".
    add_compile_options(-w-spa)
    add_compile_options(-w-pia)
    add_compile_options(-w-csu)
ENDIF()

IF(USE_SSE2)
    IF(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")
        add_compile_options(-msse2)
    ENDIF()
    IF(MSVC)
        add_compile_definitions(__SSE2__=1)
    ENDIF()
ENDIF()

IF(USE_SSSE3)
    IF(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")
        add_compile_options(-mssse3)
    ENDIF()
    IF(MSVC)
        add_compile_definitions(__SSE2__=1 __SSSE3__=1)
    ENDIF()
ENDIF()

# Test for the unix ftruncate function
CHECK_SYMBOL_EXISTS(ftruncate "unistd.h" HAVE_FTRUNCATE)
IF(HAVE_FTRUNCATE)
    add_compile_definitions(HAVE_UNISTD_H HAVE_FTRUNCATE)
ENDIF()

# Check is System V IPC is supported on this machine
check_source_compiles(C "
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
int main(void) {
  shmat(0,0,0);
  shmdt(0);
  shmget(0, 0, 0);
  semget(0, 0, 0);
  return 0;
}" HAVE_SHMEM_SERVICES)
IF(HAVE_SHMEM_SERVICES)
    add_compile_definitions(HAVE_SHMEM_SERVICES)

    # Some systems define flock_t, for others we have to define it ourselves
    check_source_compiles(C "
    #include <sys/file.h>
    int main(void) {
      flock_t filler;
      return 0;
    }" HAVE_FLOCK_T)
    IF(HAVE_FLOCK_T)
        add_compile_definitions(HAVE_FLOCK_T)
    ENDIF()

    # Check union semun
    check_source_compiles(C "
    #include <sys/ipc.h>
    #include <sys/shm.h>
    #include <sys/sem.h>
    int main(void) {
      union semun filler;
      return 0;
    }" HAVE_UNION_SEMUN)
    IF(HAVE_UNION_SEMUN)
        add_compile_definitions(HAVE_UNION_SEMUN)
    ENDIF()
ENDIF()

IF(WIN32 AND BUILD_SHARED_LIBS)
    add_compile_definitions(cfitsio_EXPORTS)
ENDIF()

#==============================================================================
# CFITSIO source code:
#==============================================================================

include(GenerateExportHeader)

SET(H_FILES fitsio.h fitsio2.h longnam.h)

SET(SRC_FILES
    buffers.c cfileio.c checksum.c
    drvrfile.c drvrmem.c drvrnet.c drvrsmem.c
    editcol.c edithdu.c eval_f.c eval_l.c eval_y.c
    f77_wrap1.c f77_wrap2.c f77_wrap3.c f77_wrap4.c
    fits_hcompress.c fits_hdecompress.c fitscore.c
    getcol.c getcolb.c getcold.c getcole.c getcoli.c getcolj.c getcolk.c
    getcoll.c getcols.c getcolsb.c getcolui.c getcoluj.c getcoluk.c
    getkey.c group.c grparser.c histo.c imcompress.c iraffits.c modkey.c
    pliocomp.c
    putcol.c putcolb.c putcold.c putcole.c putcoli.c putcolj.c putcolk.c
    putcoll.c putcols.c putcolsb.c putcolu.c putcolui.c putcoluj.c putcoluk.c
    putkey.c quantize.c region.c ricecomp.c scalnull.c simplerng.c swapproc.c
    wcssub.c wcsutil.c zcompress.c zuncompress.c
)

list(APPEND CMAKE_MODULE_PATH ${CFITSIO_SOURCE_DIR})

# For future modifications:

# drvrsmem.c is only usable if HAVE_SHMEM_SERVICES is defined:
#drvrsmem.c

# drvrgsiftp.c is only usable if HAVE_NET_SERVICES & HAVE_GSIFTP are defined:
#drvrgsiftp.c

#==============================================================================
# Add libraries:
#==============================================================================

ADD_LIBRARY(${LIB_NAME} ${LIB_TYPE} ${H_FILES} ${SRC_FILES})

# Math:
TARGET_LINK_LIBRARIES(${LIB_NAME} INTERFACE ${M_LIB})
# zlib:
if(TARGET ZLIB::ZLIB)
    TARGET_LINK_LIBRARIES(${LIB_NAME} PRIVATE ZLIB::ZLIB)
    # For cfitsio.pc.cmake:
    string(APPEND PKG_CONFIG_LIBS " -lz")
endif()
# cURL:
if(CURL_FOUND)
    TARGET_LINK_LIBRARIES(${LIB_NAME} PRIVATE CURL::libcurl)
    # For cfitsio.pc.cmake:
    string(APPEND PKG_CONFIG_LIBS " -lcurl")
endif()
# PThreads:
TARGET_LINK_LIBRARIES(${LIB_NAME} PRIVATE $<$<BOOL:${Threads_FOUND}>:Threads::Threads>)
# BZip2:
if(BZip2_FOUND)
    TARGET_LINK_LIBRARIES(${LIB_NAME} PRIVATE BZip2::BZip2)
    string(APPEND PKG_CONFIG_LIBS " -lbz2")
endif()

#==============================================================================

generate_export_header(cfitsio)
set_property(TARGET cfitsio PROPERTY VERSION ${CFITSIO_VERSION})
set_property(TARGET cfitsio PROPERTY SOVERSION 10)
set_property(TARGET cfitsio PROPERTY INTERFACE_cfitsio_MAJOR_VERSION ${${PROJECT_NAME}_MAJOR})
set_property(TARGET cfitsio APPEND PROPERTY
  COMPATIBLE_INTERFACE_STRING cfitsio_MAJOR_VERSION
)

install(TARGETS cfitsio EXPORT cfitsioTargets)

# If zlib is provided via FetchContent, include it in the same export set
# so cfitsio's export generation remains valid.
if(TARGET zlib)
  install(TARGETS zlib EXPORT cfitsioTargets)
endif()

install(FILES ${H_FILES} "${CFITSIO_BINARY_DIR}/cfitsio_export.h"
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT Devel)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CFITSIO_BINARY_DIR}/cfitsio/cfitsioConfigVersion.cmake"
  VERSION ${CFITSIO_VERSION}
  COMPATIBILITY AnyNewerVersion
)

export(EXPORT cfitsioTargets
  FILE "${CFITSIO_BINARY_DIR}/cfitsio/cfitsioTargets.cmake"
  NAMESPACE CFITSIO::
)
configure_file(cmake/cfitsioConfig.cmake
  "${CFITSIO_BINARY_DIR}/cfitsio/cfitsioConfig.cmake"
  COPYONLY
)

set(ConfigPackageLocation ${CMAKE_INSTALL_LIBDIR}/cmake/cfitsio)
install(EXPORT cfitsioTargets
  FILE
    cfitsioTargets.cmake
  NAMESPACE
    CFITSIO::
  DESTINATION
    ${ConfigPackageLocation}
)
install(
  FILES
    cmake/cfitsioConfig.cmake
    "${CFITSIO_BINARY_DIR}/cfitsio/cfitsioConfigVersion.cmake"
  DESTINATION
    ${ConfigPackageLocation}
  COMPONENT
    Devel
)

# cfitsio.pc:
configure_file(${CFITSIO_SOURCE_DIR}/cfitsio.pc.cmake ${CFITSIO_BINARY_DIR}/cfitsio.pc @ONLY)
install(FILES ${CFITSIO_BINARY_DIR}/cfitsio.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

#==============================================================================
# Test programs:
#==============================================================================
IF (TESTS)

    ENABLE_TESTING()

    ADD_EXECUTABLE(TestProg utilities/testprog.c)
    TARGET_LINK_LIBRARIES(TestProg PRIVATE ${LIB_NAME})
    add_test(NAME TestProg COMMAND TestProg)
    # Copy testprog.tpt to build directory to allow quick test
    # of ./TestProg (or .\Release\TestProg.exe in MSVC):
    configure_file(${CFITSIO_SOURCE_DIR}/testprog.tpt ${CFITSIO_BINARY_DIR}/testprog.tpt COPYONLY)

    ADD_EXECUTABLE(cookbook utilities/cookbook.c)
    TARGET_LINK_LIBRARIES(cookbook PRIVATE ${LIB_NAME})
    add_test(NAME cookbook COMMAND cookbook)

    if(BUILD_SHARED_LIBS AND WIN32)
      set(_cfitsio_dll_path "PATH=path_list_prepend:$<TARGET_FILE_DIR:${LIB_NAME}>")
      if(TARGET zlib)
        list(APPEND _cfitsio_dll_path "PATH=path_list_prepend:$<TARGET_FILE_DIR:zlib>")
      endif()
      set_tests_properties(TestProg cookbook PROPERTIES ENVIRONMENT_MODIFICATION "${_cfitsio_dll_path}")
    endif()

    install(TARGETS cookbook)

ENDIF(TESTS)

IF (ITERPROGS)

    ADD_EXECUTABLE(Iter_Image utilities/iter_image.c)
    TARGET_LINK_LIBRARIES(Iter_Image PRIVATE ${LIB_NAME})
    #add_test(NAME Iter_Image COMMAND Iter_Image)
    #install(TARGETS Iter_Image)
    configure_file(${CFITSIO_SOURCE_DIR}/iter_image.fit ${CFITSIO_BINARY_DIR}/iter_image.fit COPYONLY)

    ADD_EXECUTABLE(Iter_Var utilities/iter_var.c)
    TARGET_LINK_LIBRARIES(Iter_Var PRIVATE ${LIB_NAME})
    #add_test(NAME Iter_Var COMMAND Iter_Var)
    #install(TARGETS Iter_Var)
    configure_file(${CFITSIO_SOURCE_DIR}/vari.fits ${CFITSIO_BINARY_DIR}/vari.fits COPYONLY)

ENDIF(ITERPROGS)

#==============================================================================
# Utilities:
#==============================================================================
IF (UTILS)

    ADD_EXECUTABLE(fpack utilities/fpack.c utilities/fpackutil.c)
    TARGET_LINK_LIBRARIES(fpack PRIVATE ${LIB_NAME})

    ADD_EXECUTABLE(funpack utilities/funpack.c utilities/fpackutil.c)
    TARGET_LINK_LIBRARIES(funpack PRIVATE ${LIB_NAME})

    ADD_EXECUTABLE(fitscopy utilities/fitscopy.c)
    TARGET_LINK_LIBRARIES(fitscopy PRIVATE ${LIB_NAME})

    ADD_EXECUTABLE(fitsverify
                  utilities/ftverify.c utilities/fvrf_data.c
                  utilities/fvrf_file.c utilities/fvrf_head.c
                  utilities/fvrf_key.c utilities/fvrf_misc.c)
    TARGET_COMPILE_DEFINITIONS(fitsverify PRIVATE STANDALONE)
    TARGET_LINK_LIBRARIES(fitsverify PRIVATE ${LIB_NAME})

    ADD_EXECUTABLE(imcopy utilities/imcopy.c)
    TARGET_LINK_LIBRARIES(imcopy PRIVATE ${LIB_NAME})

    IF(HAVE_SHMEM_SERVICES)
      ADD_EXECUTABLE(smem utilities/smem.c)
      TARGET_LINK_LIBRARIES(smem PRIVATE ${LIB_NAME})
    ENDIF()

    ADD_EXECUTABLE(speed utilities/speed.c)
    TARGET_LINK_LIBRARIES(speed PRIVATE ${LIB_NAME})

    # To expand the command line arguments in Windows, see:
    # http://msdn.microsoft.com/en-us/library/8bch7bkk.aspx
    if(MSVC AND NOT CMAKE_SYSTEM_NAME MATCHES "WindowsStore")
      set_target_properties(fpack funpack PROPERTIES LINK_FLAGS "setargv.obj")
    endif()

    install(TARGETS fpack funpack fitscopy fitsverify imcopy speed)
    IF(HAVE_SHMEM_SERVICES)
      install(TARGETS smem)
    ENDIF()

ENDIF(UTILS)
