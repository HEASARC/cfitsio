#include <stdio.h>
#include "fitsio.h"

int main(int argc, char *argv[])
{
    fitsfile *infptr, *outfptr;
    int status = 0, hdutype, ii;
    char infile[FLEN_FILENAME],outfile[FLEN_FILENAME]; 

    if (argc != 3)
    {
        printf("Usage: fitscopy inputfile outputfile\n");
        return(1);
    }

    strcpy(infile,  argv[1] );  
    strcpy(outfile, argv[2] );  

    if ( fits_open_file(&infptr, infile, READONLY, &status) )
    {
         fits_report_error(stderr, status);
         return(status);
    }

    if ( fits_create_file(&outfptr, outfile, &status) )
    {
         fits_close_file(infptr, &status);
         fits_report_error(stderr, status);
         return(status);
    }

    /* attempt to move to next HDU, until we get an EOF error */
    for (ii = 1; !(fits_movabs_hdu(infptr, ii, &hdutype, &status) ); ii++) 
    {
        if ( fits_copy_hdu(infptr, outfptr, 0, &status) )
        {
            fits_report_error(stderr, status);
            break;
        }
    }

    if (status == END_OF_FILE)   
        status = 0;              /* got the expected EOF error; reset = 0  */

    fits_close_file(infptr,  &status);
    fits_close_file(outfptr, &status);

    if (status)
        fits_report_error(stderr, status);

    return(0);
}
