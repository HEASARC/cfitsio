#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "fitsio.h"

void printerror( int status);
int main(int argc, char *argv[])
{
    fitsfile *infptr, *outfptr;  /* pointer to the FITS files */

    int status, imgtype, compress_type = 0, blocksize, nbits;
    char infile[FLEN_FILENAME];     
    char outfile[FLEN_FILENAME];
    long tilesize[2], origsize, outsize;

    status = 0;

    if (argc != 6 && argc != 4 && argc != 3 )
    {
        printf("Usage:\n\n");
        printf("   compress_fits infile outfile compress_type xtilesize ytilesize\n\n");
        printf(" where,\n");
        printf("   infile  = name of input FITS image file to be compressed\n");
        printf("   outfile = name of output compressed FITS image file \n");
        printf("   compress_type = r (RICE, default), g (GZIP), or p (PLIO) \n");
        printf("   xtilesize = x dimension of tiles   (default = NAXIS1)\n");
        printf("   ytilesize = y dimension of tiles   (default = 1)\n\n");
        printf(" Example 1: compress infile using RICE algorithm and default tile size:\n\n");
        printf("        compress_fits infile.fits outfile.fits\n\n");
        printf(" Example 2: compress infile using GZIP algorithm and 64x64 pixel tile size:\n\n");
        printf("        compress_fits infile.fits outfile.fits g 64 64\n\n");
        exit(0);
    }

    strcpy(infile, argv[1] );   /* name of file to compress */
    strcpy(outfile, argv[2] );   /* name of output file */

    /*  open input image  */
    if ( fits_open_file(&infptr, infile, READONLY, &status) ) 
         printerror( status );

    /*  get image dimensions */
    fits_get_img_size(infptr, 2, tilesize, &status);
    printf("image dimensions = %ld x %ld\n", tilesize[0], tilesize[1]);

    fits_get_img_type(infptr, &imgtype, &status);
    printf("imgtype = %d\n", imgtype);

    origsize = tilesize[0] * tilesize[1] * abs(imgtype) / 8;

    /*  parse command-line arguments */
    if (argc == 3)
    {
        tilesize[1] = 1;
        compress_type = RICE_1;
    }
    else if (argc == 4)
    {
         tilesize[1] = 1;
         if (*argv[3] == 'r')
             compress_type = RICE_1;
         else if (*argv[3] == 'g')
             compress_type = GZIP_1;

         else if (*argv[3] == 'p')
             compress_type = PLIO_1;
         else
         {
             printf("unknown compression type: %s\n", argv[3]);
             exit(1);
         }
    }
    else if (argc == 5)
    {
          tilesize[0] = atoi(argv[3]);
          tilesize[1] = atoi(argv[4]);
    }
    else 
    {
         if (*argv[3] == 'r')
             compress_type = RICE_1;
         else if (*argv[3] == 'g')
             compress_type = GZIP_1;

         else if (*argv[3] == 'p')
             compress_type = PLIO_1;
         else
         {
             printf("unknown compression type: %s\n", argv[3]);
             exit(1);
         }

          tilesize[0] = atoi(argv[4]);
          tilesize[1] = atoi(argv[5]);
    }

    printf("tile dimensions  = %ld x %ld\n", tilesize[0], tilesize[1]);

    if (compress_type == RICE_1)
          printf("using RICE compression algorithm\n");
    else if (compress_type == GZIP_1)
          printf("using GZIP compression algorithm\n");
    else if (compress_type == PLIO_1)
          printf("using PLIO compression algorithm\n");

    /* create output file */
    if ( fits_create_file(&outfptr, outfile, &status) ) 
         printerror( status );

    /* compress the image */
    blocksize = 32; /* RICE compression parameter */
    nbits = 4;      /* compression parameter, used for float images */
    
    if ( fits_comp_img(infptr, outfptr, compress_type, tilesize,
         blocksize, nbits, &status) ) 
         printerror( status );

    ffgkyj(outfptr, "PCOUNT", &outsize, NULL, &status);

    printf("  insize = %ld\n outsize = %ld\n",
            origsize, outsize);

    if (outsize)
       printf("   ratio = %.2f\n", (double) origsize / outsize);

    /* close the files */
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
