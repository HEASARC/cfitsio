#include <stdio.h>
#include <stdlib.h>
#include "fitsio.h"

int main(int argc, char *argv[])
{
    fitsfile *infptr, *outfptr;   /* FITS file pointers defined in fitsio.h */
    int status = 0, ii = 1, iteration = 0;
    int hdutype, bitpix, bytepix, naxis, nkeys, keytype, datatype, anynul;
    long naxes[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
    long first= 1, totpix, npix;
    double *array, bscale = 1.0, bzero = 0.0, nulval = 0.;
    char card[81];

    if (argc != 3)
    {
 printf("\n");
 printf("Usage:  imcopy inputImage outputImage[compress]\n");
 printf("\n");
 printf("Copy an input image to an output image, optionally compressing\n");
 printf("or uncompressing the image in the process.  If the [compress]\n");
 printf("qualifier is appended to the output file name then the input image\n");
 printf("will be compressed using the tile-compressed format.  In this format,\n");
 printf("the image is divided into rectangular tiles and each tile of pixels\n");
 printf("is compressed and stored in a variable-length row of a binary table.\n");
 printf("If the [compress] qualifier is omitted, and the input image is\n");
 printf("in tile-compressed format, then the output image will be uncompressed.\n");
 printf("\n");
 printf("Examples:\n");
 printf("\n");
 printf("1)  imcopy image.fit 'cimage.fit[compress]'\n");
 printf("\n");
 printf("    This compresses the input image using the default parameters, i.e.,\n");
 printf("    using the Rice compression algorithm and using row by row tiles.\n");
 printf("\n");
 printf("2)  imcopy cimage.fit image2.fit\n");
 printf("\n");
 printf("    This uncompress the image created in the first example.\n");
 printf("    image2.fit should be identical to image.fit if the image\n");
 printf("    has an integer datatype.  There will be small differences\n");
 printf("    in the pixel values if it is a floating point image.\n");
 printf("\n");
 printf("3)  imcopy image.fit 'cimage.fit[compress GZIP 100,100;4]'\n");
 printf("\n");
 printf("    This compresses the input image using the following parameters:\n");
 printf("         GZIP compression algorithm;\n");
 printf("         100 X 100 pixel compression tiles;\n");
 printf("         noise_bits = 4 (only used with floating point images)\n");
 printf("\n");
 printf("The full syntax of the compression qualifier is:\n");
 printf("    [compress ALGORITHM TDIM1,TDIM2,...; NOISE_BITS]\n");
 printf("where the allowed ALGORITHM values are Rice, GZIP, PLIO, \n");
 printf("and TDIMn is the size of the compression tile in each dimension,\n");
 printf("and NOISE_BITS = 1, 2, 3, or 4 and controls the amount of noise\n");
 printf("suppression when compressing floating point images. \n");
 printf("\n");
 printf("Note that it may be necessary to enclose the file names\n");
 printf("in single quote characters on the Unix command line.\n");
      return(0);
    }

    /* Open the input file */
    if (fits_open_file(&infptr, argv[1], READONLY, &status) ) {
        fits_report_error(stderr, status);
        return(status);
    }

    fits_get_hdu_type(infptr, &hdutype, &status);
    if (hdutype != IMAGE_HDU) {
       printf("ERROR: input is not a FITS image\n");
       return(1);
    }

    /* get image dimensions */
    fits_get_img_param(infptr, 9, &bitpix, &naxis, naxes, &status);

    if (naxis == 0)  { /* null image; look for image in next extension */
        fits_movrel_hdu(infptr, 1, &hdutype, &status);
        if (status || hdutype != IMAGE_HDU)  {
            printf("ERROR: Couldn't find input image extension\n");
            return(1);
        }

        /* get image dimensions */
        fits_get_img_param(infptr, 9, &bitpix, &naxis, naxes, &status);

        if (naxis == 0)   {
            printf("ERROR: Couldn't find input image extension\n");
            return(1);
        }
    }

    totpix = naxes[0] * naxes[1] * naxes[2] * naxes[3] * naxes[4]
           * naxes[5] * naxes[6] * naxes[7] * naxes[8];

    if (totpix == 0) {
        printf("Image contains 0 pixels; not copied\n");
        return(1);
    }

    /* Create the output file */
    if (fits_create_file(&outfptr, argv[2], &status) ) {
        fits_report_error(stderr, status);
        return(status);
    }

    /* Explicitly create new image, to support compression */
    /* Otherwise, we could have just called fits_copy_hdu */
    fits_create_img(outfptr, bitpix, naxis, naxes, &status);

    /* copy all the user keywords (not the structural keywords) */
    fits_get_hdrspace(infptr, &nkeys, NULL, &status); 

    for (ii = 1; ii <= nkeys; ii++) {
        fits_read_record(infptr, ii, card, &status);
        if (fits_get_keyclass(card) > TYP_CMPRS_KEY)
            fits_write_record(outfptr, card, &status);
    }

    switch(bitpix) {
        case BYTE_IMG:
                  datatype = TBYTE;
                  break;
        case SHORT_IMG:
                  datatype = TSHORT;
                  break;
        case LONG_IMG:
                  datatype = TLONG;
                  break;
        case FLOAT_IMG:
                  datatype = TFLOAT;
                  break;
        case DOUBLE_IMG:
                  datatype = TDOUBLE;
                  break;
    }

    bytepix = abs(bitpix) / 8;

    npix = totpix;
    iteration = 0;

    /* try to allocate memory for the entire image */
    /* use double type to force memory alignment */
    array = (double *) calloc(npix, bytepix);

    /* if allocation failed, divide size by 2 and try again */
    while (!array && iteration < 10)  {
        iteration++;
        npix = npix / 2;
        array = (double *) calloc(npix, bytepix);
    }

    if (!array)  {
        printf("Memory allocation error\n");
        return(0);
    }

    /* turn off any scaling so that we read and write the raw pixel values */
    fits_set_bscale(infptr,  bscale, bzero, &status);
    fits_set_bscale(outfptr, bscale, bzero, &status);

    while (totpix > 0 && !status)
    {
       /* read all or part of image then write it back to the output file */
       fits_read_img(infptr, datatype, first, npix, 
                     &nulval, array, &anynul, &status);

       fits_write_img(outfptr, datatype, first, npix, array, &status);
       totpix = totpix - npix;
       first  = first  + npix;
    }
    free(array);

    fits_close_file(outfptr,  &status);
    fits_close_file(infptr, &status);

    /* if error occurred, print out error message */
    if (status)
       fits_report_error(stderr, status);
    return(status);
}
