/*
  This program reads and writes FITS files in memory.
  At the end, it copies the memory file out to a disk file
  called 'testmem.fit'.
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
/* stddef.h is apparently needed to define size_t */
#include <stddef.h>
#include "fitsio.h"

void writeimage( void **memptr, size_t *initsize, size_t deltasize);
void writeascii( void **memptr, size_t *initsize, size_t deltasize );
void writebintable( void **memptr, size_t *initsize, size_t deltasize );
void readheader( void **memptr, size_t *initsize, size_t deltasize );
void readimage( void **memptr, size_t *initsize, size_t deltasize );
void readtable( void **memptr, size_t *initsize, size_t deltasize );
void printerror( int status);

main()
{
/*************************************************************************
   This is a simple main program that calls the following routines:

   THIS VERSION WRITES AND READS THE FILES IN CORE MEMORY, NOT DISK

    writeimage    - write a FITS primary array image
    writeascii    - write a FITS ASCII table extension
    writebintable - write a FITS binary table extension
    readheader    - read and print the header keywords in every extension
    readimage     - read a FITS image and compute the min and max value
    readtable     - read columns of data from ASCII and binary tables

**************************************************************************/
    void *memptr;
    size_t memsize,deltasize;
    char outfile[] = "testmem.fit";
    int status;

    /* allocate a chunk of memory for the FITS file */
    memsize = 2880;
    deltasize = 2880;
    memptr = malloc(memsize); 


    writeimage(&memptr, &memsize, deltasize);
    writeascii(&memptr, &memsize, deltasize);
    writebintable(&memptr, &memsize, deltasize);
    readheader(&memptr, &memsize, deltasize);
    readimage(&memptr, &memsize, deltasize);
    readtable(&memptr, &memsize, deltasize);

    remove(outfile);   /* delete old version of disk file, if it exists */

    /* write the memory file to a disk file */
    status = 0;
    fits_write_mem_buff(memptr, memsize, outfile, &status);

    free(memptr);

    if (status > 0)
       printerror( status );

    printf("\nThe cfitsio memory routines ran successfully.\n");
    return(0);
}
/*--------------------------------------------------------------------------*/
void writeimage( void **memptr, size_t *initsize, size_t deltasize )

    /******************************************************/
    /* Create a FITS primary array containing a 2-D image */
    /******************************************************/
{
    fitsfile *fptr;       /* pointer to the FITS file, defined in fitsio.h */
    int status, ii, jj;
    long  fpixel, nelements, exposure;
    unsigned short array[200][300];

    /* initialize FITS image parameters */
    char filename[] = " ";             /* name for new FITS file */
    int bitpix   =  USHORT_IMG; /* 16-bit unsigned short pixel values       */
    long naxis    =   2;  /* 2-dimensional image                            */    
    long naxes[2] = { 300, 200 };   /* image is 300 pixels wide by 200 rows */

    status = 0;         /* initialize status before calling fitsio routines */

    /*  Define the memory buffer that is to be used */
    fits_set_mem_buff(&fptr, memptr, initsize, deltasize, realloc, &status);

    if (fits_create_file(&fptr, filename, &status)) /* create new FITS file */
         printerror( status );           /* call printerror if error occurs */

    /* write the required keywords for the primary array image.     */
    /* Since bitpix = USHORT_IMG, this will cause cfitsio to create */
    /* a FITS image with BITPIX = 16 (signed short integers) with   */
    /* BSCALE = 1.0 and BZERO = 32768.  This is the convention that */
    /* FITS uses to store unsigned integers.  Note that the BSCALE  */
    /* and BZERO keywords will be automatically written by cfitsio  */
    /* in this case.                                                */

    if ( fits_create_img(fptr,  bitpix, naxis, naxes, &status) )
         printerror( status );          

    /* initialize the values in the image with a linear ramp function */
    for (jj = 0; jj < naxes[1]; jj++)
    {   for (ii = 0; ii < naxes[0]; ii++)
        {
            array[jj][ii] = ii + jj;
        }
    }

    fpixel = 1;                               /* first pixel to write      */
    nelements = naxes[0] * naxes[1];          /* number of pixels to write */

    /* write the array of unsigned integers to the FITS file */
    if ( fits_write_img(fptr, TUSHORT, fpixel, nelements, array[0], &status) )
        printerror( status );            

    /* write another optional keyword to the header */
    /* Note that the ADDRESS of the value is passed in the routine */
    exposure = 1500.;
    if ( fits_write_key(fptr, TLONG, "EXPOSURE", &exposure,
         "Total Exposure Time", &status) )
         printerror( status );           

    if ( fits_close_file(fptr, &status) )                /* close the file */
         printerror( status );           

    return;
}
/*--------------------------------------------------------------------------*/
void writeascii (  void **memptr, size_t *initsize, size_t deltasize )

    /*******************************************************************/
    /* Create an ASCII table extension containing 3 columns and 6 rows */
    /*******************************************************************/
{
    fitsfile *fptr;       /* pointer to the FITS file, defined in fitsio.h */
    int status;
    long firstrow, firstelem;

    int tfields = 3;       /* table will have 3 columns */
    long nrows  = 6;       /* table will have 6 rows    */

    char filename[] = " ";           /* name for new FITS file */
    char extname[] = "PLANETS_ASCII";             /* extension name */

    /* define the name, datatype, and physical units for the 3 columns */
    char *ttype[] = { "Planet", "Diameter", "Density" };
    char *tform[] = { "a8",     "I6",       "F4.2"    };
    char *tunit[] = { " ",      "km",       "g/cm"    };

    /* define the name diameter, and density of each planet */
    char *planet[] = {"Mercury", "Venus", "Earth", "Mars","Jupiter","Saturn"};
    long  diameter[] = {4880,    12112,    12742,   6800,  143000,   121000};
    float density[]  = { 5.1,     5.3,      5.52,   3.94,    1.33,    0.69};

    status=0;

    /*  Define the memory buffer that is to be used */
    fits_set_mem_buff(&fptr, memptr, initsize, deltasize, realloc, &status);

    /* open with write access the FITS file containing a primary array */
    if ( fits_open_file(&fptr, filename, READWRITE, &status) ) 
         printerror( status );

    /* append a new empty ASCII table onto the FITS file */
    if ( fits_create_tbl( fptr, ASCII_TBL, nrows, tfields, ttype, tform,
                tunit, extname, &status) )
         printerror( status );

    firstrow  = 1;  /* first row in table to write   */
    firstelem = 1;  /* first element in row  (ignored in ASCII tables) */

    /* write names to the first column (character strings) */
    /* write diameters to the second column (longs) */
    /* write density to the third column (floats) */

    fits_write_col(fptr, TSTRING, 1, firstrow, firstelem, nrows, planet,
                   &status);
    fits_write_col(fptr, TLONG, 2, firstrow, firstelem, nrows, diameter,
                   &status);
    fits_write_col(fptr, TFLOAT, 3, firstrow, firstelem, nrows, density,
                   &status);

    if ( fits_close_file(fptr, &status) )       /* close the FITS file */
         printerror( status );
    return;
}
/*--------------------------------------------------------------------------*/
void writebintable (  void **memptr, size_t *initsize, size_t deltasize )

    /*******************************************************************/
    /* Create a binary table extension containing 3 columns and 6 rows */
    /*******************************************************************/
{
    fitsfile *fptr;       /* pointer to the FITS file, defined in fitsio.h */
    int status, hdutype;
    long firstrow, firstelem;

    int tfields   = 3;       /* table will have 3 columns */
    long nrows    = 6;       /* table will have 6 rows    */

    char filename[] = " ";           /* name for new FITS file */
    char extname[] = "PLANETS_Binary";           /* extension name */

    /* define the name, datatype, and physical units for the 3 columns */
    char *ttype[] = { "Planet", "Diameter", "Density" };
    char *tform[] = { "8a",     "1J",       "1E"    };
    char *tunit[] = { " ",      "km",       "g/cm"    };

    /* define the name diameter, and density of each planet */
    char *planet[] = {"Mercury", "Venus", "Earth", "Mars","Jupiter","Saturn"};
    long  diameter[] = {4880,     12112,   12742,   6800,  143000,   121000};
    float density[]  = { 5.1,      5.3,     5.52,   3.94,   1.33,     0.69};

    status=0;

    /*  Define the memory buffer that is to be used */
    fits_set_mem_buff(&fptr, memptr, initsize, deltasize, realloc, &status);

    /* open the FITS file containing a primary array and an ASCII table */
    if ( fits_open_file(&fptr, filename, READWRITE, &status) ) 
         printerror( status );

    if ( fits_movabs_hdu(fptr, 2, &hdutype, &status) ) /* move to 2nd HDU */
         printerror( status );

    /* append a new empty binary table onto the FITS file */
    if ( fits_create_tbl( fptr, BINARY_TBL, nrows, tfields, ttype, tform,
                tunit, extname, &status) )
         printerror( status );

    firstrow  = 1;  /* first row in table to write   */
    firstelem = 1;  /* first element in row  (ignored in ASCII tables) */

    /* write names to the first column (character strings) */
    /* write diameters to the second column (longs) */
    /* write density to the third column (floats) */

    fits_write_col(fptr, TSTRING, 1, firstrow, firstelem, nrows, planet,
                   &status);
    fits_write_col(fptr, TLONG, 2, firstrow, firstelem, nrows, diameter,
                   &status);
    fits_write_col(fptr, TFLOAT, 3, firstrow, firstelem, nrows, density,
                   &status);

    if ( fits_close_file(fptr, &status) )       /* close the FITS file */
         printerror( status );
    return;
}
/*--------------------------------------------------------------------------*/
void readheader (  void **memptr, size_t *initsize, size_t deltasize )

    /**********************************************************************/
    /* Print out all the header keywords in all extensions of a FITS file */
    /**********************************************************************/
{
    fitsfile *fptr;       /* pointer to the FITS file, defined in fitsio.h */

    int status, nkeys, keypos, hdutype, ii, jj;
    char filename[]  = " ";     /* name of existing FITS file   */
    char card[FLEN_CARD];   /* standard string lengths defined in fitsioc.h */

    status = 0;

    /*  Define the memory buffer that is to be used */
    fits_set_mem_buff(&fptr, memptr, initsize, deltasize, realloc, &status);

    if ( fits_open_file(&fptr, filename, READONLY, &status) ) 
         printerror( status );

    /* attempt to move to next HDU, until we get an EOF error */
    for (ii = 1; !(fits_movabs_hdu(fptr, ii, &hdutype, &status) ); ii++) 
    {
        /* get no. of keywords */
        if (fits_get_hdrpos(fptr, &nkeys, &keypos, &status) )
            printerror( status );

        printf("Header listing for HDU #%d:\n", ii);
        for (jj = 1; jj <= nkeys; jj++)  {
            if ( fits_read_record(fptr, jj, card, &status) )
                 printerror( status );

            printf("%s\n", card); /* print the keyword card */
        }
        printf("END\n\n");  /* terminate listing with END */
    }

    if (status == END_OF_FILE)   /* status values are defined in fitsioc.h */
    {
        status = 0;              /* got the expected EOF error; reset = 0  */
        ffcmsg();                /* clear the EOF error message            */
    }
    else
       printerror( status );     /* got an unexpected error                */

    if ( fits_close_file(fptr, &status) )
         printerror( status );

    return;
}
/*--------------------------------------------------------------------------*/
void readimage(  void **memptr, size_t *initsize, size_t deltasize )

    /************************************************************************/
    /* Read a FITS image and determine the minimum and maximum pixel values */
    /************************************************************************/
{
    fitsfile *fptr;       /* pointer to the FITS file, defined in fitsio.h */
    int status,  nfound, anynull;
    long naxes[2], fpixel, nbuffer, npixels, ii;

#define buffsize 1000
    float datamin, datamax, nullval, buffer[buffsize];
    char filename[]  = " ";     /* name of existing FITS file   */

    status = 0;

    /*  Define the memory buffer that is to be used */
    fits_set_mem_buff(&fptr, memptr, initsize, deltasize, realloc, &status);

    if ( fits_open_file(&fptr, filename, READONLY, &status) )
         printerror( status );

    /* read the NAXIS1 and NAXIS2 keyword to get image size */
    if ( fits_read_keys_lng(fptr, "NAXIS", 1, 2, naxes, &nfound, &status) )
         printerror( status );

    npixels  = naxes[0] * naxes[1];         /* number of pixels in the image */
    fpixel   = 1;
    nullval  = 0;                /* don't check for null values in the image */
    datamin  = 1.0E30;
    datamax  = -1.0E30;

    while (npixels > 0)
    {
      nbuffer = npixels;
      if (npixels > buffsize)
        nbuffer = buffsize;     /* read as many pixels as will fit in buffer */

      /* Note that even though the FITS images contains unsigned integer */
      /* pixel values (or more accurately, signed integer pixels with    */
      /* a bias of 32768),  this routine is reading the values into a    */
      /* float array.   Cfitsio automatically performs the datatype      */
      /* conversion in cases like this.                                  */

      if ( fits_read_img(fptr, TFLOAT, fpixel, nbuffer, &nullval,
                  buffer, &anynull, &status) )
           printerror( status );

      for (ii = 0; ii < nbuffer; ii++)  {
        if ( buffer[ii] < datamin )
            datamin = buffer[ii];

        if ( buffer[ii] > datamax )
            datamax = buffer[ii];
      }
      npixels -= nbuffer;    /* increment remaining number of pixels */
      fpixel  += nbuffer;    /* next pixel to be read in image */
    }

    printf("\nMin and max image pixels =  %.0f, %.0f\n", datamin, datamax);

    if ( fits_close_file(fptr, &status) )
         printerror( status );

    return;
}
/*--------------------------------------------------------------------------*/
void readtable(  void **memptr, size_t *initsize, size_t deltasize )

    /************************************************************/
    /* read and print data values from an ASCII or binary table */
    /************************************************************/
{
    fitsfile *fptr;       /* pointer to the FITS file, defined in fitsio.h */
    int status, hdutype,  nfound, anynull, ii;
    long hdunum, frow, felem, nelem, longnull, dia[6];
    float floatnull, den[6];
    char strnull[10], *name[6], *ttype[3]; 

    char filename[]  = " ";     /* name of existing FITS file   */

    status = 0;

    /*  Define the memory buffer that is to be used */
    fits_set_mem_buff(&fptr, memptr, initsize, deltasize, realloc, &status);

    if ( fits_open_file(&fptr, filename, READONLY, &status) )
         printerror( status );

    for (ii = 0; ii < 3; ii++)      /* allocate space for the column labels */
        ttype[ii] = (char *) malloc(FLEN_VALUE);  /* max label length = 69 */

    for (ii = 0; ii < 6; ii++)    /* allocate space for string column value */
        name[ii] = (char *) malloc(10);   

    for (hdunum = 2; hdunum <= 3; hdunum++) /*read ASCII, then binary table */
    {
      /* move to the HDU */
      if ( fits_movabs_hdu(fptr, hdunum, &hdutype, &status) ) 
           printerror( status );

      if (hdutype == ASCII_TBL)
          printf("\nReading ASCII table in HDU %d:\n",  hdunum);
      else if (hdutype == BINARY_TBL)
          printf("\nReading binary table in HDU %d:\n", hdunum);
      else
      {
          printf("Error: this HDU is not an ASCII or binary table\n");
          printerror( status );
      }

      /* read the column names from the TTYPEn keywords */
      fits_read_keys_str(fptr, "TTYPE", 1, 3, ttype, &nfound, &status);

      printf(" Row  %10s %10s %10s\n", ttype[0], ttype[1], ttype[2]);

      frow      = 1;
      felem     = 1;
      nelem     = 6;
      strcpy(strnull, " ");
      longnull  = 0;
      floatnull = 0.;

      /*  read the columns */  
      fits_read_col(fptr, TSTRING, 1, frow, felem, nelem, strnull,  name,
                    &anynull, &status);
      fits_read_col(fptr, TLONG, 2, frow, felem, nelem, &longnull,  dia,
                    &anynull, &status);
      fits_read_col(fptr, TFLOAT, 3, frow, felem, nelem, &floatnull, den,
                    &anynull, &status);

      for (ii = 0; ii < 6; ii++)
        printf("%5d %10s %10d %10.2f\n", ii + 1, name[ii], dia[ii], den[ii]);
    }

    for (ii = 0; ii < 3; ii++)      /* free the memory for the column labels */
        free( ttype[ii] );

    for (ii = 0; ii < 6; ii++)      /* free the memory for the string column */
        free( name[ii] );

    if ( fits_close_file(fptr, &status) ) 
         printerror( status );

    return;
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
