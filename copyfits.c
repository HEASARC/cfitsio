#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "fitsio.h"

void printerror( int status);

main()
{
    fitsfile *infptr, *outfptr;       /* pointer to the FITS files */

    int status, nkeys, keypos, hdutype, ii, jj;
    char infile[]  = "-";     /* read FITS file from STDIN */
    char outfile[]  = "-";     /* write FITS file to STDOUT */
    char card[FLEN_CARD];   /* standard string lengths defined in fitsioc.h */

    status = 0;

    if ( fits_open_file(&infptr, infile, READONLY, &status) ) 
         printerror( status );

    if ( fits_create_file(&outfptr, outfile, &status) ) 
         printerror( status );

    /* attempt to move to next HDU, until we get an EOF error */
    for (ii = 1; !(fits_movabs_hdu(infptr, ii, &hdutype, &status) ); ii++) 
    {
        if (ii > 1)
           if (fits_create_hdu(outfptr, &status) )  /* create new extension */
              printerror( status );

        if ( fits_copy_hdu(infptr, outfptr, 0, &status) )
            printerror( status );

        if ( fits_write_history(outfptr, "This is a copy", &status) )
            printerror( status );
    }

    if (status == END_OF_FILE)   /* status values are defined in fitsioc.h */
        status = 0;              /* got the expected EOF error; reset = 0  */
    else
       printerror( status );     /* got an unexpected error                */

    if ( fits_close_file(infptr, &status) )
         printerror( status );
    if ( fits_close_file(outfptr, &status) )
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
