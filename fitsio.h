#ifndef _FITSIO_H
#define _FITSIO_H
 
#include <stdio.h>
/* stddef.h is apparently needed to define size_t */
#include <stddef.h>
#include "longnam.h"
 
/* global variables */
 
#define FLEN_KEYWORD    9  /* max length of a keyword */
#define FLEN_CARD      81  /* length of a FITS header card */
#define FLEN_VALUE     71  /* max length of a keyword value string */
#define FLEN_COMMENT   73  /* max length of a keyword comment string */
#define FLEN_ERRMSG    81  /* max length of a FITSIO error message */
#define FLEN_STATUS    31  /* max length of a FITSIO status text string */
 
#define TBIT          1  /* codes for FITS table data types */
#define TBYTE        11
#define TLOGICAL     14
#define TSTRING      16
#define TUSHORT      20
#define TSHORT       21
#define TINT         31
#define TULONG       40
#define TLONG        41
#define TFLOAT       42
#define TDOUBLE      82
#define TCOMPLEX     83
#define TDBLCOMPLEX 163

#define BYTE_IMG      8  /* BITPIX code values for FITS image types */
#define SHORT_IMG    16
#define LONG_IMG     32
#define FLOAT_IMG   -32
#define DOUBLE_IMG  -64
                         /* The following 2 codes are not true FITS         */
                         /* datatypes; these codes are only used internally */
                         /* within cfitsio to make it easier for users      */
                         /* to deal with unsigned integers.                 */
#define USHORT_IMG   20
#define ULONG_IMG    40

#define IMAGE_HDU  0  /* Primary Array or IMAGE HDU */
#define ASCII_TBL  1  /* ASCII table HDU  */
#define BINARY_TBL 2  /* Binary table HDU */
 
#define READONLY  0    /* options when openning a file */
#define READWRITE 1
 
#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define CASESEN   1   /* do case-sensitive string match */
#define CASEINSEN 0   /* do case-insensitive string match */
 
#define MAXHDU 1000    /* maximum number of extensions allowed in a FITS file */
 
typedef struct        /* structure used to store table column information */
{
    char ttype[70];   /* column name = FITS TTYPEn keyword; */
    long tbcol;       /* offset in row to first byte of each column */
    int  tdatatype;   /* datatype code of each column */
    int  trepeat;     /* repeat count of column; number of elements */
    double tscale;    /* FITS TSCALn linear scaling factor */
    double tzero;     /* FITS TZEROn linear scaling zero point */
    long tnull;       /* FITS null value for int image or binary table cols */
    char strnull[20]; /* FITS null value string for ASCII table columns */
    char tform[10];   /* FITS tform keyword value  */
    long  twidth;      /* width of each ASCII table column */
}tcolumn;
 
typedef struct      /* structure used to store basic HDU information */
{
    FILE *fileptr;  /* ptr returned by fopen */
    char *filename; /* file name */
    long bufftype;  /* buffer type: 101 = use memory buffer, else diskfile */
    void **memptr;  /* address of memory pointer, if used */
    size_t *memsize;  /* size of the memory buffer (bytes), if used */ 
    size_t deltasize; /* increment for future realloc's of memory buffer */
    void *(*mem_realloc)(void *p, size_t newsize);  /* realloc function */
    long filesize;  /* current size of the physical disk file in bytes */
    long logfilesize; /* logical size of file, including unflushed buffers */
    long bytepos;   /* current logical I/O pointer position in file */
    long io_pos;    /* current I/O pointer position in the physical file */
    int last_io_op; /* last I/O operation: seek (0), read (1), or write (2) */
    int curbuf;     /* number of I/O buffer currently in use */ 
    int curhdu;     /* current HDU number; 0 = primary array */
    int hdutype;    /* 0 = primary array, 1 = ASCII table, 2 = binary table */
    int writemode;  /* 0 = readonly, 1 = readwrite */
    int maxhdu;     /* highest numbered HDU known to exist in the file */
    long headstart[MAXHDU + 1]; /* byte offset in file to start of each HDU */
    long headend;   /* byte offest in file to end of the current HDU header */
    long nextkey;   /* byte offset in file to beginning of next keyword */
    long datastart; /* byte offset in file to start of the current data unit */
    int tfield;     /* number of fields in the table (primary array has 2 */
    long rowlength;  /* total length of a table row, in bytes */
    tcolumn *tableptr; /* pointer to the table structure */
    long heapstart;   /* heap start byte relative to start of data unit */
    long heapsize;    /* size of the heap, in bytes */
} fitsfile;
 
/* error status codes */
 
#define USE_MEM_BUFF     -101  /* use memory buffer when opening file */
#define OVERFLOW_ERR      -11  /* overflow during datatype conversion */
#define SAME_FILE         101  /* input and output files are the same */
#define TOO_MANY_FILES    103  /* tried to open too many FITS files */
#define FILE_NOT_OPENED   104  /* could not open the named file */
#define FILE_NOT_CREATED  105  /* could not create the named file */
#define WRITE_ERROR       106  /* error writing to FITS file */
#define END_OF_FILE       107  /* tried to move past end of file */
#define READ_ERROR        108  /* error reading from FITS file */
#define FILE_NOT_CLOSED   110  /* could not close the file */
#define ARRAY_TOO_BIG     111  /* array dimensions exceed internal limit */
#define READONLY_FILE     112  /* Cannot write to readonly file */
#define HEADER_NOT_EMPTY  201  /* header already contains keywords */
#define KEY_NO_EXIST      202  /* keyword not found in header */
#define KEY_OUT_BOUNDS    203  /* keyword record number is out of bounds */
#define VALUE_UNDEFINED   204  /* keyword value field is blank */
#define NO_QUOTE          205  /* string is missing the closing quote */
#define BAD_KEYCHAR       207  /* illegal character in keyword name or card */
#define BAD_ORDER         208  /* required keywords out of order */
#define NOT_POS_INT       209  /* keyword value is not a positive integer */
#define NO_END            210  /* couldn't find END keyword */
#define BAD_BITPIX        211  /* illegal BITPIX keyword value*/
#define BAD_NAXIS         212  /* illegal NAXIS keyword value */
#define BAD_NAXES         213  /* illegal NAXISn keyword value */
#define BAD_PCOUNT        214  /* illegal PCOUNT keyword value */
#define BAD_GCOUNT        215  /* illegal GCOUNT keyword value */
#define BAD_TFIELDS       216  /* illegal TFIELDS keyword value */
#define NEG_WIDTH         217  /* negative table row size */
#define NEG_ROWS          218  /* negative number of rows in table */
#define COL_NOT_FOUND     219  /* column with this name not found in table */
#define BAD_SIMPLE        220  /* illegal value of SIMPLE keyword  */
#define NO_SIMPLE         221  /* Primary array doesn't start with SIMPLE */
#define NO_BITPIX         222  /* Second keyword not BITPIX */
#define NO_NAXIS          223  /* Third keyword not NAXIS */
#define NO_NAXES          224  /* Couldn't find all the NAXISn keywords */
#define NO_XTENSION       225  /* HDU doesn't start with XTENSION keyword */
#define NOT_ATABLE        226  /* the CHDU is not an ASCII table extension */
#define NOT_BTABLE        227  /* the CHDU is not a binary table extension */
#define NO_PCOUNT         228  /* couldn't find PCOUNT keyword */
#define NO_GCOUNT         229  /* couldn't find GCOUNT keyword */
#define NO_TFIELDS        230  /* couldn't find TFIELDS keyword */
#define NO_TBCOL          231  /* couldn't find TBCOLn keyword */
#define NO_TFORM          232  /* couldn't find TFORMn keyword */
#define NOT_IMAGE         233  /* the CHDU is not an IMAGE extension */
#define BAD_TBCOL         234  /* TBCOLn keyword value < 0 or > rowlength */
#define NOT_TABLE         235  /* the CHDU is not a table */
#define COL_TOO_WIDE      236  /* column is too wide to fit in table */
#define COL_NOT_UNIQUE    237  /* more than 1 column name matches template */
#define BAD_ROW_WIDTH     241  /* sum of column widths not = NAXIS1 */
#define UNKNOWN_EXT       251  /* unrecognizable FITS extension type */
#define UNKNOWN_REC       252  /* unrecognizable FITS record */
#define END_JUNK          253  /* END keyword is not blank */
#define BAD_HEADER_FILL   254  /* Header fill area not blank */
#define BAD_DATA_FILL     255  /* Data fill area not blank or zero */
#define BAD_TFORM         261  /* illegal TFORM format code */
#define BAD_TFORM_DTYPE   262  /* unrecognizable TFORM datatype code */
#define BAD_TDIM          263  /* illegal TDIMn keyword value */
 
#define BAD_HDU_NUM       301  /* HDU number < 1 or > MAXHDU */
#define BAD_COL_NUM       302  /* column number < 1 or > tfields */
#define NEG_FILE_POS      304  /* tried to move before beginning of file  */
#define NEG_BYTES         306  /* tried to read or write negative bytes */
#define BAD_ROW_NUM       307  /* illegal starting row number in table */
#define BAD_ELEM_NUM      308  /* illegal starting element number in vector */
#define NOT_ASCII_COL     309  /* this is not an ASCII string column */
#define NOT_LOGICAL_COL   310  /* this is not a logical datatype column */
#define BAD_ATABLE_FORMAT 311  /* ASCII table column has wrong format */
#define BAD_BTABLE_FORMAT 312  /* Binary table column has wrong format */
#define NO_NULL           314  /* null value has not been defined */
#define NOT_VARI_LEN      317  /* this is not a variable length column */
#define BAD_DIMEN         320  /* illegal number of dimensions in array */
#define BAD_PIX_NUM       321  /* first pixel number greater than last pixel */
#define ZERO_SCALE        322  /* illegal BSCALE or TSCALn keyword = 0 */
#define NEG_AXIS          323  /* illegal axis length < 1 */
 
#define BAD_I2C           401  /* bad int to formatted string conversion */
#define BAD_F2C           402  /* bad float to formatted string conversion */
#define BAD_INTKEY        403  /* can't interprete keyword value as integer */
#define BAD_LOGICALKEY    404  /* can't interprete keyword value as logical */
#define BAD_FLOATKEY      405  /* can't interprete keyword value as float */
#define BAD_DOUBLEKEY     406  /* can't interprete keyword value as double */
#define BAD_C2I           407  /* bad formatted string to int conversion */
#define BAD_C2F           408  /* bad formatted string to float conversion */
#define BAD_C2D           409  /* bad formatted string to double conversion */
#define BAD_DATATYPE      410  /* bad keyword datatype code */
#define BAD_DECIM         411  /* bad number of decimal places specified */
#define NUM_OVERFLOW      412  /* overflow during datatype conversion */

#define ANGLE_TOO_BIG     501  /* celestial angle too large for projection */
#define BAD_WCS_VAL       502  /* bad celestial coordinate or pixel value */
#define WCS_ERROR         503  /* error in celestial coordinate calculation */
#define BAD_WCS_PROJ      505  /* unsupported type of celestial projection */
#define NO_WCS_KEY        505  /* celestial coordinate keywords not found */

/*  the following 3 lines are needed to support C++ compilers */
#ifdef __cplusplus
extern "C" {
#endif

/*----------------  FITS file I/O routines -------------*/
int ffsbuf(fitsfile **fptr, void **buffptr, size_t *buffsize,
           size_t deltasize, void *(*mem_realloc)(void *p, size_t newsize),
           int *status);
int ffwbuf(void *buffptr, size_t buffsize, const char *filename,
           int *status);
int ffopen(fitsfile **fptr, const char *filename, int iomode, int *status);
int ffinit(fitsfile **fptr, const char *filename, int *status);
int ffflus(fitsfile *fptr, int *status);
int ffclos(fitsfile *fptr, int *status);
int ffdelt(fitsfile *fptr, int *status);
 
/*---------------- utility routines -------------*/
float ffvers(float *version);
int ffgsdt(int *day, int *month, int *year2, int *status);
void ffupch(char *string);
void ffgerr(int status, char *errtext);
void ffpmsg(const char *err_message);
int  ffgmsg(char *err_message);
void ffcmsg(void);
void ffcmps(char *templt, char *colname, int  casesen, int *match,
           int *exact);
int fftkey(char *keyword, int *status);
int fftrec(char *card, int *status);
int ffkeyn(char *keyroot, int value, char *keyname, int *status);
int ffnkey(int value, char *keyroot, char *keyname, int *status);
int ffdtyp(char *cval, char *dtype, int *status);
int ffpsvc(char *card, char *value, char *comm, int *status);
int ffgthd(char *tmplt, char *card, int *hdtype, int *status);
int ffasfm(char *tform, int *datacode, long *width, int *decim, int *status);
int ffbnfm(char *tform, int *datacode, long *repeat, long *width, int *status);
int ffgabc(int tfields, char **tform, int space, long *rowlen, long *tbcol,
           int *status);
 
/*----------------- write single keywords --------------*/
int ffpky(fitsfile *fptr, int datatype, char *keyname, void *value,
          char *comm, int *status);
int ffprec(fitsfile *fptr, const char *card, int *status);
int ffpcom(fitsfile *fptr, const char *comm, int *status);
int ffpunt(fitsfile *fptr, char *keyname, char *unit, int *status);
int ffphis(fitsfile *fptr, const char *history, int *status);
int ffpdat(fitsfile *fptr, int *status);
int ffpkyu(fitsfile *fptr, char *keyname, char *comm, int *status);
int ffpkys(fitsfile *fptr, char *keyname, char *value, char *comm,int *status);
int ffpkls(fitsfile *fptr, char *keyname, char *value, char *comm,int *status);
int ffplsw(fitsfile *fptr, int *status);
int ffpkyl(fitsfile *fptr, char *keyname, int  value, char *comm, int *status);
int ffpkyj(fitsfile *fptr, char *keyname, long value, char *comm, int *status);
int ffpkyf(fitsfile *fptr, char *keyname, float value, int decim, char *comm,
          int *status);
int ffpkye(fitsfile *fptr, char *keyname, float  value, int decim, char *comm,
          int *status);
int ffpkyg(fitsfile *fptr, char *keyname, double value, int decim, char *comm,
          int *status);
int ffpkyd(fitsfile *fptr, char *keyname, double value, int decim, char *comm,
          int *status);
int ffpkyt(fitsfile *fptr, char *keyname, long intval, double frac, char *comm,
          int *status);
int ffptdm( fitsfile *fptr, int colnum, int naxis, long naxes[], int *status);
 
/*----------------- write array of keywords --------------*/
int ffpkns(fitsfile *fptr, char *keyroot, int nstart, int nkey, char *value[],
           char *comm[], int *status);
int ffpknl(fitsfile *fptr, char *keyroot, int nstart, int nkey, int *value,
           char *comm[], int *status);
int ffpknj(fitsfile *fptr, char *keyroot, int nstart, int nkey, long *value,
           char *comm[], int *status);
int ffpknf(fitsfile *fptr, char *keyroot, int nstart, int nkey, float *value,
           int decim, char *comm[], int *status);
int ffpkne(fitsfile *fptr, char *keyroot, int nstart, int nkey, float *value,
           int decim, char *comm[], int *status);
int ffpkng(fitsfile *fptr, char *keyroot, int nstart, int nkey, double *value,
           int decim, char *comm[], int *status);
int ffpknd(fitsfile *fptr, char *keyroot, int nstart, int nkey, double *value,
           int decim, char *comm[], int *status);
 
/*----------------- write required header keywords --------------*/
int ffphps( fitsfile *fptr, int bitpix, int naxis, long naxes[], int *status);
int ffphpr( fitsfile *fptr, int simple, int bitpix, int naxis, long naxes[],
            long pcount, long gcount, int extend, int *status);
int ffphtb(fitsfile *fptr, long naxis1, long naxis2, int tfields, char **ttype,
          long *tbcol, char **tform, char **tunit, char *extname, int *status);
int ffphbn(fitsfile *fptr, long naxis2, int tfields, char **ttype,
          char **tform, char **tunit, char *extname, long pcount, int *status);
 
/*------------------ get header information --------------*/
int ffghsp(fitsfile *fptr, int *nexist, int *nmore, int *status);
int ffghps(fitsfile *fptr, int *nexist, int *position, int *status);
 
/*------------------ move position in header -------------*/
int ffmaky(fitsfile *fptr, int nrec, int *status);
int ffmrky(fitsfile *fptr, int nrec, int *status);
 
/*------------------ read single keywords -----------------*/
int ffgnxk(fitsfile *fptr, char **inclist, int ninc, char **exclist,
           int nexc, char *card, int  *status);
int ffgrec(fitsfile *fptr, int nrec,      char *card, int *status);
int ffgcrd(fitsfile *fptr, char *keyname, char *card, int *status);
int ffgunt(fitsfile *fptr, char *keyname, char *unit, int  *status);
int ffgkyn(fitsfile *fptr, int nkey, char *keyname, char *keyval, char *comm,
           int *status);
int ffgkey(fitsfile *fptr, char *keyname, char *keyval, char *comm,
           int *status);
 
int ffgky( fitsfile *fptr, int datatype, char *keyname, void *value,
           char *comm, int *status);
int ffgkys(fitsfile *fptr, char *keyname, char *value, char *comm, int *status);
int ffgkls(fitsfile *fptr, char *keyname, char **value, char *comm, int *status)
;
int ffgkyl(fitsfile *fptr, char *keyname, int *value, char *comm, int *status);
int ffgkyj(fitsfile *fptr, char *keyname, long *value, char *comm, int *status);
int ffgkye(fitsfile *fptr, char *keyname, float *value, char *comm,int *status);
int ffgkyd(fitsfile *fptr, char *keyname, double *value,char *comm,int *status);
int ffgkyt(fitsfile *fptr, char *keyname, long *ivalue, double *dvalue,
           char *comm, int *status);
int ffgtdm(fitsfile *fptr, int colnum, int maxdim, int *naxis, long naxes[],
           int *status);
 
/*------------------ read array of keywords -----------------*/
int ffgkns(fitsfile *fptr, char *keyname, int nstart, int nmax, char *value[],
           int *nfound,  int *status);
int ffgknl(fitsfile *fptr, char *keyname, int nstart, int nmax, int *value,
           int *nfound, int *status);
int ffgknj(fitsfile *fptr, char *keyname, int nstart, int nmax, long *value,
           int *nfound, int *status);
int ffgkne(fitsfile *fptr, char *keyname, int nstart, int nmax, float *value,
           int *nfound, int *status);
int ffgknd(fitsfile *fptr, char *keyname, int nstart, int nmax, double *value,
           int *nfound, int *status);
 
/*----------------- read required header keywords --------------*/
int ffghpr(fitsfile *fptr, int maxdim, int *simple, int *bitpix, int *naxis,
          long naxes[], long *pcount, long *gcount, int *extend, int *status);
 
int ffghtb(fitsfile *fptr,int maxfield, long *naxis1, long *naxis2,
           int *tfields, char **ttype, long *tbcol, char **tform, char **tunit,
           char *extname,  int *status);
 
int ffghbn(fitsfile *fptr, int maxfield, long *naxis2, int *tfields,
           char **ttype, char **tform, char **tunit, char *extname,
           long *pcount, int *status);
 
/*--------------------- update keywords ---------------*/
int ffucrd(fitsfile *fptr, char *keyname, char *card, int *status);
int ffukyu(fitsfile *fptr, char *keyname, char *comm, int *status);
int ffukys(fitsfile *fptr, char *keyname, char *value, char *comm, int *status);
int ffukyl(fitsfile *fptr, char *keyname, int value, char *comm, int *status);
int ffukyj(fitsfile *fptr, char *keyname, long value, char *comm, int *status);
int ffukyf(fitsfile *fptr, char *keyname, float value, int decim, char *comm,
          int *status);
int ffukye(fitsfile *fptr, char *keyname, float value, int decim, char *comm,
          int *status);
int ffukyg(fitsfile *fptr, char *keyname, double value, int decim, char *comm,
          int *status);
int ffukyd(fitsfile *fptr, char *keyname, double value, int decim, char *comm,
          int *status);
 
/*--------------------- modify keywords ---------------*/
int ffmrec(fitsfile *fptr, int nkey, char *card, int *status);
int ffmcrd(fitsfile *fptr, char *keyname, char *card, int *status);
int ffmnam(fitsfile *fptr, char *oldname, char *newname, int *status);
int ffmcom(fitsfile *fptr, char *keyname, char *comm, int *status);
int ffmkyu(fitsfile *fptr, char *keyname, char *comm, int *status);
int ffmkys(fitsfile *fptr, char *keyname, char *value, char *comm,int *status);
int ffmkyl(fitsfile *fptr, char *keyname, int value, char *comm, int *status);
int ffmkyj(fitsfile *fptr, char *keyname, long value, char *comm, int *status);
int ffmkyf(fitsfile *fptr, char *keyname, float value, int decim, char *comm,
          int *status);
int ffmkye(fitsfile *fptr, char *keyname, float value, int decim, char *comm,
          int *status);
int ffmkyg(fitsfile *fptr, char *keyname, double value, int decim, char *comm,
          int *status);
int ffmkyd(fitsfile *fptr, char *keyname, double value, int decim, char *comm,
          int *status);
 
/*--------------------- insert keywords ---------------*/
int ffirec(fitsfile *fptr, int nkey, char *card, int *status);
int ffikyu(fitsfile *fptr, char *keyname, char *comm, int *status);
int ffikys(fitsfile *fptr, char *keyname, char *value, char *comm,int *status);
int ffikyl(fitsfile *fptr, char *keyname, int value, char *comm, int *status);
int ffikyj(fitsfile *fptr, char *keyname, long value, char *comm, int *status);
int ffikyf(fitsfile *fptr, char *keyname, float value, int decim, char *comm,
          int *status);
int ffikye(fitsfile *fptr, char *keyname, float value, int decim, char *comm,
          int *status);
int ffikyg(fitsfile *fptr, char *keyname, double value, int decim, char *comm,
          int *status);
int ffikyd(fitsfile *fptr, char *keyname, double value, int decim, char *comm,
          int *status);
 
/*--------------------- delete keywords ---------------*/
int ffdkey(fitsfile *fptr, char *keyname, int *status);
int ffdrec(fitsfile *fptr, int keypos, int *status);
 
/*--------------------- get HDU information -------------*/
int ffghdn(fitsfile *fptr, int *chdunum);
int ffghad(fitsfile *fptr, long *chduaddr, long *nextaddr);
 
/*--------------------- HDU operations -------------*/
int ffmahd(fitsfile *fptr, int hdunum, int *exttype, int *status);
int ffmrhd(fitsfile *fptr, int hdumov, int *exttype, int *status);
int ffcrhd(fitsfile *fptr, int *status);
int ffcrim(fitsfile *fptr, int bitpix, int naxis, long *naxes, int *status);
int ffcrtb(fitsfile *fptr, int tbltype, long naxis2, int tfields, char **ttype,
           char **tform, char **tunit, char *extname, int *status);
int ffiimg(fitsfile *fptr, int bitpix, int naxis, long *naxes, int *status);
int ffitab(fitsfile *fptr, long naxis1, long naxis2, int tfields, char **ttype,
           long *tbcol, char **tform, char **tunit, char *extname, int *status);
int ffibin(fitsfile *fptr,long naxis2, int tfields, char **ttype, char **tform,
           char **tunit, char *extname, long pcount, int *status);
int ffrsim(fitsfile *fptr, int bitpix, int naxis, long *naxes, int *status);
int ffdhdu(fitsfile *fptr, int *hdutype, int *status);
int ffcopy(fitsfile *infptr, fitsfile *outfptr, int morekeys, int *status);
int ffcpdt(fitsfile *infptr, fitsfile *outfptr, int *status);
int ffchfl(fitsfile *fptr, int *status);
int ffcdfl(fitsfile *fptr, int *status);

int ffrdef(fitsfile *fptr, int *status);
int ffhdef(fitsfile *fptr, int morekeys, int *status);
int ffpthp(fitsfile *fptr, long theap, int *status);
 
int ffcsum(fitsfile *fptr, long nrec, unsigned long *sum, int *status);
void ffesum(unsigned long sum, int complm, char *ascii);
unsigned long ffdsum(char *ascii, int complm, unsigned long *sum);
int ffpcks(fitsfile *fptr, int *status);
int ffupck(fitsfile *fptr, int *status);
int ffvcks(fitsfile *fptr, int *datastatus, int *hdustatus, int *status);
int ffgcks(fitsfile *fptr, unsigned long *datasum, unsigned long *hdusum,
    int *status);
 
/*--------------------- define scaling or null values -------------*/
int ffpscl(fitsfile *fptr, double scale, double zero, int *status);
int ffpnul(fitsfile *fptr, long nulvalue, int *status);
int fftscl(fitsfile *fptr, int colnum, double scale, double zero, int *status);
int fftnul(fitsfile *fptr, int colnum, long nulvalue, int *status);
int ffsnul(fitsfile *fptr, int colnum, char *nulstring, int *status);
 
/*--------------------- get column information -------------*/
int ffgcno(fitsfile *fptr, int casesen, char *templt, int  *colnum,
           int *status);
int ffgcnn(fitsfile *fptr, int casesen, char *templt, char *colname,
           int *colnum, int *status);
 
int ffgtcl(fitsfile *fptr, int colnum, int *typecode, long *repeat,
           long *width, int *status);
int ffgacl(fitsfile *fptr, int colnum, char *ttype, long *tbcol,
           char *tunit, char *tform, double *tscal, double *tzero,
           char *tnull, char *tdisp, int *status);
int ffgbcl(fitsfile *fptr, int colnum, char *ttype, char *tunit,
           char *dtype, long *repeat, double *tscal, double *tzero,
           long *tnull, char *tdisp, int  *status);
int ffgrsz(fitsfile *fptr, long *nrows, int *status);

/*--------------------- read primary array or image elements -------------*/
int ffgpv(fitsfile *fptr, int  datatype, long firstelem, long nelem,
          void *nulval, void *array, int *anynul, int  *status);
int ffgpvb(fitsfile *fptr, long group, long firstelem, long nelem, unsigned
           char nulval, unsigned char *array, int *anynul, int *status);
int ffgpvui(fitsfile *fptr, long group, long firstelem, long nelem,
           unsigned short nulval, unsigned short *array, int *anynul, 
           int *status);
int ffgpvi(fitsfile *fptr, long group, long firstelem, long nelem,
           short nulval, short *array, int *anynul, int *status);
int ffgpvuj(fitsfile *fptr, long group, long firstelem, long nelem,
           unsigned long nulval, unsigned long *array, int *anynul, 
           int *status);
int ffgpvj(fitsfile *fptr, long group, long firstelem, long nelem,
           long nulval, long *array, int *anynul, int *status);
int ffgpvk(fitsfile *fptr, long group, long firstelem, long nelem,
           int nulval, int *array, int *anynul, int *status);
int ffgpve(fitsfile *fptr, long group, long firstelem, long nelem,
           float nulval, float *array, int *anynul, int *status);
int ffgpvd(fitsfile *fptr, long group, long firstelem, long nelem,
           double nulval, double *array, int *anynul, int *status);
 
int ffgpfb(fitsfile *fptr, long group, long firstelem, long nelem,
           unsigned char *array, char *nularray, int *anynul, int *status);
int ffgpfui(fitsfile *fptr, long group, long firstelem, long nelem,
           unsigned short *array, char *nularray, int *anynul, int *status);
int ffgpfi(fitsfile *fptr, long group, long firstelem, long nelem,
           short *array, char *nularray, int *anynul, int *status);
int ffgpfuj(fitsfile *fptr, long group, long firstelem, long nelem,
           unsigned long *array, char *nularray, int *anynul, int *status);
int ffgpfj(fitsfile *fptr, long group, long firstelem, long nelem,
           long *array, char *nularray, int *anynul, int *status);
int ffgpfk(fitsfile *fptr, long group, long firstelem, long nelem,
           int *array, char *nularray, int *anynul, int *status);
int ffgpfe(fitsfile *fptr, long group, long firstelem, long nelem,
           float *array, char *nularray, int *anynul, int *status);
int ffgpfd(fitsfile *fptr, long group, long firstelem, long nelem,
           double *array, char *nularray, int *anynul, int *status);
 
int ffg2db(fitsfile *fptr, long group, unsigned char nulval, long ncols,
           long naxis1, long naxis2, unsigned char *array,
           int *anynul, int *status);
int ffg2dui(fitsfile *fptr, long group, unsigned short nulval, long ncols,
           long naxis1, long naxis2, unsigned short *array,
           int *anynul, int *status);
int ffg2di(fitsfile *fptr, long group, short nulval, long ncols,
           long naxis1, long naxis2, short *array,
           int *anynul, int *status);
int ffg2duj(fitsfile *fptr, long group, unsigned long nulval, long ncols,
           long naxis1, long naxis2, unsigned long *array,
           int *anynul, int *status);
int ffg2dj(fitsfile *fptr, long group, long nulval, long ncols,
           long naxis1, long naxis2, long *array,
           int *anynul, int *status);
int ffg2dk(fitsfile *fptr, long group, int nulval, long ncols,
           long naxis1, long naxis2, int *array,
           int *anynul, int *status);
int ffg2de(fitsfile *fptr, long group, float nulval, long ncols,
           long naxis1, long naxis2, float *array,
           int *anynul, int *status);
int ffg2dd(fitsfile *fptr, long group, double nulval, long ncols,
           long naxis1, long naxis2, double *array,
           int *anynul, int *status);
 
int ffg3db(fitsfile *fptr, long group, unsigned char nulval, long ncols,
           long nrows, long naxis1, long naxis2, long naxis3,
           unsigned char *array, int *anynul, int *status);
int ffg3dui(fitsfile *fptr, long group, unsigned short nulval, long ncols,
           long nrows, long naxis1, long naxis2, long naxis3,
           unsigned short *array, int *anynul, int *status);
int ffg3di(fitsfile *fptr, long group, short nulval, long ncols,
           long nrows, long naxis1, long naxis2, long naxis3,
           short *array, int *anynul, int *status);
int ffg3duj(fitsfile *fptr, long group, unsigned long nulval, long ncols,
           long nrows, long naxis1, long naxis2, long naxis3,
           unsigned long *array, int *anynul, int *status);
int ffg3dj(fitsfile *fptr, long group, long nulval, long ncols,
           long nrows, long naxis1, long naxis2, long naxis3,
           long *array, int *anynul, int *status);
int ffg3dk(fitsfile *fptr, long group, int nulval, long ncols,
           long nrows, long naxis1, long naxis2, long naxis3,
           int *array, int *anynul, int *status);
int ffg3de(fitsfile *fptr, long group, float nulval, long ncols,
           long nrows, long naxis1, long naxis2, long naxis3,
           float *array, int *anynul, int *status);
int ffg3dd(fitsfile *fptr, long group, double nulval, long ncols,
           long nrows, long naxis1, long naxis2, long naxis3,
           double *array, int *anynul, int *status);
 
int ffgsvb(fitsfile *fptr, int colnum, int naxis, long *naxes, long *blc,
  long *trc, long *inc, unsigned char nulval, unsigned char *array,
  int *anynul, int *status);
int ffgsvui(fitsfile *fptr, int colnum, int naxis, long *naxes, long *blc,
  long *trc, long *inc, unsigned short nulval, unsigned short *array, 
  int *anynul, int *status);
int ffgsvi(fitsfile *fptr, int colnum, int naxis, long *naxes, long *blc,
  long *trc, long *inc, short nulval, short *array, int *anynul, int *status);
int ffgsvuj(fitsfile *fptr, int colnum, int naxis, long *naxes, long *blc,
  long *trc, long *inc, unsigned long nulval, unsigned long *array, 
  int *anynul, int *status);
int ffgsvj(fitsfile *fptr, int colnum, int naxis, long *naxes, long *blc,
  long *trc, long *inc, long nulval, long *array, int *anynul, int *status);
int ffgsvk(fitsfile *fptr, int colnum, int naxis, long *naxes, long *blc,
  long *trc, long *inc, int nulval, int *array, int *anynul, int *status);
int ffgsve(fitsfile *fptr, int colnum, int naxis, long *naxes, long *blc,
  long *trc, long *inc, float nulval, float *array, int *anynul, int *status);
int ffgsvd(fitsfile *fptr, int colnum, int naxis, long *naxes, long *blc,
  long *trc, long *inc, double nulval, double *array, int *anynul,
  int *status);
 
int ffgsfb(fitsfile *fptr, int colnum, int naxis, long *naxes, long *blc,
  long *trc, long *inc, unsigned char *array, char *flagval,
  int *anynul, int *status);
int ffgsfui(fitsfile *fptr, int colnum, int naxis, long *naxes, long *blc,
  long *trc, long *inc, unsigned short *array, char *flagval, int *anynul, 
  int *status);
int ffgsfi(fitsfile *fptr, int colnum, int naxis, long *naxes, long *blc,
  long *trc, long *inc, short *array, char *flagval, int *anynul, int *status);
int ffgsfuj(fitsfile *fptr, int colnum, int naxis, long *naxes, long *blc,
  long  *trc, long *inc, unsigned long *array, char *flagval, int *anynul,
  int *status);
int ffgsfj(fitsfile *fptr, int colnum, int naxis, long *naxes, long *blc,
  long  *trc, long *inc, long *array, char *flagval, int *anynul, int *status);
int ffgsfk(fitsfile *fptr, int colnum, int naxis, long *naxes, long *blc,
  long  *trc, long *inc, int *array, char *flagval, int *anynul, int *status);
int ffgsfe(fitsfile *fptr, int colnum, int naxis, long *naxes, long *blc,
  long *trc, long *inc, float *array, char *flagval, int *anynul, int *status);
int ffgsfd(fitsfile *fptr, int colnum, int naxis, long *naxes, long *blc,
  long *trc, long *inc, double *array, char *flagval, int *anynul,
  int *status);
 
int ffggpb(fitsfile *fptr, long group, long firstelem, long nelem,
           unsigned char *array, int *status);
int ffggpui(fitsfile *fptr, long group, long firstelem, long nelem,
           unsigned short *array, int *status);
int ffggpi(fitsfile *fptr, long group, long firstelem, long nelem,
           short *array, int *status);
int ffggpuj(fitsfile *fptr, long group, long firstelem, long nelem,
           unsigned long *array, int *status);
int ffggpj(fitsfile *fptr, long group, long firstelem, long nelem,
           long *array, int *status);
int ffggpk(fitsfile *fptr, long group, long firstelem, long nelem,
           int *array, int *status);
int ffggpe(fitsfile *fptr, long group, long firstelem, long nelem,
           float *array, int *status);
int ffggpd(fitsfile *fptr, long group, long firstelem, long nelem,
           double *array, int *status);
 
/*--------------------- read column elements -------------*/
int ffgcv( fitsfile *fptr, int datatype, int colnum, long firstrow,
           long firstelem, long nelem, void *nulval, void *array, int *anynul,
           int  *status);
int ffgcvs(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, char *nulval, char **array, int *anynul, int *status);
int ffgcl (fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, char *array, int  *status);
int ffgcvb(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, unsigned char nulval, unsigned char *array,
           int *anynul, int *status);
int ffgcvui(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, unsigned short nulval, unsigned short *array, 
           int *anynul, int *status);
int ffgcvi(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, short nulval, short *array, int *anynul, int *status);
int ffgcvuj(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, unsigned long nulval, unsigned long *array, int *anynul,
           int *status);
int ffgcvj(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, long nulval, long *array, int *anynul, int *status);
int ffgcvk(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, int nulval, int *array, int *anynul, int *status);
int ffgcve(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, float nulval, float *array, int *anynul, int *status);
int ffgcvd(fitsfile *fptr, int colnum, long firstrow, long firstelem,
         long nelem, double nulval, double *array, int *anynul, int *status);
int ffgcvc(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, float nulval, float *array, int *anynul, int *status);
int ffgcvm(fitsfile *fptr, int colnum, long firstrow, long firstelem,
         long nelem, double nulval, double *array, int *anynul, int *status);
int ffgcx(fitsfile *fptr, int colnum, long firstrow, long firstbit,
            long nbits, char *larray, int *status);
 
int ffgcfs(fitsfile *fptr, int colnum, long firstrow, long firstelem, long
          nelem, char **array, char *nularray, int *anynul, int *status);
int ffgcfl(fitsfile *fptr, int colnum, long firstrow, long firstelem, long
          nelem, char *array, char *nularray, int *anynul, int *status);
int ffgcfb(fitsfile *fptr, int colnum, long firstrow, long firstelem, long
      nelem, unsigned char *array, char *nularray, int *anynul, int *status);
int ffgcfui(fitsfile *fptr, int colnum, long firstrow, long firstelem,
      long nelem, unsigned short *array, char *nularray, int *anynul, 
      int *status);
int ffgcfi(fitsfile *fptr, int colnum, long firstrow, long firstelem,
      long nelem, short *array, char *nularray, int *anynul, int *status);
int ffgcfuj(fitsfile *fptr, int colnum, long firstrow, long firstelem,
      long nelem, unsigned long *array, char *nularray, int *anynul,
      int *status);
int ffgcfj(fitsfile *fptr, int colnum, long firstrow, long firstelem,
      long nelem, long *array, char *nularray, int *anynul, int *status);
int ffgcfk(fitsfile *fptr, int colnum, long firstrow, long firstelem,
      long nelem, int *array, char *nularray, int *anynul, int *status);
int ffgcfe(fitsfile *fptr, int colnum, long firstrow, long firstelem,
      long nelem, float *array, char *nularray, int *anynul, int *status);
int ffgcfd(fitsfile *fptr, int colnum, long firstrow, long firstelem,
      long nelem, double *array, char *nularray, int *anynul, int *status);
int ffgcfc(fitsfile *fptr, int colnum, long firstrow, long firstelem,
      long nelem, float *array, char *nularray, int *anynul, int *status);
int ffgcfm(fitsfile *fptr, int colnum, long firstrow, long firstelem,
      long nelem, double *array, char *nularray, int *anynul, int *status);
 
int ffgdes(fitsfile *fptr, int colnum, long rownum, long *length,
           long *heapaddr, int *status);
 
int ffgtbb(fitsfile *fptr, long firstrow, long firstchar, long nchars,
           unsigned char *values, int *status);
 
/*------------ write primary array or image elements -------------*/
int ffppr(fitsfile *fptr, int  datatype, long  firstelem, long  nelem,
          void  *array, int  *status);
int ffpprb(fitsfile *fptr, long group, long firstelem,
           long nelem, unsigned char *array, int *status);
int ffpprui(fitsfile *fptr, long group, long firstelem,
           long nelem, unsigned short *array, int *status);
int ffppri(fitsfile *fptr, long group, long firstelem,
           long nelem, short *array, int *status);
int ffppruj(fitsfile *fptr, long group, long firstelem,
           long nelem, unsigned long *array, int *status);
int ffpprj(fitsfile *fptr, long group, long firstelem,
           long nelem, long *array, int *status);
int ffpprk(fitsfile *fptr, long group, long firstelem,
           long nelem, int *array, int *status);
int ffppre(fitsfile *fptr, long group, long firstelem,
           long nelem, float *array, int *status);
int ffpprd(fitsfile *fptr, long group, long firstelem,
           long nelem, double *array, int *status);
 
int ffppnb(fitsfile *fptr, long group, long firstelem, long nelem,
           unsigned char *array, unsigned char nulval, int *status);
int ffppnui(fitsfile *fptr, long group, long firstelem,
           long nelem, unsigned short *array, unsigned short nulval,
           int *status);
int ffppni(fitsfile *fptr, long group, long firstelem,
           long nelem, short *array, short nulval, int *status);
int ffppnj(fitsfile *fptr, long group, long firstelem,
           long nelem, long *array, long nulval, int *status);
int ffppnuj(fitsfile *fptr, long group, long firstelem, long nelem,
           unsigned long *array, unsigned long nulval, int *status);
int ffppnk(fitsfile *fptr, long group, long firstelem,
           long nelem, int *array, int nulval, int *status);
int ffppne(fitsfile *fptr, long group, long firstelem,
           long nelem, float *array, float nulval, int *status);
int ffppnd(fitsfile *fptr, long group, long firstelem,
           long nelem, double *array, double nulval, int *status);
 
int ffp2db(fitsfile *fptr, long group, long ncols, long naxis1,
           long naxis2, unsigned char *array, int *status);
int ffp2dui(fitsfile *fptr, long group, long ncols, long naxis1,
           long naxis2, unsigned short *array, int *status);
int ffp2di(fitsfile *fptr, long group, long ncols, long naxis1,
           long naxis2, short *array, int *status);
int ffp2duj(fitsfile *fptr, long group, long ncols, long naxis1,
           long naxis2, unsigned long *array, int *status);
int ffp2dj(fitsfile *fptr, long group, long ncols, long naxis1,
           long naxis2, long *array, int *status);
int ffp2dk(fitsfile *fptr, long group, long ncols, long naxis1,
           long naxis2, int *array, int *status);
int ffp2de(fitsfile *fptr, long group, long ncols, long naxis1,
           long naxis2, float *array, int *status);
int ffp2dd(fitsfile *fptr, long group, long ncols, long naxis1,
           long naxis2, double *array, int *status);
 
int ffp3db(fitsfile *fptr, long group, long ncols, long nrows, long naxis1,
           long naxis2, long naxis3, unsigned char *array, int *status);
int ffp3dui(fitsfile *fptr, long group, long ncols, long nrows, long naxis1,
           long naxis2, long naxis3, unsigned short *array, int *status);
int ffp3di(fitsfile *fptr, long group, long ncols, long nrows, long naxis1,
           long naxis2, long naxis3, short *array, int *status);
int ffp3duj(fitsfile *fptr, long group, long ncols, long nrows, long naxis1,
           long naxis2, long naxis3, unsigned long *array, int *status);
int ffp3dj(fitsfile *fptr, long group, long ncols, long nrows, long naxis1,
           long naxis2, long naxis3, long *array, int *status);
int ffp3dk(fitsfile *fptr, long group, long ncols, long nrows, long naxis1,
           long naxis2, long naxis3, int *array, int *status);
int ffp3de(fitsfile *fptr, long group, long ncols, long nrows, long naxis1,
           long naxis2, long naxis3, float *array, int *status);
int ffp3dd(fitsfile *fptr, long group, long ncols, long nrows, long naxis1,
           long naxis2, long naxis3, double *array, int *status);
 
int ffpssb(fitsfile *fptr, long group, long naxis, long *naxes,
           long *fpixel, long *lpixel, unsigned char *array, int *status);
int ffpssui(fitsfile *fptr, long group, long naxis, long *naxes,
           long *fpixel, long *lpixel, unsigned short *array, int *status);
int ffpssi(fitsfile *fptr, long group, long naxis, long *naxes,
           long *fpixel, long *lpixel, short *array, int *status);
int ffpssuj(fitsfile *fptr, long group, long naxis, long *naxes,
           long *fpixel, long *lpixel, unsigned long *array, int *status);
int ffpssj(fitsfile *fptr, long group, long naxis, long *naxes,
           long *fpixel, long *lpixel, long *array, int *status);
int ffpssk(fitsfile *fptr, long group, long naxis, long *naxes,
           long *fpixel, long *lpixel, int *array, int *status);
int ffpsse(fitsfile *fptr, long group, long naxis, long *naxes,
           long *fpixel, long *lpixel, float *array, int *status);
int ffpssd(fitsfile *fptr, long group, long naxis, long *naxes,
           long *fpixel, long *lpixel, double *array, int *status);
 
int ffpgpb(fitsfile *fptr, long group, long firstelem,
           long nelem, unsigned char *array, int *status);
int ffpgpui(fitsfile *fptr, long group, long firstelem,
           long nelem, unsigned short *array, int *status);
int ffpgpi(fitsfile *fptr, long group, long firstelem,
           long nelem, short *array, int *status);
int ffpgpuj(fitsfile *fptr, long group, long firstelem,
           long nelem, unsigned long *array, int *status);
int ffpgpj(fitsfile *fptr, long group, long firstelem,
           long nelem, long *array, int *status);
int ffpgpk(fitsfile *fptr, long group, long firstelem,
           long nelem, int *array, int *status);
int ffpgpe(fitsfile *fptr, long group, long firstelem,
           long nelem, float *array, int *status);
int ffpgpd(fitsfile *fptr, long group, long firstelem,
           long nelem, double *array, int *status);
 
int ffppru(fitsfile *fptr, long group, long firstelem,
           long nelem, int *status);
 
/*--------------------- write column elements -------------*/
int ffpcl(fitsfile *fptr, int datatype, int colnum, long firstrow,
          long firstelem, long nelem, void *array, int *status);
int ffpcls(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, char **array, int *status);
int ffpcll(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, char *array, int *status);
int ffpclb(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, unsigned char *array, int *status);
int ffpclui(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, unsigned short *array, int *status);
int ffpcli(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, short *array, int *status);
int ffpcluj(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, unsigned long *array, int *status);
int ffpclj(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, long *array, int *status);
int ffpclk(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, int *array, int *status);
int ffpcle(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, float *array, int *status);
int ffpcld(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, double *array, int *status);
int ffpclc(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, float *array, int *status);
int ffpclm(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, double *array, int *status);
int ffpclu(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, int *status);
int ffpclx(fitsfile *fptr, int colnum, long frow, long fbit, long nbit,
            char *larray, int *status);
int ffpcnb(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, unsigned char *array, unsigned char nulvalue,
           int *status);
int ffpcnui(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, unsigned short *array, unsigned short nulvalue,
           int *status);
int ffpcni(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, short *array, short nulvalue, int *status);
int ffpcnuj(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, unsigned long *array, unsigned long nulvalue,
           int *status);
int ffpcnj(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, long *array, long nulvalue, int *status);
int ffpcnk(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, int *array, int nulvalue, int *status);
int ffpcne(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, float *array, float nulvalue, int *status);
int ffpcnd(fitsfile *fptr, int colnum, long firstrow, long firstelem,
           long nelem, double *array, double nulvalue, int *status);
 
int ffpdes(fitsfile *fptr, int colnum, long rownum, long length,
           long heapaddr, int *status);
 
int ffptbb(fitsfile *fptr, long firstrow, long firstchar, long nchars,
           unsigned char *values, int *status);
 
int ffirow(fitsfile *fptr, long firstrow, long nrows, int *status);
int ffdrow(fitsfile *fptr, long firstrow, long nrows, int *status);
int fficol(fitsfile *fptr, int numcol, char *ttype, char *tform, int *status);
int fficls(fitsfile *fptr, int firstcol, int ncols, char **ttype,
           char **tform, int *status);
int ffdcol(fitsfile *fptr, int numcol, int *status);

/*--------------------- WCS Utilities -------------*/
int ffgics(fitsfile *fptr, double *xrval, double *yrval, double *xrpix,
           double *yrpix, double *xinc, double *yinc, double *rot,
           char *type, int *status);
int ffgtcs(fitsfile *fptr, int xcol, int ycol, double *xrval,
           double *yrval, double *xrpix, double *yrpix, double *xinc,
           double *yinc, double *rot, char *type, int *status);
int ffwldp(double xpix, double ypix, double xref, double yref,
           double xrefpix, double yrefpix, double xinc, double yinc,
           double rot, char *type, double *xpos, double *ypos, int *status);
int ffxypx(double xpos, double ypos, double xref, double yref, 
           double xrefpix, double yrefpix, double xinc, double yinc,
           double rot, char *type, double *xpix, double *ypix, int *status);

#ifdef __cplusplus
}
#endif

#endif

