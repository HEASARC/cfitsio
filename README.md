# CFITSIO Interface Library

CFITSIO is a library of ANSI C routines for reading and writing FITS format data files. A set of Fortran-callable wrapper routines are also included for the convenience of Fortran programmers.  This README file gives a brief summary of how to build and test CFITSIO, but the latest and most complete information may be found in the "docs" folder in the CFITSIO User's Guide:


## User Guides

### User Guides for C programmers

* `cfitsio.tex` (LaTeX source file)
* `cfitsio.pdf` (Portable Document Format)

### User Guides for Fortran programmers

* `fitsio.tex` (LaTeX source file)
* `fitsio.pdf` (Portable Document Format)

### Quick Start Guide

* `quick.tex` (LaTeX source file)
* `quick.pdf` (Portable Document Format)


## Building and Installing CFITSIO

The standard way to build the CFITSIO library from source on Unix-like systems is:

```bash
./configure --prefix=/target/installation/path
make
make install
```

For complete installation and build instructions across all platforms, please see [INSTALL.md](INSTALL.md).

The INSTALL.md guide includes details for:

- Advanced Unix/Linux options (CMake, shared libraries)
- macOS (Xcode, MacPorts, Homebrew, Conda)
- Windows (Visual Studio, CMake)


## Testing CFITSIO

The CFITSIO library may be tested by building and running the `testprog.c` program that is included with the release (in the `utilities` folder). On Unix systems, type:

```bash
% make testprog
% ./testprog > testprog.lis
% diff testprog.lis testprog.out
% cmp testprog.fit testprog.std
```

The `testprog` program should produce a FITS file called `testprog.fit` that is identical to the `testprog.std` FITS file included in this release.  The diagnostic messages (which were piped to the file `testprog.lis` in the Unix example) should be identical to the listing contained in the file `testprog.out`. The `diff` and `cmp` commands shown above should not report any differences in the files.

## Using CFITSIO

The CFITSIO User's Guide, contained in the files mentioned above, provides detailed documentation about how to build and use the CFITSIO library. It contains a description of every user-callable routine in the CFITSIO interface.

The `cookbook.c` file in the utilities folder provides some sample routines for performing common operations on various types of FITS files. Programmers are urged to examine these routines for recommended programming practices when using CFITSIO. Users are free to copy or modify these routines for their own purposes.


## Getting Help

Any problem reports or suggestions for improvements are welcome and should be sent to the CFITSIO/CCFITS help desk at:

[ccfits@heasarc.gsfc.nasa.gov](mailto:ccfits@heasarc.gsfc.nasa.gov)

-------------------------------------------------------------------------
William D. Pence
HEASARC, NASA/GSFC
