#ifndef _FITSIO2_H
#define _FITSIO2_H
 
#include "fitsio.h"
 
#define DBUFFSIZE 28800 /* size of data buffer in bytes */

#define NIOBUF 25        /* number of IO buffers to create */
#define IOBUFLEN 2880    /* size in bytes of each IO buffer */
#define MINDIRECT 8640   /* minimum size for direct reads and writes */
                         /* MINDIRECT must have a value >= 8640 */
#define IO_SEEK 0        /* last file I/O operation was a seek */
#define IO_READ 1        /* last file I/O operation was a read */
#define IO_WRITE 2       /* last file I/O operation was a write */

#define MEMBUFF 101      /* code to signify user-allocated memory buffer    */
                         /* rather then disk file is used for the file I/O  */

#define TMPMEMBUFF 102   /* code to signify temporary memory buffer         */
                         /* rather then disk file is used for the file I/O. */
                         /* CFITSIO must free this buffer when the file     */
                         /* is closed.                                      */


#define NATIVE             0 /* a generic machine that uses the same IEEE formats as FITS */
#define ULTRIX             1
#define ALPHA_OSF          2
#define VAXVMS             3
#define ALPHAVMS           4
#define IBMPC              5
#define CRAY               6

#define GFLOAT             1
#define IEEEFLOAT          2

/* the following are used to determine what type machine we are running on */

/* the following block determines the size of longs on SGI IRIX machines */
#if defined(_MIPS_SZLONG)
#  if _MIPS_SZLONG == 32
#    define LONGSIZE 32
#  elif _MIPS_SZLONG == 64
#    define LONGSIZE 64
#  else
#    error "can't handle long size given by _MIPS_SZLONG"
#  endif
#else
#  define LONGSIZE 32
#endif

#if defined(vax) && defined(VMS)
 
#define MACHINE VAXVMS
#define BYTESWAPPED TRUE
 
#elif defined(__alpha) && defined(__VMS) &&  (__G_FLOAT == TRUE)
 
#define MACHINE ALPHAVMS
#define BYTESWAPPED TRUE
#define FLOATTYPE GFLOAT
 
#elif defined(__alpha) && defined(__VMS) &&  (__IEEE_FLOAT == TRUE)
 
#define MACHINE ALPHAVMS
#define BYTESWAPPED TRUE
#define FLOATTYPE IEEEFLOAT
 
#elif defined(__alpha) && defined(__unix__)
 
#define MACHINE ALPHA_OSF
#define BYTESWAPPED TRUE
 
#elif defined(ultrix) && defined(unix)
 
#define MACHINE ULTRIX
#define BYTESWAPPED TRUE
 
#elif defined(__i386) || defined(__i386__) || defined(__i486__) || defined(__i586__) 

/*  IBM PC */
#define MACHINE IBMPC
#define BYTESWAPPED TRUE

#elif defined(_MSC_VER) || defined(__BORLANDC__) || defined(__TURBOC__)

/*  IBM PC running DOS or Windows */
#define MACHINE IBMPC       
#define BYTESWAPPED TRUE

#elif defined(_NI_mswin_)

/*  LabWindows/CVI with Windows 3.x, 95, or NT  */
#define MACHINE IBMPC       
#define BYTESWAPPED TRUE

#elif  defined(__EMX__)

/*  IBM PC running OS/2 */
#define MACHINE IBMPC
#define BYTESWAPPED TRUE

#else

/*  assume machine uses the same IEEE formats as used in FITS files */
#define MACHINE NATIVE
#define BYTESWAPPED FALSE
 
#endif
  
#define IGNORE_EOF 1
#define REPORT_EOF 0
#define DATA_UNDEFINED -1
#define NULL_UNDEFINED 1234554321
#define ASCII_NULL_UNDEFINED 1   /* indicate no defined null value */
 
#define maxvalue(A,B) ((A) > (B) ? (A) : (B))
#define minvalue(A,B) ((A) < (B) ? (A) : (B))
 
#if MACHINE == ALPHAVMS
 
#define FNANMASK   0x7F00 /* mask bits 1 - 7; all set on NaNs */
                                     /* all 0 on underflow  or 0. */
 
#define DNANMASK   0x7FE0 /* mask bits 1 - 10; all set on NaNs */
                                     /* all 0 on underflow  or 0. */
 
#else
 
#define FNANMASK   0x7F80 /* mask bits 1 - 8; all set on NaNs */
                                     /* all 0 on underflow  or 0. */
 
#define DNANMASK   0x7FF0 /* mask bits 1 - 11; all set on NaNs */
                                     /* all 0 on underflow  or 0. */
 
#endif
 
#if MACHINE == CRAY
    /*
      Cray machines:   the large negative integer corresponds
      to the 3 most sig digits set to 1.   If these
      3 bits are set in a floating point number (64 bits), then it represents
      a reserved value (i.e., a NaN)
    */
#define fnan(L) ( (L) >= 0xE000000000000000 ? 1 : 0) )
 
#else
    /* these functions work for both big and little endian machines */
    /* that use the IEEE floating point format for internal numbers */
 
   /* These functions tests whether the float value is a reserved IEEE     */
   /* value such as a Not-a-Number (NaN), or underflow, overflow, or       */
   /* infinity.   The functions returns 1 if the value is a NaN, overflow  */
   /* or infinity; it returns 2 if the value is an denormalized underflow  */
   /* value; otherwise it returns 0. fnan tests floats, dnan tests doubles */
 
#define fnan(L) \
      ( (L & FNANMASK) == FNANMASK ?  1 : (L & FNANMASK) == 0 ? 2 : 0)
 
#define dnan(L) \
      ( (L & DNANMASK) == DNANMASK ?  1 : (L & DNANMASK) == 0 ? 2 : 0)
 
#endif
 
#define DUCHAR_MAX  255.49 /* max double value that fits in an unsigned char */
#define DUCHAR_MIN -0.49   /* min double value that fits in an unsigned char */
#define DUSHRT_MAX  65535.49 /* max double value that fits in a unsigned short*/
#define DUSHRT_MIN -0.49   /* min double value that fits in an unsigned short */
#define DSHRT_MAX  32767.49 /* max double value that fits in a short */
#define DSHRT_MIN -32768.49 /* min double value that fits in a short */
#define DULONG_MAX 4294967295.49 /* max double that fits in a unsigned long */
#define DULONG_MIN -0.49   /* min double value that fits in an unsigned long */
#define DLONG_MAX  2147483647.49 /* max double value that fits in a long */
#define DLONG_MIN -2147483648.49 /* min double value that fits in a long */
 
void ffmkky(char *keyname, char *keyval, char *comm, char *card);
int ffgnky(fitsfile *fptr, char *card, int *status);
void ffcfmt(char *tform, char *cform);
void ffswap2(short *values, long nvalues);
void ffswaplong(long *values, long nvalues);
void ffunswaplong(long *values, long nvalues);
void ffpacklong(long *values, long nvalues);
void ffunpacklong(long *values, long nvalues);
void ffswapfloat(float *values, long nvalues);
void ffswap8(double *values, long nvalues);
int ffi2c(long ival, char *cval, int *status);
int ffl2c(int lval, char *cval, int *status);
int ffs2c(char *instr, char *outstr, int *status);
int ffr2f(float fval, int decim, char *cval, int *status);
int ffr2e(float fval, int decim, char *cval, int *status);
int ffd2f(double dval, int decim, char *cval, int *status);
int ffd2e(double dval, int decim, char *cval, int *status);
int ffc2ii(char *cval, long *ival, int *status);
int ffc2ll(char *cval, int *lval, int *status);
int ffc2rr(char *cval, float *fval, int *status);
int ffc2dd(char *cval, double *dval, int *status);
int ffc2x(char *cval, char *dtype, long *ival, int *lval, char *sval,
          double *dval, int *status);
int ffc2s(char *instr, char *outstr, int *status);
int ffc2i(char *cval, long *ival, int *status);
int ffc2r(char *cval, float *fval, int *status);
int ffc2d(char *cval, double *dval, int *status);
int ffc2l(char *cval, int *lval, int *status);
void ffxmsg(int action, char *err_message);
int ffgcnt(fitsfile *fptr, char *value, int *status);
int ffgtkn(fitsfile *fptr, int numkey, char *keyname, long *value, int *status);
int fftkyn(fitsfile *fptr, int numkey, char *keyname, char *value, int *status);
int ffgphd(fitsfile *fptr, int maxdim, int *simple, int *bitpix, int *naxis,
        long naxes[], long *pcount, long *gcount, int *extend, double *bscale,
          double *bzero, long *blank, int *nspace, int *status);
int ffgttb(fitsfile *fptr, long *rowlen, long *nrows, long *pcount,
          long *tfield, int *status);
 
int ffmkey(fitsfile *fptr, char *card, int *status);
int ffikey(fitsfile *fptr, char *card, int *status);
 
int ffmbyt(fitsfile *fptr, long bytpos, int ignore_err, int *status);
int ffgbyt(fitsfile *fptr, long nbytes, void *buffer, int *status);
int ffpbyt(fitsfile *fptr, long nbytes, void *buffer, int *status);
int ffgbytoff(fitsfile *fptr, long gsize, long ngroups, long offset, 
           void *buffer, int *status);
int ffpbytoff(fitsfile *fptr, long gsize, long ngroups, long offset,
           void *buffer, int *status);
int ffldrc(fitsfile *fptr, long record, int err_mode, int *status);
int ffwhbf(fitsfile *fptr, int *nbuff);
int ffflsh(fitsfile *fptr, int clearbuf, int *status);
int ffbfwt(int nbuff, int *status);

int ffrhdu(fitsfile *fptr, int *hdutype, int *status);
int ffpinit(fitsfile *fptr, int *status);
int ffainit(fitsfile *fptr, int *status);
int ffbinit(fitsfile *fptr, int *status);
int ffchdu(fitsfile *fptr, int *status);
int ffwend(fitsfile *fptr, int *status);
int ffpdfl(fitsfile *fptr, int *status);
int ffuptf(fitsfile *fptr, int *status);

int ffdblk(fitsfile *fptr, long nblocks, int *status);
int ffgext(fitsfile *fptr, int moveto, int *exttype, int *status);
int ffgtbc(fitsfile *fptr, long *totalwidth, int *status);
int ffgtbp(fitsfile *fptr, char *name, char *value, int *status);
int ffiblk(fitsfile *fptr, long nblock, int headdata, int *status);
int ffshft(fitsfile *fptr, long firstbyte, long nbytes, long nshift,
    int *status);
 
int ffgcpr(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, int writemode, double *scale, double *zero, char *tform,
           long *twidth, int *tcode, int *maxelem, long *startpos,
           long *elemnum, long *incre, long *repeat,long *rowlen,
           int *hdutype, long *tnull, char *snull, int *status);
 
int ffopenfile(char *filename, int newfile, int readwrite,
           FILE **diskfile, size_t *filesize, int *status);
int ffisfilecompressed(char *filename, FILE **diskfile, size_t *filesize,
     size_t *finalsize,int *status);
int ffclosex(fitsfile *fptr, int keep, int *status);
int ffclosefile(char *filename, FILE *diskfile, int keep,  int *status);
int ffstdin2mem(void **buffptr, size_t *buffsize,
    void *(*mem_realloc)(void *p, size_t newsize), size_t *filesize,
    int *status);
int fffile2mem(char *filename, void **buffptr, size_t *buffsize,
    void *(*mem_realloc)(void *p, size_t newsize), size_t *filesize,
    int *status);
int ffuncompress2mem(char *filename, FILE *diskfile,  void **buffptr,
    size_t *buffsize, void *(*mem_realloc)(void *p, size_t newsize),
    size_t *filesize, int *status);
int ffcreate_mem(fitsfile *fptr, size_t init_size, int *status);
int ffflushx(fitsfile *fptr);
int ffseek(fitsfile *fptr, long position);
int ffread(fitsfile *fptr, long nbytes, void *buffer,
            int *status);
int ffwrite(fitsfile *fptr, long nbytes, void *buffer,
            int *status);
 
int ffgcls(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, int nultyp, char *nulval,
           char **array, char *nularray, int *anynul, int  *status);
int ffgclb(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, long  elemincre, int nultyp, unsigned char nulval,
           unsigned char *array, char *nularray, int *anynul, int  *status);
int ffgclui(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, long  elemincre, int nultyp, unsigned short nulval,
           unsigned short *array, char *nularray, int *anynul, int  *status);
int ffgcli(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, long  elemincre, int nultyp, short nulval,
           short *array, char *nularray, int *anynul, int  *status);
int ffgcluj(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, long elemincre, int nultyp, unsigned long nulval,
           unsigned long *array, char *nularray, int *anynul, int  *status);
int ffgclj(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, long elemincre, int nultyp, long nulval, long *array,
           char *nularray, int *anynul, int  *status);
int ffgclk(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, long elemincre, int nultyp, int nulval, int *array,
           char *nularray, int *anynul, int  *status);
int ffgcle(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, long elemincre, int nultyp,  float nulval, float *array,
           char *nularray, int *anynul, int  *status);
int ffgcld(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, long elemincre, int nultyp, double nulval,
           double *array, char *nularray, int *anynul, int  *status);
 
int ffpi1b(fitsfile *fptr, long nelem, long incre, unsigned char *buffer,
           int *status);
int ffpi2b(fitsfile *fptr, long nelem, long incre, short *buffer, int *status);
int ffpi4b(fitsfile *fptr, long nelem, long incre, long *buffer, int *status);
int ffpr4b(fitsfile *fptr, long nelem, long incre, float *buffer, int *status);
int ffpr8b(fitsfile *fptr, long nelem, long incre, double *buffer, int *status);
 
int ffgi1b(fitsfile *fptr, long pos, long nelem, long incre,
          unsigned char *buffer, int *status);
int ffgi2b(fitsfile *fptr, long pos, long nelem, long incre, short *buffer,
          int *status);
int ffgi4b(fitsfile *fptr, long pos, long nelem, long incre, long *buffer,
          int *status);
int ffgr4b(fitsfile *fptr, long pos, long nelem, long incre, float *buffer,
          int *status);
int ffgr8b(fitsfile *fptr, long pos, long nelem, long incre, double *buffer,
          int *status);
 
int ffcins(fitsfile *fptr, long naxis1, long naxis2, long nbytes,
           long bytepos, int *status);
int ffcdel(fitsfile *fptr, long naxis1, long naxis2, long nbytes,
           long bytepos, int *status);
int ffkshf(fitsfile *fptr, int firstcol, int tfields, int nshift, int *status);
 
int fffi1i1(unsigned char *input, long ntodo, double scale, double zero,
            int nullcheck, unsigned char tnull, unsigned char nullval, char
             *nullarray, int *anynull, unsigned char *output, int *status);
int fffi2i1(short *input, long ntodo, double scale, double zero,
            int nullcheck, short tnull, unsigned char nullval, char *nullarray,
            int *anynull, unsigned char *output, int *status);
int fffi4i1(long *input, long ntodo, double scale, double zero,
            int nullcheck, long tnull, unsigned char nullval, char *nullarray,
            int *anynull, unsigned char *output, int *status);
int fffr4i1(float *input, long ntodo, double scale, double zero,
            int nullcheck, unsigned char nullval, char *nullarray,
            int *anynull, unsigned char *output, int *status);
int fffr8i1(double *input, long ntodo, double scale, double zero,
            int nullcheck, unsigned char nullval, char *nullarray,
            int *anynull, unsigned char *output, int *status);
int fffstri1(char *input, long ntodo, double scale, double zero,
            long twidth, double power, int nullcheck, char *snull,
            unsigned char nullval, char *nullarray, int *anynull,
            unsigned char *output, int *status);
 
int fffi1u2(unsigned char *input, long ntodo, double scale, double zero,
            int nullcheck, unsigned char tnull, unsigned short nullval, 
            char *nullarray,
            int *anynull, unsigned short *output, int *status);
int fffi2u2(short *input, long ntodo, double scale, double zero,
            int nullcheck, short tnull, unsigned short nullval, char *nullarray,
            int *anynull, unsigned short *output, int *status);
int fffi4u2(long *input, long ntodo, double scale, double zero,
            int nullcheck, long tnull, unsigned short nullval, char *nullarray,
            int *anynull, unsigned short *output, int *status);
int fffr4u2(float *input, long ntodo, double scale, double zero,
            int nullcheck, unsigned short nullval, char *nullarray,
            int *anynull, unsigned short *output, int *status);
int fffr8u2(double *input, long ntodo, double scale, double zero,
            int nullcheck, unsigned short nullval, char *nullarray,
            int *anynull, unsigned short *output, int *status);
int fffstru2(char *input, long ntodo, double scale, double zero,
            long twidth, double power, int nullcheck, char *snull,
            unsigned short nullval, char *nullarray, int  *anynull, 
            unsigned short *output, int *status);

int fffi1i2(unsigned char *input, long ntodo, double scale, double zero,
            int nullcheck, unsigned char tnull, short nullval, char *nullarray,
            int *anynull, short *output, int *status);
int fffi2i2(short *input, long ntodo, double scale, double zero,
            int nullcheck, short tnull, short nullval, char *nullarray,
            int *anynull, short *output, int *status);
int fffi4i2(long *input, long ntodo, double scale, double zero,
            int nullcheck, long tnull, short nullval, char *nullarray,
            int *anynull, short *output, int *status);
int fffr4i2(float *input, long ntodo, double scale, double zero,
            int nullcheck, short nullval, char *nullarray,
            int *anynull, short *output, int *status);
int fffr8i2(double *input, long ntodo, double scale, double zero,
            int nullcheck, short nullval, char *nullarray,
            int *anynull, short *output, int *status);
int fffstri2(char *input, long ntodo, double scale, double zero,
            long twidth, double power, int nullcheck, char *snull,
            short nullval, char *nullarray, int  *anynull, short *output,
            int *status);

int fffi1u4(unsigned char *input, long ntodo, double scale, double zero,
            int nullcheck, unsigned char tnull, unsigned long nullval,
            char *nullarray,
            int *anynull, unsigned long *output, int *status);
int fffi2u4(short *input, long ntodo, double scale, double zero,
            int nullcheck, short tnull, unsigned long nullval, char *nullarray,
            int *anynull, unsigned long *output, int *status);
int fffi4u4(long *input, long ntodo, double scale, double zero,
            int nullcheck, long tnull, unsigned long nullval, char *nullarray,
            int *anynull, unsigned long *output, int *status);
int fffr4u4(float *input, long ntodo, double scale, double zero,
            int nullcheck, unsigned long nullval, char *nullarray,
            int *anynull, unsigned long *output, int *status);
int fffr8u4(double *input, long ntodo, double scale, double zero,
            int nullcheck, unsigned long nullval, char *nullarray,
            int *anynull, unsigned long *output, int *status);
int fffstru4(char *input, long ntodo, double scale, double zero,
            long twidth, double power, int nullcheck, char *snull,
            unsigned long nullval, char *nullarray, int *anynull,
            unsigned long *output, int *status);
 
int fffi1i4(unsigned char *input, long ntodo, double scale, double zero,
            int nullcheck, unsigned char tnull, long nullval, char *nullarray,
            int *anynull, long *output, int *status);
int fffi2i4(short *input, long ntodo, double scale, double zero,
            int nullcheck, short tnull, long nullval, char *nullarray,
            int *anynull, long *output, int *status);
int fffi4i4(long *input, long ntodo, double scale, double zero,
            int nullcheck, long tnull, long nullval, char *nullarray,
            int *anynull, long *output, int *status);
int fffr4i4(float *input, long ntodo, double scale, double zero,
            int nullcheck, long nullval, char *nullarray,
            int *anynull, long *output, int *status);
int fffr8i4(double *input, long ntodo, double scale, double zero,
            int nullcheck, long nullval, char *nullarray,
            int *anynull, long *output, int *status);
int fffstri4(char *input, long ntodo, double scale, double zero,
            long twidth, double power, int nullcheck, char *snull,
            long nullval, char *nullarray, int *anynull, long *output,
            int *status);
 
int fffi1int(unsigned char *input, long ntodo, double scale, double zero,
            int nullcheck, unsigned char tnull, int nullval, char *nullarray,
            int *anynull, int *output, int *status);
int fffi2int(short *input, long ntodo, double scale, double zero,
            int nullcheck, short tnull, int nullval, char *nullarray,
            int *anynull, int *output, int *status);
int fffi4int(long *input, long ntodo, double scale, double zero,
            int nullcheck, long tnull, int nullval, char *nullarray,
            int *anynull, int *output, int *status);
int fffr4int(float *input, long ntodo, double scale, double zero,
            int nullcheck, int nullval, char *nullarray,
            int *anynull, int *output, int *status);
int fffr8int(double *input, long ntodo, double scale, double zero,
            int nullcheck, int nullval, char *nullarray,
            int *anynull, int *output, int *status);
int fffstrint(char *input, long ntodo, double scale, double zero,
            long twidth, double power, int nullcheck, char *snull,
            int nullval, char *nullarray, int *anynull, int *output,
            int *status);
 
int fffi1r4(unsigned char *input, long ntodo, double scale, double zero,
            int nullcheck, unsigned char tnull, float nullval, char *nullarray,
            int *anynull, float *output, int *status);
int fffi2r4(short *input, long ntodo, double scale, double zero,
            int nullcheck, short tnull, float nullval, char *nullarray,
            int *anynull, float *output, int *status);
int fffi4r4(long *input, long ntodo, double scale, double zero,
            int nullcheck, long tnull, float nullval, char *nullarray,
            int *anynull, float *output, int *status);
int fffr4r4(float *input, long ntodo, double scale, double zero,
            int nullcheck, float nullval, char *nullarray,
            int *anynull, float *output, int *status);
int fffr8r4(double *input, long ntodo, double scale, double zero,
            int nullcheck, float nullval, char *nullarray,
            int *anynull, float *output, int *status);
int fffstrr4(char *input, long ntodo, double scale, double zero,
            long twidth, double power, int nullcheck, char *snull,
            float nullval, char *nullarray, int *anynull, float *output,
            int *status);
 
int fffi1r8(unsigned char *input, long ntodo, double scale, double zero,
            int nullcheck, unsigned char tnull, double nullval, char *nullarray,
            int *anynull, double *output, int *status);
int fffi2r8(short *input, long ntodo, double scale, double zero,
            int nullcheck, short tnull, double nullval, char *nullarray,
            int *anynull, double *output, int *status);
int fffi4r8(long *input, long ntodo, double scale, double zero,
            int nullcheck, long tnull, double nullval, char *nullarray,
            int *anynull, double *output, int *status);
int fffr4r8(float *input, long ntodo, double scale, double zero,
            int nullcheck, double nullval, char *nullarray,
            int *anynull, double *output, int *status);
int fffr8r8(double *input, long ntodo, double scale, double zero,
            int nullcheck, double nullval, char *nullarray,
            int *anynull, double *output, int *status);
int fffstrr8(char *input, long ntodo, double scale, double zero,
            long twidth, double power, int nullcheck, char *snull,
            double nullval, char *nullarray, int *anynull, double *output,
            int *status);
 
 
int ffi1fi1(unsigned char *array, long ntodo, double scale, double zero,
            unsigned char *buffer, int *status);
int ffu2fi1(unsigned short *array, long ntodo, double scale, double zero,
            unsigned char *buffer, int *status);
int ffi2fi1(short *array, long ntodo, double scale, double zero,
            unsigned char *buffer, int *status);
int ffu4fi1(unsigned long *array, long ntodo, double scale, double zero,
            unsigned char *buffer, int *status);
int ffi4fi1(long *array, long ntodo, double scale, double zero,
            unsigned char *buffer, int *status);
int ffintfi1(int *array, long ntodo, double scale, double zero,
            unsigned char *buffer, int *status);
int ffr4fi1(float *array, long ntodo, double scale, double zero,
            unsigned char *buffer, int *status);
int ffr8fi1(double *array, long ntodo, double scale, double zero,
            unsigned char *buffer, int *status);
 
int ffi1fi2(unsigned char *array, long ntodo, double scale, double zero,
            short *buffer, int *status);
int ffu2fi2(unsigned short *array, long ntodo, double scale, double zero,
            short *buffer, int *status);
int ffi2fi2(short *array, long ntodo, double scale, double zero,
            short *buffer, int *status);
int ffu4fi2(unsigned long *array, long ntodo, double scale, double zero,
            short *buffer, int *status);
int ffi4fi2(long *array, long ntodo, double scale, double zero,
            short *buffer, int *status);
int ffintfi2(int *array, long ntodo, double scale, double zero,
            short *buffer, int *status);
int ffr4fi2(float *array, long ntodo, double scale, double zero,
            short *buffer, int *status);
int ffr8fi2(double *array, long ntodo, double scale, double zero,
            short *buffer, int *status);
 
int ffi1fi4(unsigned char *array, long ntodo, double scale, double zero,
            long *buffer, int *status);
int ffu2fi4(unsigned short *array, long ntodo, double scale, double zero,
            long *buffer, int *status);
int ffi2fi4(short *array, long ntodo, double scale, double zero,
            long *buffer, int *status);
int ffu4fi4(unsigned long *array, long ntodo, double scale, double zero,
            long *buffer, int *status);
int ffi4fi4(long *array, long ntodo, double scale, double zero,
            long *buffer, int *status);
int ffintfi4(int *array, long ntodo, double scale, double zero,
            long *buffer, int *status);
int ffr4fi4(float *array, long ntodo, double scale, double zero,
            long *buffer, int *status);
int ffr8fi4(double *array, long ntodo, double scale, double zero,
            long *buffer, int *status);
 
int ffi1fr4(unsigned char *array, long ntodo, double scale, double zero,
            float *buffer, int *status);
int ffu2fr4(unsigned short *array, long ntodo, double scale, double zero,
            float *buffer, int *status);
int ffi2fr4(short *array, long ntodo, double scale, double zero,
            float *buffer, int *status);
int ffu4fr4(unsigned long *array, long ntodo, double scale, double zero,
            float *buffer, int *status);
int ffi4fr4(long *array, long ntodo, double scale, double zero,
            float *buffer, int *status);
int ffintfr4(int *array, long ntodo, double scale, double zero,
            float *buffer, int *status);
int ffr4fr4(float *array, long ntodo, double scale, double zero,
            float *buffer, int *status);
int ffr8fr4(double *array, long ntodo, double scale, double zero,
            float *buffer, int *status);
 
int ffi1fr8(unsigned char *array, long ntodo, double scale, double zero,
            double *buffer, int *status);
int ffu2fr8(unsigned short *array, long ntodo, double scale, double zero,
            double *buffer, int *status);
int ffi2fr8(short *array, long ntodo, double scale, double zero,
            double *buffer, int *status);
int ffu4fr8(unsigned long *array, long ntodo, double scale, double zero,
            double *buffer, int *status);
int ffi4fr8(long *array, long ntodo, double scale, double zero,
            double *buffer, int *status);
int ffintfr8(int *array, long ntodo, double scale, double zero,
            double *buffer, int *status);
int ffr4fr8(float *array, long ntodo, double scale, double zero,
            double *buffer, int *status);
int ffr8fr8(double *array, long ntodo, double scale, double zero,
            double *buffer, int *status);

int ffi1fstr(unsigned char *input, long ntodo, double scale, double zero,
            char *cform, long twidth, char *output, int *status);
int ffu2fstr(unsigned short *input, long ntodo, double scale, double zero,
            char *cform, long twidth, char *output, int *status);
int ffi2fstr(short *input, long ntodo, double scale, double zero,
            char *cform, long twidth, char *output, int *status);
int ffu4fstr(unsigned long *input, long ntodo, double scale, double zero,
            char *cform, long twidth, char *output, int *status);
int ffi4fstr(long *input, long ntodo, double scale, double zero,
            char *cform, long twidth, char *output, int *status);
int ffintfstr(int *input, long ntodo, double scale, double zero,
            char *cform, long twidth, char *output, int *status);
int ffr4fstr(float *input, long ntodo, double scale, double zero,
            char *cform, long twidth, char *output, int *status);
int ffr8fstr(double *input, long ntodo, double scale, double zero,
            char *cform, long twidth, char *output, int *status);

/*  the following 2 routines are VMS macros used on the VAX */
void ieevpd(double *inarray, double *outarray, long *nvals);
void ieevud(double *inarray, double *outarray, long *nvals);
 
#endif

