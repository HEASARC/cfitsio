#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "fitsio.h"

void writeimage( void );
void writeascii( void );
void writebintable( void );
void copyhdu( void );
void selectrows( void );
void readheader( void );
void readimage( void );
void readtable( void );
void printerror( int status);

main()
{
/*************************************************************************
   This is a simple main program that calls the following routines:

    writeimage    - write a FITS primary array image
    writeascii    - write a FITS ASCII table extension
    writebintable - write a FITS binary table extension
    copyhdu       - copy a header/data unit from one FITS file to another
    selectrows    - copy selected row from one HDU to another
    readheader    - read and print the header keywords in every extension
    readimage     - read a FITS image and compute the min and max value
    readtable     - read columns of data from ASCII and binary tables

**************************************************************************/

    writeimage();
    writeascii();
    writebintable();
    copyhdu();
    selectrows();
    readheader();
    readimage();
    readtable();

    printf("\nAll the cfitsio cookbook routines ran successfully.\n");
    return(0);
}
/*--------------------------------------------------------------------------*/
void writeimage( void )

    /******************************************************/
    /* Create a FITS primary array containing a 2-D image */
    /******************************************************/
{
    fitsfile *fptr;                               /* pointer to a FITS file */
    int status, ii, jj;
    long group, fpixel, nelements, array[200][300];

    /* initialize FITS image parameters */
    char filename[] = "atestfil.fit";            /* name for new FITS file */
    int simple   = TRUE;
    int bitpix   =  16;   /* 16-bit short signed integer pixel values       */
    long naxis    =   2;  /* 2-dimensional image                            */    
    long naxes[2] = { 300, 200 };   /* image is 300 pixels wide by 200 rows */
    long pcount   =   0;  /* no group parameters */
    long gcount   =   1;  /* only a single image/group */
    int extend   = TRUE;  /* there may be additional extension in the file  */
    
    remove(filename);               /* Delete old file if it already exists */

    status = 0;         /* initialize status before calling fitsio routines */

    if ( ffinit(&fptr, filename, &status) )     /* create the new FITS file */
         printerror( status );           /* call printerror if error occurs */

    if ( ffphpr(fptr, simple, bitpix, naxis, /* write the required keywords */
         naxes, pcount, gcount, extend, &status) )
         printerror( status );           /* call printerror if error occurs */

    /* initialize the values in the image with a linear ramp function */
    for (jj = 0; jj < naxes[1]; jj++)
    {   for (ii = 0; ii < naxes[0]; ii++)
        {
            array[jj][ii] = ii + jj;
        }
    }
    /* write the array to the FITS file */
    group  = 0;                                /* group to write            */
    fpixel = 1;                                /* first pixel to write      */
    nelements = naxes[0] * naxes[1];           /* number of pixels to write */

    if ( ffpprj(fptr, group, fpixel, nelements, array[0], &status) )
        printerror( status );            /* call printerror if error occurs */

    /* write another optional keyword to the header */
    if ( ffpkyj(fptr, "EXPOSURE", 1500, "Total Exposure Time", &status) )
         printerror( status );           /* call printerror if error occurs */

    if ( ffclos(fptr, &status) )                          /* close the file */
         printerror( status );           /* call printerror if error occurs */

    return;
}
/*--------------------------------------------------------------------------*/
void writeascii ( void )

    /*******************************************************************/
    /* Create an ASCII table extension containing 3 columns and 6 rows */
    /*******************************************************************/
{
    fitsfile *fptr;                            /* pointer to the FITS file */
    int status;
    long rowlen, tbcol[3], firstrow, firstelem;

    int tfields = 3;       /* table will have 3 columns */
    long nrows  = 6;       /* table will have 6 rows    */
    int nspace  = 1;       /* leave 1 space between columns */

    char filename[] = "atestfil.fit";           /* name for new FITS file */
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

    /* open the FITS primary array */
    if ( ffopen(&fptr, filename, READWRITE, &status) ) 
         printerror( status );

    /* create new extension following primary array */
    if ( ffcrhd(fptr, &status) )
         printerror( status );
 
    /* calculate the starting position of each column, and the row length */
    if ( ffgabc( tfields, tform, nspace, &rowlen, tbcol, &status) )
         printerror( status );

    /* write the required header parameters for the ASCII table */
    if ( ffphtb( fptr, rowlen, nrows, tfields, ttype, tbcol, tform, tunit,
                 extname, &status) )
         printerror( status );

    firstrow  = 1;  /* first row in table to write   */
    firstelem = 1;  /* first element in row  (ignored in ASCII tables) */

    /* write names to the first column (character strings) */
    /* write diameters to the second column (longs) */
    /* write density to the third column (floats) */

    ffpcls(fptr, 1, firstrow, firstelem, nrows, planet,   &status);
    ffpclj(fptr, 2, firstrow, firstelem, nrows, diameter, &status);
    ffpcle(fptr, 3, firstrow, firstelem, nrows, density,  &status);

    if ( ffclos(fptr, &status) )                /* close the FITS file */
         printerror( status );
    return;
}
/*--------------------------------------------------------------------------*/
void writebintable ( void )

    /*******************************************************************/
    /* Create a binary table extension containing 3 columns and 6 rows */
    /*******************************************************************/
{
    fitsfile *fptr;                            /* pointer to the FITS file */
    int status, hdutype;
    long firstrow, firstelem;

    int tfields   = 3;       /* table will have 3 columns */
    long nrows    = 6;       /* table will have 6 rows    */
    long varidata = 0;       /* no variable length data; heap size = 0 */

    char filename[] = "atestfil.fit";           /* name for new FITS file */
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

    /* open the FITS primary array */
    if ( ffopen(&fptr, filename, READWRITE, &status) ) 
         printerror( status );

    if ( ffmahd(fptr, 2, &hdutype, &status) )            /* move to 2nd HDU */
         printerror( status );

    if ( ffcrhd(fptr, &status) )                    /* create new extension */
         printerror( status );
 
    /* write the required header parameters for the ASCII table */
    if ( ffphbn( fptr, nrows, tfields, ttype, tform, tunit,
                 extname, varidata, &status) )
         printerror( status );

    firstrow  = 1;  /* first row in table to write   */
    firstelem = 1;  /* first element in row  (ignored in ASCII tables) */

    /* write names to the first column (character strings) */
    /* write diameters to the second column (longs) */
    /* write density to the third column (floats) */

    ffpcls(fptr, 1, firstrow, firstelem, nrows, planet,   &status);
    ffpclj(fptr, 2, firstrow, firstelem, nrows, diameter, &status);
    ffpcle(fptr, 3, firstrow, firstelem, nrows, density,  &status);

    if ( ffclos(fptr, &status) )                /* close the FITS file */
         printerror( status );
    return;
}
/*--------------------------------------------------------------------------*/
void copyhdu( void)
{
    /********************************************************************/
    /* copy the 1st and 3rd HDUs from the input file to a new FITS file */
    /********************************************************************/
    fitsfile *infptr;                  /* pointer to the existing FITS file */
    fitsfile *outfptr;                 /* pointer to the new FITS file      */

    char infilename[]  = "atestfil.fit";  /* name for existing FITS file   */
    char outfilename[] = "btestfil.fit";  /* name for new FITS file        */

    int status, morekeys, hdutype;

    status = 0;

    remove(outfilename);            /* Delete old file if it already exists */

    /* open the existing FITS file */
    if ( ffopen(&infptr, infilename, READONLY, &status) ) 
         printerror( status );

    if ( ffinit(&outfptr, outfilename, &status) )   /* create new FITS file */
         printerror( status );           /* call printerror if error occurs */

    /* copy the primary array from the input file to the output file */
    morekeys = 0;     /* don't reserve space for additional keywords */
    if ( ffcopy(infptr, outfptr, morekeys, &status) )
         printerror( status );
    if ( ffcrhd(outfptr, &status) )  /* create new extension in output file */
         printerror( status );
 
    /* move to the 3rd HDU in the input file */
    if ( ffmahd(infptr, 3, &hdutype, &status) )
         printerror( status );

    /* copy 3rd HDU from the input file to the output file (to 2nd HDU) */
    if ( ffcopy(infptr, outfptr, morekeys, &status) )
         printerror( status );

    if (ffclos(outfptr, &status) || ffclos(infptr, &status)) /* close files */
         printerror( status );

    return;
}
/*--------------------------------------------------------------------------*/
void selectrows( void )

    /*********************************************************************/
    /* select rows from an input table and copy them to the output table */
    /*********************************************************************/

{
    fitsfile *infptr, *outfptr;  /* pointer to input and output FITS files */
    unsigned char *buffer;
    char card[FLEN_CARD];
    int status, hdutype, nkeys, keypos, nfound, colnum, anynulls, ii;
    long naxes[2], frow, felem, noutrows, irow;
    float nullval, density[6];

    char infilename[]  = "atestfil.fit";  /* name for existing FITS file   */
    char outfilename[] = "btestfil.fit";  /* name for new FITS file        */

    status = 0;

    /* open the existing FITS files */
    if ( ffopen(&infptr,  infilename,  READONLY,  &status) ||
         ffopen(&outfptr, outfilename, READWRITE, &status) ) 
         printerror( status );

    /* move to the 3rd HDU in the input file (a binary table in this case) */
    if ( ffmahd(infptr, 3, &hdutype, &status) )
         printerror( status );

    if (hdutype != HDU_BTABLE)  {
        printf("Error: expected to find a binary table in this HDU\n");
        return;
    }
    /* move to the last (2rd) HDU in the output file */
    if ( ffmahd(outfptr, 2, &hdutype, &status) )
         printerror( status );

    /* create new extension in the output file */
    if ( ffcrhd(outfptr, &status) )
         printerror( status );

    if (ffghps(infptr, &nkeys, &keypos, &status) ) /* get no. of keywords */
        printerror( status );

    /* copy all the keywords from the input to the output extension */
    for (ii = 1; ii <= nkeys; ii++)  {
        ffgrec(infptr, ii, card, &status);  /* read keyword from input */
        ffprec(outfptr,    card, &status);  /* write keyword to output */
    }
    /* read the NAXIS1 and NAXIS2 keyword to get table size */
    if ( ffgknj(infptr, "NAXIS", 1, 2, naxes, &nfound, &status) )
         printerror( status );

    /* find which column contains the DENSITY values */
    if ( ffgcno(infptr, CASEINSEN, "density", &colnum, &status) )
         printerror( status );

    /* read the DENSITY column values */
    frow = 1;
    felem = 1;
    nullval = -99.;
    if (ffgcve(infptr, colnum, frow, felem, naxes[1], nullval, density,
               &anynulls, &status) )
        printerror( status );

    /* allocate buffer large enough for 1 row of the table */
    buffer = (unsigned char *) malloc(naxes[0]);

    /* If the density is less than 3.0, copy the row to the output table */
    for (noutrows = 0, irow = 1; irow <= naxes[1]; irow++)  {
      if (density[irow - 1] < 3.0)  {
         noutrows++;
         ffgtbb(infptr,  irow,     1, naxes[0], buffer, &status); 
         ffptbb(outfptr, noutrows, 1, naxes[0], buffer, &status); 
    } }

    /* update the NAXIS2 keyword with the correct number of rows */
    if ( ffmkyj(outfptr, "NAXIS2", noutrows, "&", &status) )
         printerror( status );

    if (ffclos(outfptr, &status) || ffclos(infptr, &status)) /* close files */
        printerror( status );

    return;
}
/*--------------------------------------------------------------------------*/
void readheader ( void )

    /**********************************************************************/
    /* Print out all the header keywords in all extensions of a FITS file */
    /**********************************************************************/
{
    fitsfile *fptr;                             /* pointer to the FITS file */

    int status, nkeys, keypos, hdutype, ii, jj;
    char filename[]  = "btestfil.fit";     /* name of existing FITS file   */
    char card[FLEN_CARD];   /* standard string lengths defined in fitsioc.h */

    status = 0;

    if ( ffopen(&fptr, filename, READONLY, &status) ) 
         printerror( status );

    /* attempt to move to next HDU, until we get an EOF error */
    for (ii = 1; !(ffmahd(fptr, ii, &hdutype, &status) ); ii++) 
    {
        if (ffghps(fptr, &nkeys, &keypos, &status) ) /* get no. of keywords */
            printerror( status );

        printf("Header listing for HDU #%d:\n", ii);
        for (jj = 1; jj <= nkeys; jj++)  {
            if ( ffgrec(fptr, jj, card, &status) )
                 printerror( status );

            printf("%s\n", card); /* print the keyword card */
        }
        printf("END\n\n");  /* terminate listing with END */
    }

    if (status == END_OF_FILE)   /* status values are defined in fitsioc.h */
        status = 0;              /* got the expected EOF error; reset = 0  */
    else
       printerror( status );     /* got an unexpected error                */

    if ( ffclos(fptr, &status) )                    /* close the FITS file */
         printerror( status );

    return;
}
/*--------------------------------------------------------------------------*/
void readimage( void )

    /************************************************************************/
    /* Read a FITS image and determine the minimum and maximum pixel values */
    /************************************************************************/
{
    fitsfile *fptr;                             /* pointer to the FITS file */
    int status,  nfound, anynull;
    long naxes[2], group, fpixel, nbuffer, npixels, ii;

#define buffsize 1000
    float datamin, datamax, nullval, buffer[buffsize];
    char filename[]  = "atestfil.fit";     /* name of existing FITS file   */

    status = 0;

    if ( ffopen(&fptr, filename, READONLY, &status) )     /* open the image */
         printerror( status );

    /* read the NAXIS1 and NAXIS2 keyword to get image size */
    if ( ffgknj(fptr, "NAXIS", 1, 2, naxes, &nfound, &status) )
         printerror( status );

    npixels  = naxes[0] * naxes[1];         /* number of pixels in the image */
    group    = 1;
    fpixel   = 1;
    nullval  = 0;                /* don't check for null values in the image */
    datamin  = 1.0E30;
    datamax  = -1.0E30;

    while (npixels > 0)
    {
      nbuffer = npixels;
      if (npixels > buffsize)
        nbuffer = buffsize;     /* read as many pixels as will fit in buffer */

      if ( ffgpve(fptr, group, fpixel, nbuffer, nullval,
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

    if ( ffclos(fptr, &status) )                    /* close the FITS file */
         printerror( status );

    return;
}
/*--------------------------------------------------------------------------*/
void readtable( void )

    /************************************************************/
    /* read and print data values from an ASCII or binary table */
    /************************************************************/
{
    fitsfile *fptr;                             /* pointer to the FITS file */
    int status, hdutype,  nfound, anynull, ii;
    long hdunum, frow, felem, nelem, longnull, dia[6];
    float floatnull, den[6];
    char strnull[10], *name[6], *ttype[3]; 

    char filename[]  = "atestfil.fit";     /* name of existing FITS file   */

    status = 0;

    if ( ffopen(&fptr, filename, READONLY, &status) )     /* open the image */
         printerror( status );

    for (ii = 0; ii < 3; ii++)      /* allocate space for the column labels */
        ttype[ii] = (char *) malloc(FLEN_VALUE);  /* max label length = 69 */

    for (ii = 0; ii < 6; ii++)    /* allocate space for string column value */
        name[ii] = (char *) malloc(10);   

    for (hdunum = 2; hdunum <= 3; hdunum++) /*read ASCII, then binary table */
    {
      if ( ffmahd(fptr, hdunum, &hdutype, &status) ) /* move to the HDU */
           printerror( status );

      if (hdutype == HDU_ATABLE)
          printf("\nReading ASCII table in HDU %d:\n",  hdunum);
      else if (hdutype == HDU_BTABLE)
          printf("\nReading binary table in HDU %d:\n", hdunum);
      else
      {
          printf("Error: this HDU is not an ASCII or binary table\n");
          printerror( status );
      }

      /* read the column names from the TTYPEn keywords */
      ffgkns(fptr, "TTYPE", 1, 3, ttype, &nfound, &status);

      printf(" Row  %10s %10s %10s\n", ttype[0], ttype[1], ttype[2]);

      frow      = 1;
      felem     = 1;
      nelem     = 6;
      strcpy(strnull, " ");
      longnull  = 0;
      floatnull = 0.;

      /*  read the columns */  
      ffgcvs(fptr, 1, frow, felem, nelem, strnull,  name, &anynull, &status);
      ffgcvj(fptr, 2, frow, felem, nelem, longnull,  dia, &anynull, &status);
      ffgcve(fptr, 3, frow, felem, nelem, floatnull, den, &anynull, &status);

      for (ii = 0; ii < 6; ii++)
        printf("%5d %10s %10d %10.2f\n", ii + 1, name[ii], dia[ii], den[ii]);
    }

    for (ii = 0; ii < 3; ii++)      /* free the memory for the column labels */
        free( ttype[ii] );

    for (ii = 0; ii < 6; ii++)      /* free the memory for the string column */
        free( name[ii] );

    if ( ffclos(fptr, &status) )                    /* close the FITS file */
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

    ffgerr(status, status_str);        /* get the error status description */
    fprintf(stderr, "\nstatus = %d: %s\n", status, status_str);

    if ( ffgmsg(errmsg) )  /* get first message; null if stack is empty */
    {
         fprintf(stderr, "\nError message stack:\n");
         fprintf(stderr, " %s\n", errmsg);

         while ( ffgmsg(errmsg) )  /* get remaining messages */
             fprintf(stderr, " %s\n", errmsg);
    }

    exit( status );       /* terminate the program, returning error status */
}
