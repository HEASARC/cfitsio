#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
  Every program which uses the CFITSIO interface must include the
  the fitsio.h header file.  This contains the prototypes for all
  the routines and defines the error status values and other symbolic
  constants used in the interface.  
*/
#include "fitsio2.h"

#define minvalue(A,B) ((A) < (B) ? (A) : (B))

#define RAWLOOP 8700
#define RAWSIZE 2880

#define XSIZE 2500
#define YSIZE 5000

#define SHTSIZE 100000
static short array[ SHTSIZE ];

#define BUFFSIZE 6000
static short  sarray[BUFFSIZE];
static float  farray[BUFFSIZE];
static double darray[BUFFSIZE];

/* no. of rows in binary table */
#define BROWS 1800000

/* no. of rows in ASCII table */
#define AROWS 500000

/*  CLOCKS_PER_SEC should be defined in most compilers */
#if defined(CLOCKS_PER_SEC)
#define CLOCKTICKS CLOCKS_PER_SEC
#else
/* on SUN OS machine, CLOCKS_PER_SEC is not defined, so set its value */
#define CLOCKTICKS 1000000
#define difftime(A,B) ((double) A - (double) B)
#endif

int writeimage(fitsfile *fptr, int *status);
int writebintable(fitsfile *fptr, int *status);
int writeasctable(fitsfile *fptr, int *status);
int readimage(fitsfile *fptr, int *status);
int readatable(fitsfile *fptr, int *status);
int readbtable(fitsfile *fptr, int *status);
void printerror( int status);

main()
{
/*************************************************************************
    This program tests the speed of writing/reading FITS files with cfitsio
**************************************************************************/

    FILE *diskfile;
    fitsfile *fptr;        /* pointer to the FITS file, defined in fitsio.h */
    int status, ii;
    char filename[] = "speedxx.fit";              /* name for new FITS file */
    char buffer[2880] = {2880 * 0};
    char buff2[2880];
    clock_t scpu, ecpu;
    time_t start, finish;
    time_t tbegin, tend;
    float rate, size, elapcpu, cpufrac;
    double elapse;

    tbegin = time(0);

/* goto skipit; */

    remove(filename);               /* Delete old file if it already exists */

    diskfile =  fopen(filename,"w+b");
    printf("                                                ");
    printf(" SIZE / ELAPSE(\%CPU) = RATE\n");
/*-----------------------------------------------------------------*/
    printf("RAW fwrite (2880 bytes/loop)...                 ");

    scpu = clock();
    start = time(0);

    for (ii = 0; ii < RAWLOOP; ii++)
      if (fwrite(buff2, 1, 2880, diskfile) != 2880)
        printf("write error \n");

    finish = time(0);
    ecpu = clock();
    elapcpu = (ecpu - scpu) * 1.0 / CLOCKTICKS;
    elapse = difftime(finish, start) + 0.5;
    cpufrac = elapcpu / elapse * 100.;
    size = 2880. * RAWLOOP / 1000000.;
    rate = size / elapse;
    printf(" %4.1fMB/%4.1fs(%3.0f) = %5.2fMB/s\n", size, elapse, cpufrac,rate);
/*-----------------------------------------------------------------*/

#if BYTESWAPPED == TRUE
    printf("RAW fwrite (2880 bytes/loop) + copy + swap...   ");
#else
    printf("RAW fwrite (2880 bytes/loop) + copy...          ");
#endif

    scpu = clock();
    start = time(0);
    for (ii = 0; ii < RAWLOOP; ii++)
    {
      memcpy(buff2, buffer, 2880);
#if BYTESWAPPED == TRUE
      ffswap2( (short *) buff2, 2880/2);
#endif
      if (fwrite(buff2, 1, 2880, diskfile) != 2880)
        printf("write error \n");
    }
    finish = time(0);
    ecpu = clock();
    elapcpu = (ecpu - scpu) * 1.0 / CLOCKTICKS;
    elapse = difftime(finish, start) + 0.5;
    cpufrac = elapcpu / elapse * 100.;
    size = 2880. * RAWLOOP / 1000000.;
    rate = size / elapse;
    printf(" %4.1fMB/%4.1fs(%3.0f) = %5.2fMB/s\n", size, elapse, cpufrac,rate);

/*-----------------------------------------------------------------*/

    printf("RAW fread  (2880 bytes/loop)...                 ");

    fseek(diskfile, 0, 0);
    scpu = clock();
    start = time(0);
    for (ii = 0; ii < RAWLOOP; ii++)
      if (fread(buff2, 1, 2880, diskfile) != 2880)
        printf("read error \n");

    finish = time(0);
    ecpu = clock();
    elapcpu = (ecpu - scpu) * 1.0 / CLOCKTICKS;
    elapse = difftime(finish, start) + 0.5;
    cpufrac = elapcpu / elapse * 100.;
    size = 2880. * RAWLOOP / 1000000.;
    rate = size / elapse;
    printf(" %4.1fMB/%4.1fs(%3.0f) = %5.2fMB/s\n", size, elapse, cpufrac,rate);
/*-----------------------------------------------------------------*/

#if BYTESWAPPED == TRUE
    printf("RAW fread  (2880 bytes/loop) + copy + swap...   ");
#else
    printf("RAW fread  (2880 bytes/loop) + copy...          ");
#endif

    scpu = clock();
    start = time(0);
    for (ii = 0; ii < RAWLOOP; ii++)
    {
      memcpy(buff2, buffer, 2880);
#if BYTESWAPPED == TRUE
      ffswap2( (short *) buff2, 2880/2);
#endif
      if (fread(buff2, 1, 2880, diskfile) != 2880)
        printf("read error \n");
    }
    finish = time(0);
    ecpu = clock();
    elapcpu = (ecpu - scpu) * 1.0 / CLOCKTICKS;
    elapse = difftime(finish, start) + 0.5;
    cpufrac = elapcpu / elapse * 100.;
    size = 2880. * RAWLOOP / 1000000.;
    rate = size / elapse;
    printf(" %4.1fMB/%4.1fs(%3.0f) = %5.2fMB/s\n", size, elapse, cpufrac,rate);
/*-----------------------------------------------------------------*/

    fclose(diskfile);
skipit:

    remove(filename);

    status = 0;     

    if (fits_create_file(&fptr, filename, &status)) /* create new FITS file */
       printerror( status);          
   
    if (writeimage(fptr, &status))
       printerror( status);     

    if (writebintable(fptr, &status))
       printerror( status);     

    if (writeasctable(fptr, &status))
       printerror( status);     

    if (readimage(fptr, &status))
       printerror( status);     

    if (readbtable(fptr, &status))
       printerror( status);     

    if (readatable(fptr, &status))
       printerror( status);     

    if (fits_close_file(fptr, &status))     
         printerror( status );

    tend = time(0);
    elapse = difftime(tend, tbegin) + 0.5;
    printf("Total elapsed time = %.1fs, status = %d\n",elapse, status);
    return(0);
}
/*--------------------------------------------------------------------------*/
int writeimage(fitsfile *fptr, int *status)

    /**************************************************/
    /* write the primary array containing a 2-D image */
    /**************************************************/
{
    long  nremain, ntodo, fpixel = 1;
    time_t start, finish;
    clock_t scpu, ecpu;
    float rate, size, elapcpu, cpufrac;
    double elapse;

    /* initialize FITS image parameters */
    int bitpix   =  16;   /* 16-bit short signed integer pixel values       */
    long naxis    =   2;  /* 2-dimensional image                            */    
    long naxes[2] = {XSIZE, YSIZE }; /* image size */

    /* write the required keywords for the primary array image */
    if ( fits_create_img(fptr, bitpix, naxis, naxes, status) )
         printerror( *status );          

    printf("\nWrite %dx%d short image, %d pixels/loop:", XSIZE, YSIZE, SHTSIZE);

    nremain = XSIZE * YSIZE;

    scpu = clock();
    start = time(0);

    while(nremain)
    {
      ntodo = minvalue(SHTSIZE, nremain);
      ffppri(fptr, 0, fpixel, ntodo, array, status);
      fpixel += ntodo;
      nremain -= ntodo;
    }

    ffflus(fptr, status);  /* flush all buffers to disk */

    finish = time(0);

    elapse = difftime(finish, start) + 0.5;
    ecpu = clock();
    elapcpu = (ecpu - scpu) * 1.0 / CLOCKTICKS;
    cpufrac = elapcpu / elapse * 100.;
    size = XSIZE * 2. * YSIZE / 1000000.;
    rate = size / elapse;
    printf(" %4.1fMB/%4.1fs(%3.0f) = %5.2fMB/s\n", size, elapse, cpufrac,rate);

    return( *status );
}
/*--------------------------------------------------------------------------*/
int writebintable (fitsfile *fptr, int *status)

    /*********************************************************/
    /* Create a binary table extension containing 3 columns  */
    /*********************************************************/
{
    int hdutype, tfields = 3;
    long nremain, ntodo, firstrow = 1, firstelem = 1;
    long nrows;
    clock_t scpu, ecpu;
    time_t start, finish;
    float rate, size, elapcpu, cpufrac;
    double elapse;

    char extname[] = "Speed_Test";           /* extension name */

    /* define the name, datatype, and physical units for the columns */
    char *ttype[] = { "first", "second", "third" };
    char *tform[] = {"1D",       "1E",      "1I" };
    char *tunit[] = { " ",      " ",       " "   };

    /* append a new empty binary table onto the FITS file */

    if ( fits_create_tbl( fptr, BINARY_TBL, BROWS, tfields, ttype, tform,
                tunit, extname, status) )
         printerror( *status );

    /* get table row size and optimum number of rows to write per loop */
    fits_get_rowsize(fptr, &nrows, status);
    nrows = minvalue(nrows, BUFFSIZE);
    nremain = BROWS;

    printf("Write %7drow x %dcol bintable %4d rows/loop:", BROWS, tfields,
       nrows);

    scpu = clock();
    start = time(0);

    while(nremain)
    {
      ntodo = minvalue(nrows, nremain);
      ffpcld(fptr, 1, firstrow, firstelem, ntodo, darray, status);
      ffpcle(fptr, 2, firstrow, firstelem, ntodo, farray, status);
      ffpcli(fptr, 3, firstrow, firstelem, ntodo, sarray, status);
      firstrow += ntodo;
      nremain -= ntodo;
    }

    ffflus(fptr, status);  /* flush all buffers to disk */

    finish = time(0);
    ecpu = clock();
    elapcpu = (ecpu - scpu) * 1.0 / CLOCKTICKS;
    elapse = difftime(finish, start) + 0.5;
    cpufrac = elapcpu / elapse * 100.;
    size = BROWS * 14. / 1000000.;
    rate = size / elapse;
    printf(" %4.1fMB/%4.1fs(%3.0f) = %5.2fMB/s\n", size, elapse, cpufrac,rate);

    return( *status );
}
/*--------------------------------------------------------------------------*/
int writeasctable (fitsfile *fptr, int *status)

    /*********************************************************/
    /* Create an ASCII table extension containing 2 columns  */
    /*********************************************************/
{
    int hdutype, tfields = 2;
    long nremain, ntodo, firstrow = 1, firstelem = 1;
    long nrows;
    clock_t scpu, ecpu;
    time_t start, finish;
    float rate, size, elapcpu, cpufrac;
    double elapse;

    char extname[] = "Speed_Test";           /* extension name */

    /* define the name, datatype, and physical units for the columns */
    char *ttype[] = { "first", "second" };
    char *tform[] = {"I6",       "I6"   };
    char *tunit[] = { " ",      " "     };

    /* append a new empty ASCII table onto the FITS file */
    if ( fits_create_tbl( fptr, ASCII_TBL, AROWS, tfields, ttype, tform,
                tunit, extname, status) )
         printerror( *status );

    /* get table row size and optimum number of rows to write per loop */
    fits_get_rowsize(fptr, &nrows, status);
    nrows = minvalue(nrows, BUFFSIZE);
    nremain = AROWS;

    printf("Write %7drow x %dcol asctable %4d rows/loop:", AROWS, tfields,
           nrows);

    scpu = clock();
    start = time(0);
    sarray[0] = 32000;

    while(nremain)
    {
      ntodo = minvalue(nrows, nremain);
      ffpcli(fptr, 1, firstrow, firstelem, ntodo, sarray, status);
      ffpcli(fptr, 2, firstrow, firstelem, ntodo, sarray, status);
      firstrow += ntodo;
      nremain -= ntodo;
    }

    ffflus(fptr, status);  /* flush all buffers to disk */

    finish = time(0);
    ecpu = clock();
    elapcpu = (ecpu - scpu) * 1.0 / CLOCKTICKS;
    elapse = difftime(finish, start) + 0.5;
    cpufrac = elapcpu / elapse * 100.;
    size = AROWS * 13. / 1000000.;
    rate = size / elapse;
    printf(" %4.1fMB/%4.1fs(%3.0f) = %5.2fMB/s\n", size, elapse, cpufrac,rate);

    return( *status );
}
/*--------------------------------------------------------------------------*/
int readimage( fitsfile *fptr, int *status )

    /*********************/
    /* Read a FITS image */
    /*********************/
{
    int anynull, hdutype;
    long nremain, ntodo, fpixel = 1;
    short shortnull = 0;
    clock_t scpu, ecpu;
    time_t start, finish;
    float rate, size, elapcpu, cpufrac;
    double elapse;

    /* move to the primary array */
    if ( fits_movabs_hdu(fptr, 1, &hdutype, status) ) 
         printerror( *status );

    printf("\nRead  %dx%d short image, %d pixels/loop:", XSIZE, YSIZE, SHTSIZE);
    nremain = XSIZE * YSIZE;

    scpu = clock();
    start = time(0);

    while(nremain)
    {
      ntodo = minvalue(SHTSIZE, nremain);
      ffgpvi(fptr, 0, fpixel, ntodo, shortnull, array, &anynull, status);
      fpixel += ntodo;
      nremain -= ntodo;
    }

    finish = time(0);
    ecpu = clock();
    elapcpu = (ecpu - scpu) * 1.0 / CLOCKTICKS;
    elapse = difftime(finish, start) + 0.5;
    cpufrac = elapcpu / elapse * 100.;
    size = XSIZE * 2. * YSIZE / 1000000.;
    rate = size / elapse;
    printf(" %4.1fMB/%4.1fs(%3.0f) = %5.2fMB/s\n", size, elapse, cpufrac,rate);

    return( *status );
}
/*--------------------------------------------------------------------------*/
int readbtable( fitsfile *fptr, int *status )

    /************************************************************/
    /* read and print data values from the binary table */
    /************************************************************/
{
    int hdutype, anynull, tfields = 3;
    long nremain, ntodo, firstrow = 1, firstelem = 1;
    long nrows;
    short snull = 0;
    float fnull = 0.;
    double dnull = 0;
    clock_t scpu, ecpu;
    time_t start, finish;
    float rate, size, elapcpu, cpufrac;
    double elapse;

    /* move to the table */
    if ( fits_movrel_hdu(fptr, 1, &hdutype, status) ) 
           printerror( *status );

    /* get table row size and optimum number of rows to read per loop */
    fits_get_rowsize(fptr, &nrows, status);
    nrows = minvalue(nrows, BUFFSIZE);
    
    /*  read the columns */  
    nremain = BROWS;

    printf("Read  %7drow x %dcol bintable %4d rows/loop:", BROWS, tfields,
            nrows);
    scpu = clock();
    start = time(0);

    while(nremain)
    {
      ntodo = minvalue(nrows, nremain);
      ffgcvd(fptr, 1, firstrow, firstelem, ntodo,
                     dnull, darray, &anynull, status);
      ffgcve(fptr, 2, firstrow, firstelem, ntodo,
                     fnull, farray, &anynull, status);
      ffgcvi(fptr, 3, firstrow, firstelem, ntodo,
                     snull, sarray, &anynull, status);
      firstrow += ntodo; 
      nremain  -= ntodo;
    }

    finish = time(0);
    ecpu = clock();
    elapcpu = (ecpu - scpu) * 1.0 / CLOCKTICKS;
    elapse = difftime(finish, start) + 0.5;
    cpufrac = elapcpu / elapse * 100.;
    size = BROWS * 14. / 1000000.;
    rate = size / elapse;
    printf(" %4.1fMB/%4.1fs(%3.0f) = %5.2fMB/s\n", size, elapse, cpufrac,rate);

    return( *status );
}
/*--------------------------------------------------------------------------*/
int readatable( fitsfile *fptr, int *status )

    /************************************************************/
    /* read and print data values from an ASCII or binary table */
    /************************************************************/
{
    int hdutype, anynull, tfields = 2;
    long nremain, ntodo, firstrow = 1, firstelem = 1;
    long nrows;
    short shortnull = 0;
    clock_t scpu, ecpu;
    time_t start, finish;
    float rate, size, elapcpu, cpufrac;
    double elapse;

    /* move to the table */
    if ( fits_movrel_hdu(fptr, 1, &hdutype, status) ) 
           printerror( *status );

    /* get table row size and optimum number of rows to read per loop */
    fits_get_rowsize(fptr, &nrows, status);
    nrows = minvalue(nrows, BUFFSIZE);
 
    /*  read the columns */  
    nremain = AROWS;

    printf("Read  %7drow x %dcol asctable %4d rows/loop:", AROWS, tfields,
           nrows);
    scpu = clock();
    start = time(0);

    while(nremain)
    {
      ntodo = minvalue(nrows, nremain);
      ffgcvi(fptr, 1, firstrow, firstelem, ntodo,
                     shortnull, sarray, &anynull, status);
      ffgcvi(fptr, 2, firstrow, firstelem, ntodo,
                     shortnull, sarray, &anynull, status);
      firstrow += ntodo;
      nremain  -= ntodo;
    }

    finish = time(0);
    ecpu = clock();
    elapcpu = (ecpu - scpu) * 1.0 / CLOCKTICKS;
    elapse = difftime(finish, start) + 0.5;
    cpufrac = elapcpu / elapse * 100.;
    size = AROWS * 13. / 1000000.;
    rate = size / elapse;
    printf(" %4.1fMB/%4.1fs(%3.0f) = %5.2fMB/s\n", size, elapse, cpufrac,rate);

    return( *status );
}
/*--------------------------------------------------------------------------*/
void printerror( int status)
{
    /*****************************************************/
    /* Print out cfitsio error messages and exit program */
    /*****************************************************/

    char status_str[FLEN_STATUS], errmsg[FLEN_ERRMSG];
  
    if (status)
      fprintf(stderr, "\n*** Error occurred during program execution ***\n");

    fits_get_errstatus(status, status_str);   /* get the error description */
    fprintf(stderr, "\nstatus = %d: %s\n", status, status_str);

    /* get first message; null if stack is empty */
    if ( fits_read_errmsg(errmsg) ) 
    {
         fprintf(stderr, "\nError message stack:\n");
         fprintf(stderr, " %s\n", errmsg);

         while ( fits_read_errmsg(errmsg) )  /* get remaining messages */
             fprintf(stderr, " %s\n", errmsg);
    }

    exit( status );       /* terminate the program, returning error status */
}

