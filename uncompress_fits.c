#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "fitsio.h"

void printerror( int status);
int main(int argc, char *argv[])
{
    fitsfile *infptr, *outfptr;  /* pointer to the FITS files */
    int status = 0;
    char infile[FLEN_FILENAME];     
    char outfile[FLEN_FILENAME];

    if (argc != 3)
    {
        printf("Usage:  uncompress_fits infile+ext outfile \n");
        printf("  where,\n");
        printf("   infile  = name of FITS image file to be uncompressed\n");
        printf("   +ext    = number of the FITS extension \n");
        printf("   outfile = name of output uncompressed FITS image file \n");
        exit(0);
    }

    strcpy(infile, argv[1] );   /* name of file to uncompress */
    strcpy(outfile, argv[2] );   /* name of output file */


    if ( fits_open_file(&infptr, infile, READONLY, &status) ) 
         printerror( status );

    /* decompress the entire image and write to new output image */
    if ( fits_create_file(&outfptr, outfile, &status) ) 
         printerror( status );

    if ( fits_decomp_img(infptr, outfptr, &status) ) 
         printerror( status );

    fits_close_file(infptr, &status);

    if ( fits_close_file(outfptr, &status) )
         printerror( status );

    exit(0);
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
