# Build CFITSIO on Windows using Microsoft Visual Studio.

These instructions for building the CFITSIO library under Windows use
the CMake build system that is available from http://www.cmake.org.

If CMake is not already installed on your machine, download it
from  https://www.cmake.org.

Unzip the CFITSIO .zip file (e.g. cfit-4.0.0.zip) that was obtained from
the CFITSIO Web site (https://heasarc.gsfc.nasa.gov/fitsio). This will
create a new ./cfitsio-4.0.0 subdirectory that contains the source code
and documentation files.  It also contains a CMakeLists.txt file
that will be used during the CMake build process.

Open the Visual Studio Command Prompt window, likely using a desktop
icon with this same name. `cd` (change directory) into the ./cfitsio-4.0.0
directory.

## Configure CMake build

To use the default Visual Studio compilers and architecture,
simply run (for example):

```sh
cmake -B build
```

Alternatively, specify the name of a CMake Generator
using the "-G" flag:

```sh
cmake -G "<cmake generator>" -B build
```

Where the string "<cmake generator>" designates your choice
of compiler. Some possible options are:

```
"Visual Studio 18 2026"
"Visual Studio 17 2022"
"Visual Studio 16 2019"
"Ninja"
"MinGW Makefiles"
"NMake Makefiles"
```

The option `-A Win32` enables 32-bit builds if the appropriate Visual
Studio toolkit is installed.

Get a list of all the available CMake Generators by
executing the command

```pwsh
cmake /?
```

Note that these names are case-sensitive and must be entered
exactly as displayed.

The optional `-S "../cfitsio-4.0.0"` argument gives the relative
path to the directory that contains the CFITSIO source files
and the CMakeLists.txt file.

Some other optional flags are available to use during this
step, using this format:

```pwsh
cmake -G <generator> -S "../cfitsio-4.0.0" -B build -D<option>=<ON|OFF>"
```

Where the available flags are:

`-DUSE_CURL=OFF`
: Disable search for the curl library needed for remote file access (default is ON)

`-DBUILD_SHARED_LIBS=OFF`
: Disable building a shared CFITSIO library (required in order
  link the curl library into the cfitsio library; otherwise
  you will need to link curl into your application) (default is ON)

`-DUSE_PTHREADS=ON`
: Enable a thread-safe build (using pthreads) (default is OFF)

`-DUSE_BZIP2=ON`
: Enable bzip2 support (default is OFF)

`-DCFITSIO_USE_SSE2=ON`
: Enable use of instructions in the SSE2 extended instruction set (default is OFF)

`-DCFITSIO_USE_SSSE3=ON`
: Enable use of instructions in the SSSE3 extended instruction set (default is OFF)

`-DTESTS=OFF`
: Build test programs (Testprog and cookbook) (default is ON)

`-DUTILS=OFF`
: Build helper programs [fpack, funpack, fitscopy, fitsverify,
  imcopy, smem, speed] (default is ON)

## Build the CFITSIO library

After the CMake configuration step is complete:

```sh
cmake --build build --config Release
```

The "build" argument following "--build" here tells CMake to build
the files in the specified directory (i.e., in "./build").

If this process completes successfully, the CFITSIO library files
will be located in the "build/Release" subdirectory.

To test whether CFITSIO is working correctly, execute:

```sh
ctest --test-dir build -C Release -R TestProg -V
```

It should generate a long stream of diagnostic messages ending with this line:

```
"Status = 0: OK - no error".
```

Similarly, running the cookbook program should end with a message of success.

```sh
ctest --test-dir build -C Release -R cookbook -V
```

### Other CMake options

Refer to the CMake documentation for information about many
other build options.

For example, one can build a 'debug' version of the CFITSIO
library by executing the command

```sh
cmake --build build --config Debug
```

The library file(s), header files, and utility programs can
also be installed in the location specified by the CMAKE_INSTALL_PREFIX variable by executing the command:

```sh
cmake --install build
```

This may require administrative privileges, so an alternate
install location may be used by specifying a prefix flag in
the first CMake command in step f), for example:

```sh
cmake -B build --install-prefix C:/cfitsio
```

### CFITSIO with pthreads-win32 library

Download the precompiled binary files from the
[pthread-win32 project](http://sourceware.org/pthreads-win32/).
Put the files for your specific platform (.h, .lib, .dll) into a folder "../pthread",
parallel to the cfitsio source folder.

Compile cfitsio like:

```sh
cmake -B build -DUSE_PTHREADS=1 -DThreads_ROOT="../pthread"

cmake --build build --config Release
```

You may need to adapt the paths for the directories used as
examples here.

## Link CFITSIO into user application programs

First, depending on your particular programming environment, it may be
necessary to copy the cfitsio.lib and cfitsio.dll files into another
directory where your compiler expects to find them.  Or equivalently, you
may need to specify the directory path to the location of the CFITSIO
library files when creating a project that uses them.  You may also need to
copy the fitsio.h and longnam.h include files from the ./cfitsio source file
directory to a standard 'include' directory on your system.

When using the Visual Studio command line window, application programs can
be compiled and linked with CFITSIO using the following command:

		cl /MD your_program.c cfitsio.lib

The /MD command line switch must be specified to force the compiler/linker
to use the appropriate runtime library.   If this switch is omitted, then
the fits_report_error function in CFITSIO will likely crash.

When building programs in the Visual Studio graphical environment, one can
force the equivalent of the /MD switch by selecting 'Settings...' under the
'Project' menu, then click on the C/C++ tab and select the 'Code Generator'
category.  Then under 'User Run-time Library' select 'Multithreaded DLL'.
