# CFITSIO Installation Guide


## Install on Unix and Linux

The CFITSIO code (contained in `*.c` source files and several `*.h` header files) should compile and run on most Unix platforms without modification. The standard way to build the library on Unix systems is the usual GNU-like approach, i.e. by first typing

```bash
% ./configure  [--prefix=/target/installation/path]
```

at the operating system prompt.  Type `./configure` and not simply `configure` to ensure that the configure script in the current directory is run and not some other system-wide configure script. The optional `prefix` argument to configure gives the path to the directory where the CFITSIO library and include files should be installed via the later `make install` command. For example,

```bash
% ./configure --prefix=/usr1/local
```

will cause the later `make install` command to copy the library file(s) to `/usr1/local/lib` and the necessary header files to `/usr1/local/include` (assuming of course that the process has permission to write to these directories).

All the available configure options can be seen by entering the command

```bash
% ./configure --help
```

The configure command customizes the Makefile for a particular system, so after it has been run, type

```bash
% make
```

at the prompt, and this will compile the source files and build the library (static `libcfitsio.a` as well as the shared version `libcfitsio.so|.dylib`) and the helper utilities (`fpack`, `funpack`, `fitscopy`, `imcopy`, et al.) and test program (`testprog`).  To copy the library, header files, and utilities to the chosen install location, type this command:

```bash
% make check
% make install
```

When installing in /usr/local on Linux and some other systems, it may be necessary to rebuild the linker cache by running:

```bash
% sudo ldconfig
```

Alternatively, the library and utilities may be built on many systems using the CMake program.  Specific instructions for using CMake on Windows platforms can be found in the [Install on Windows](#install-on-windows) section below, but for Unix systems (e.g., Linux or macOS) the procedure should be similar to the following:

While in the CFITSIO source code directory:

```bash
% mkdir cmbuild
% cd cmbuild
% cmake -G "Unix Makefiles" ..
% cmake --build .
% cmake --install . [--prefix /usr/local]
```

Where the final step uses an optional installation prefix.


## Install on macOS

By default, the CFITSIO library will be a "Universal Binary" (i.e.
32- and 64-bit compatible) under Mac OS X when built in the standard
way, i.e.

```bash
tar xzf cfitsio3370.tar.gz # or whichever version is being built
cd cfitsio/
./configure
make
make install
```

### Install CFITSIO using MacPorts

If you have MacPorts installed, you may install CFITSIO simply with
the command

```bash
  $ sudo port install cfitsio +universal
```

For more information, please visit:

http://macports.org
https://trac.macports.org/browser/trunk/dports/science/cfitsio/Portfile

### Install CFITSIO using Homebrew

If you have Homebrew installed, you may install CFITSIO simply with
the command

```bash
brew install cfitsio
```

For more information, please visit:

http://brew.sh
http://brewformulas.org/Cfitsio

### Install CFITSIO using conda

If you have conda installed, you may install CFITSIO simply with
the command

```bash
conda install cfitsio
```

For more information, please visit:

https://github.com/conda-forge/cfitsio-feedstock

### Build CFITSIO using the XCode GUI:

```bash
tar xzf cfitsio3370.tar.gz # or whichever version is being built
cd cfitsio/
```

Start Xcode and open `cfitsio.xcodeproj/project.pbxproj`, or just "open" the file from a terminal command line,

```bash
open cfitsio.xcodeproj/project.pbxproj
```

and this will start up XCode for you.

Press the Build (or "Play") button in the upper left corner of the GUI.


## Install on Windows

Instructions on building and using CFITSIO on Windows platforms
for C programmers using Microsoft Visual Studio or Borland C++.

These instructions for building the CFITSIO library under Windows use
the CMake build system that is available from http://www.cmake.org.


### 1. Build the CFITSIO dll library

  a. If CMAKE is not already installed on your machine, download it
     from  http://www.cmake.org.  It is recommended that you choose the
     "Add CMake to the system PATH for current user" option during the
     installation setup process for convenience when running CMake later on.

  b. Install ZLIB: The compression library "zlib" is required in
     order to build CFITSIO.  Visit their site at

       https://zlib.net

     to download the latest distribution.  Unpack it, then build and
     install it from a parallel directory, for example:

       mkdir zlib.build
       cd zlib.build
       cmake ..\zlib-1.2.11 -DCMAKE_INSTALL_PREFIX=c:\Users\myname\zlib
       cmake --build . --config Release
       cmake --install .

     The cmake comands below will use the path "c:\Users\myname\zlib"
     as an example for the installed zlib location.

  c. Unzip the CFITSIO .zip file (e.g. cfit-4.0.0.zip) that was obtained from
     the CFITSIO Web site (http://heasarc.gsfc.nasa.gov/fitsio). This will
     create a new \cfitsio-4.0.0 subdirectory that contains the source code
     and documentation files.  It also contains a CMakeLists.txt file
     that will be used during the CMake build process.

  d. Open the Visual Studio Command Prompt window, likely using a desktop
     icon with this same name, (or the equivalent Borland command window)
     and cd (change directory) into the parent directory that is one level
     above the directory containing the CFITSIO source files that was
     created in the previous step.

  e. Create a new subdirectory (for example, "cfitsio.build"), and cd
     into it with the following commands:

     mkdir cfitsio.build
     cd cfitsio.build

     When using Visual Studio, all the files that are generated during
     the CMake process will be created in or under this subdirectory.

  f. Create the CMake files necessary for the build:

     To use the default Visual Studio compilers and architecture,
     simply run (for example):

     cmake ..\cfitsio-4.0.0 -DCMAKE_PREFIX_PATH=c:\Users\myname\zlib

     Alternatively, you can specify the name of a CMake Generator
     using the "-G" flag:

     cmake -G "<cmake generator>" ..\cfitsio-4.0.0 -DCMAKE_PREFIX_PATH=c:\Users\myname\zlib

     Where the string "<cmake generator>" designates your choice
     of compiler. Some possible options are:

     "Visual Studio 16 2019"
     "Visual Studio 16 2019" -A Win32    (for 32-bit builds)
     "Visual Studio 15 2017 Win64"
     "Visual Studio 15 2017"             (for 32-bit builds)
     "Visual Studio 15 2017" -A Win32    (for 32-bit builds)
     "MinGW Makefiles"
     "Borland Makefiles"
     "NMake Makefiles"

     You can see a list of all the available CMake Generators by
     executing the command

        cmake /?

     Note that these names are case-sensitive and must be entered
     exactly as displayed.

     Note that the "..\cfitsio-4.0.0" argument gives the relative
     path to the directory that contains the CFITSIO source files
     and the CMakeLists.txt file.

     Some other optional flags are available to use during this
     step, using this format:

     cmake -G <generator> ..\cfitsio-4.0.0 -D<option>=<ON|OFF>" -DCMAKE_PREFIX_PATH=c:\Users\myname\zlib

     Where the available flags are:

     -DUSE_CURL=OFF          (default is ON, but unavailable in MS Visual Studio)
       Disable search for the curl library needed for remote file access

     -DBUILD_SHARED_LIBS=OFF (default is ON)
       Disable building a shared CFITSIO library (required in order
       link the curl library into the cfitsio library; otherwise
       you will need to link curl into your application)

     -DUSE_PTHREADS=ON       (default is OFF)
       Enable a thread-safe build (using pthreads)

     -DUSE_BZIP2=ON          (default is OFF)
       Enable bzip2 support

     -DCFITSIO_USE_SSE2=ON   (default is OFF)
       Enable use of instructions in the SSE2 extended instruction set

     -DCFITSIO_USE_SSSE3=ON  (default is OFF)
       Enable use of instructions in the SSSE3 extended instruction set

     -DTESTS=OFF             (default is ON)
       Build test programs (Testprog and cookbook)

     -DUTILS=OFF             (default is ON)
       Build helper programs [fpack, funpack, fitscopy, fitsverify,
       imcopy, smem, speed]

  g. Execute the following command to build the CFITSIO library:

     cmake --build . --config Release

     The "." argument following "--build" here tells CMake to build
     the files in the current directory (i.e., in "cfitsio.build").

     If this process completes successfully, you should find the
     CFITSIO library files in the "cfitsio.build\Release" subdirectory.

     To test whether CFITSIO is working correctly, execute 'Testprog'
     in the Release directory.  It should generate a long stream of
     diagnostic messages ending with this line:

         "Status = 0: OK - no error".

     Similarly, running the cookbook program should end with a
     message of success.

  h. Other CMake options:

     Refer to the CMake documentation for information about many
     other build options.

     For example, one can build a 'debug' version of the CFITSIO
     library by executing the command "cmake --build ." instead of
     the command listed above in section f.

     The library file(s), header files, and utility programs can
     also be installed in a default location (which is currently
     C:/Program Files (x86)/CFITSIO/) by running

     cmake --install .

     This may require administrative privileges, so an alternate
     install location may be used by specifying a prefix flag in
     the first CMake command in step f), for example:

     cmake -G "<generator>" ..\cfitsio-4.0.0
      -DCMAKE_INSTALL_PREFIX=c:\Users\myname\cfitsio
      -DCMAKE_PREFIX_PATH=c:\Users\myname\zlib


     One can also make a thread safe version of CFITSIO using the
     pthread library with the following procedure:

     a. Download the precompiled files from the pthread-win32 project
     (http://sourceware.org/pthreads-win32/). Put the files for your
     specific platform (.h, .lib, .dll) into a folder 'pthread',
     parallel to the cfitsio source folder.

     b. For the compilation of cfitsio follow the cmake steps, but
     use this as a replacement for the commands in step f:

     cmake -G "<cmake generator>" ..\cfitsio-4.0.0 -DUSE_PTHREADS=1
      -DCMAKE_INCLUDE_PATH=..\pthread -DCMAKE_LIBRARY_PATH=..\pthread
      -DCMAKE_PREFIX_PATH=c:\Users\myname\zlib

     You may need to adapt the paths for the directories used as
     examples here.


### 2. Using CFITSIO when compiling and linking application programs

First, depending on your particular programming environment, it may be
necessary to copy the `cfitsio.lib` and `cfitsio.dll` files into another
directory where your compiler expects to find them.  Or equivalently, you
may need to specify the directory path to the location of the CFITSIO
library files when creating a project that uses them.  You may also need to
copy the `fitsio.h` and `longnam.h` include files from the `\cfitsio` source file
directory to a standard 'include' directory on your system.

When using the Visual Studio command line window, application programs can
be compiled and linked with CFITSIO using the following command:

```bash
		cl /MD your_program.c cfitsio.lib
```

The `/MD` command line switch must be specified to force the compiler/linker
to use the appropriate runtime library. If this switch is omitted, then
the `fits_report_error` function in CFITSIO will likely crash.

When building programs in the Visual Studio graphical environment, one can
force the equivalent of the `/MD` switch by selecting 'Settings...' under the
'Project' menu, then click on the C/C++ tab and select the 'Code Generator'
category. Then under 'User Run-time Library' select 'Multithreaded DLL'.







