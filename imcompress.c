# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <math.h>
# include "fitsio2.h"

/*--------------------------------------------------------------------------*/
int fits_comp_img(fitsfile *infptr, /* pointer to image to be compressed */
                 fitsfile *outfptr, /* empty HDU for output compressed image */
                 int compress_type, /* compression type code               */
                                    /*  RICE_1, HCOMPRESS_1, etc.          */
                 long *intilesize,    /* size in each dimension of the tiles */
                                    /* NULL pointer means tile by rows */
		 int blocksize,     /* compression parameter: blocksize  */
                 int nbits,         /* compression parameter: nbits  */
                 int *status)       /* IO - error status               */

/*
   This routine initializes the output table, copies all the keywords,
   and  loops through the input image, compressing the data and
   writing the compressed tiles to the output table.
*/
{
    int ii, bitpix, naxis;
    long naxes[MAX_COMPRESS_DIM], tilesize[MAX_COMPRESS_DIM];

    if (*status > 0)
        return(*status);

    /* get datatype and size of input image */
    if (fits_get_img_param(infptr, MAX_COMPRESS_DIM, &bitpix, 
                       &naxis, naxes, status) > 0)
        return(*status);

    if (naxis < 1 || naxis > MAX_COMPRESS_DIM)
    {
        ffpmsg("Image cannot be compressed: NAXIS out of range");
        return(*status = BAD_NAXIS);
    }

    /* determine tile size */
    if (intilesize == NULL)   /* caller did not specify tile size? */
    {
        /* default case; compress each row of the image separately */
        tilesize[0] = naxes[0];
        for (ii = 1; ii < naxis; ii++)
        {
            tilesize[ii] = 1;
        }
    }
    else
    {
        /* limit max tile size in each dimension to size of dimension */
        for (ii = 0; ii < naxis; ii++)
        {
            tilesize[ii] = minvalue(intilesize[ii], naxes[ii]);
        }
    }
    /* initialize output table */
    if (imcomp_init_table(outfptr, compress_type, bitpix, naxis, naxes,
        tilesize, blocksize, nbits, status) > 0)
        return (*status);

    /* Copy the image header keywords to the table header. */
    if (imcomp_copy_imheader(infptr, outfptr, status) > 0)
	    return (*status);

    /* turn off any intensity scaling (defined by BSCALE and BZERO */
    /* keywords) so that unscaled values will be read by CFITSIO */
    ffpscl(infptr, 1.0, 0.0, status);

    /* force a rescan of the output file keywords, so that */
    /* the compression parameters will be copied to the internal */
    /* fitsfile structure used by CFITSIO */
    ffrdef(outfptr, status);

    /* Read each image tile, compress, and write to a table row. */
    imcomp_compress_image (infptr, outfptr, status);

    /* force another rescan of the output file keywords, to */
    /* update PCOUNT and TFORMn = '1PB(iii)' keyword values. */
    ffrdef(outfptr, status);

    return (*status);
}
/*--------------------------------------------------------------------------*/
int imcomp_init_table(fitsfile *outfptr,
        int compress_type,
        int bitpix,
        int naxis,
        long *naxes,
        long *tilesize, 
        int rice_blocksize,
        int rice_nbits,
        int *status) 

/* 
  create a BINTABLE extension for the output compressed image.
*/
{
    char keyname[FLEN_KEYWORD], zcmptype[12];
    int ii, ncols;
    long nrows;
    char *ttype[] = {"COMPRESSED_DATA", "UNCOMPRESSED_DATA", "ZSCALE", "ZZERO"};
    char *tform[4];
    char tf0[4], tf1[4], tf2[4], tf3[4];
    char *tunit[] = {"\0",            "\0",            "\0",      "\0"    };

    if (*status > 0)
        return(*status);

    /* set up array of TFORM strings */
    strcpy(tf0, "1PB");
    strcpy(tf2, "1D");
    strcpy(tf3, "1D");

    tform[0] = tf0;
    tform[1] = tf1;
    tform[2] = tf2;
    tform[3] = tf3;

    /* calculate number of rows in output table */
    nrows = 1;
    for (ii = 0; ii < naxis; ii++)
    {
        nrows = nrows * ((naxes[ii] - 1)/ tilesize[ii] + 1);
    }

    if (bitpix < 0 )  /* floating point image */
        ncols = 4;
    else
        ncols = 1; /* default table has just one 'COMPRESSED_DATA' column */

    if (compress_type == RICE_1)
    {
        strcpy(zcmptype, "RICE_1");
    }
    else if (compress_type == GZIP_1)
    {
        strcpy(zcmptype, "GZIP_1");
    }
    else if (compress_type == PLIO_1)
    {
        strcpy(zcmptype, "PLIO_1");
       /* the PLIO compression algorithm outputs short integers, not bytes */
       strcpy(tform[0], "1PI");
    }
    else if (compress_type == HCOMPRESS_1)
    {
        strcpy(zcmptype, "HCOMPRESS_1");
    }
    else
    {
        ffpmsg("unknown compression type (fits_comp_img)");
        return(*status = DATA_COMPRESSION_ERR);
    }

    /* set correct datatype for any tiles that cannot be compressed */
    if (bitpix == SHORT_IMG)
       strcpy(tform[1], "1PI");
    else if (bitpix == LONG_IMG)
       strcpy(tform[1], "1PJ");
    else if (bitpix == FLOAT_IMG)
       strcpy(tform[1], "1PE");
    else if (bitpix == DOUBLE_IMG)
       strcpy(tform[1], "1PD");

    /* create the bintable extension to contain the compressed image */
    ffcrtb(outfptr, BINARY_TBL, nrows, ncols, ttype, 
                tform, tunit, "COMPRESSED_IMAGE", status);

    /* Add standard header keywords. */
    ffpkyl (outfptr, "ZIMAGE", 1, 
           "extension contains compressed image", status);                  
    
    ffpkyj (outfptr, "ZBITPIX", (long) bitpix,
			"data type of original image", status);
    ffpkyj (outfptr, "ZNAXIS", (long) naxis,
			"dimension of original image", status);

    for (ii = 0;  ii < naxis;  ii++)
    {
        sprintf (keyname, "ZNAXIS%d", ii+1);
        ffpkyj (outfptr, keyname, naxes[ii],
			"length of original image axis", status);
    }

    for (ii = 0;  ii < naxis;  ii++)
    {
        sprintf (keyname, "ZTILE%d", ii+1);
        ffpkyj (outfptr, keyname, tilesize[ii],
			"size of tiles to be compressed", status);
    }

    ffpkys (outfptr, "ZCMPTYPE", zcmptype,
	          "compression algorithm", status);


    /* write any algorithm-specific keywords */
    if (compress_type == RICE_1)
    {
        ffpkys (outfptr, "ZNAME1", "BLOCKSIZE",
            "compression block size", status);
        ffpkyj (outfptr, "ZVAL1", (long) rice_blocksize,
			"pixels per block", status);

        if (bitpix < 0 )  /* floating point image */
        {
            ffpkys (outfptr, "ZNAME2", "NOISEBIT",
                "floating point quantization level", status);
            ffpkyj (outfptr, "ZVAL2", (long) rice_nbits,
                "floating point quantization level", status);
        }
    }
    else
    {
        if (bitpix < 0 )  /* floating point image */
        {
            ffpkys (outfptr, "ZNAME1", "NOISEBIT",
                "floating point quantization level", status);
            ffpkyj (outfptr, "ZVAL1", (long) rice_nbits,
                "floating point quantization level", status);
        }
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int imcomp_calc_max_elem (int comptype, int nx, int blocksize)

/* This function returns the maximum number of bytes in a compressed
   image line.
*/
{
    if (comptype == RICE_1)
    {
        return (sizeof(float) * nx + nx / blocksize + 2 + 4);
    }
    else if (comptype == GZIP_1)
    {
        /* gzip usually compressed by at least a factor of 2 */
        /* If this size turns out to be too small, then the gzip */
        /* compression routine will allocate more space as required */

        return(nx * sizeof(int) / 2);
    }
    else
        return(nx * sizeof(int));
}
/*--------------------------------------------------------------------------*/
int imcomp_compress_image (fitsfile *infptr, fitsfile *outfptr, int *status)

/* This routine does the following:
        - reads an image one tile at a time
        - if it is a float or double image, then it quantizes the pixels
        - compresses the integer pixel values
        - writes the compressed byte stream to the FITS file.

   If the tile cannot be quantized than the raw float or double values
   are written to the output table.

*/
{
    double *tiledata = 0;
    int anynul, gotnulls = 0, datatype, tstatus, colnum;
    long ii, row, nelem, offset;
    int naxis;
    long maxtilelen, tilelen, incre[] = {1, 1, 1, 1, 1, 1};
    long naxes[MAX_COMPRESS_DIM], fpixel[MAX_COMPRESS_DIM];
    long lpixel[MAX_COMPRESS_DIM], tile[MAX_COMPRESS_DIM];
    long tilesize[MAX_COMPRESS_DIM];
    long i0, i1, i2, i3, i4, i5;
    char card[FLEN_CARD];

    if (*status > 0)
        return(*status);

    maxtilelen = (outfptr->Fptr)->maxtilelen;

    /* allocate buffer to hold 1 tile of data */
    if ((outfptr->Fptr)->zbitpix == FLOAT_IMG)
    {
        datatype = TFLOAT;
        tiledata = (double*) calloc (maxtilelen, sizeof (float));
    }
    else if ((outfptr->Fptr)->zbitpix == DOUBLE_IMG)
    {
        datatype = TDOUBLE;
        tiledata = (double*) calloc (maxtilelen, sizeof (double));
    }
    else
    {
        datatype = TINT;
        tiledata = (double*) calloc (maxtilelen, sizeof (int));
    }

    if (tiledata == NULL)
    {
	ffpmsg("Out of memory. (imcomp_compress_image)");
	return (*status = MEMORY_ALLOCATION);
    }

    /*  calculate size of tile in each dimension */
    naxis = (outfptr->Fptr)->zndim;
    for (ii = 0; ii < MAX_COMPRESS_DIM; ii++)
    {
        if (ii < naxis)
        {
             naxes[ii] = (outfptr->Fptr)->znaxis[ii];
             tilesize[ii] = (outfptr->Fptr)->tilesize[ii];
        }
        else
        {
            naxes[ii] = 1;
            tilesize[ii] = 1;
        }
    }
    row = 1;

    /* set up big loop over up to 6 dimensions */
    for (i5 = 1; i5 <= naxes[5]; i5 += tilesize[5])
    {
     fpixel[5] = i5;
     lpixel[5] = minvalue(i5 + tilesize[5] - 1, naxes[5]);
     tile[5] = lpixel[5] - fpixel[5] + 1;
     for (i4 = 1; i4 <= naxes[4]; i4 += tilesize[4])
     {
      fpixel[4] = i4;
      lpixel[4] = minvalue(i4 + tilesize[4] - 1, naxes[4]);
      tile[4] = lpixel[4] - fpixel[4] + 1;
      for (i3 = 1; i3 <= naxes[3]; i3 += tilesize[3])
      {
       fpixel[3] = i3;
       lpixel[3] = minvalue(i3 + tilesize[3] - 1, naxes[3]);
       tile[3] = lpixel[3] - fpixel[3] + 1;
       for (i2 = 1; i2 <= naxes[2]; i2 += tilesize[2])
       {
        fpixel[2] = i2;
        lpixel[2] = minvalue(i2 + tilesize[2] - 1, naxes[2]);
        tile[2] = lpixel[2] - fpixel[2] + 1;
        for (i1 = 1; i1 <= naxes[1]; i1 += tilesize[1])
        {
         fpixel[1] = i1;
         lpixel[1] = minvalue(i1 + tilesize[1] - 1, naxes[1]);
         tile[1] = lpixel[1] - fpixel[1] + 1;
         for (i0 = 1; i0 <= naxes[0]; i0 += tilesize[0])
         {
          fpixel[0] = i0;
          lpixel[0] = minvalue(i0 + tilesize[0] - 1, naxes[0]);
          tile[0] = lpixel[0] - fpixel[0] + 1;

          /* number of pixels in this tile */
          tilelen = tile[0];
          for (ii = 1; ii < naxis; ii++)
          {
             tilelen *= tile[ii];
          }

          /* read next tile of data from image */
          if (datatype == TFLOAT)
          {
              ffgsve(infptr, 1, naxis, naxes, fpixel, lpixel, incre, 
                  FLOATNULLVALUE, (float *) tiledata,  &anynul, status);
          }
          else if (datatype == TDOUBLE)
          {
              ffgsvd(infptr, 1, naxis, naxes, fpixel, lpixel, incre, 
                  DOUBLENULLVALUE, tiledata, &anynul, status);
          }
          else  /* read all integer data types as int */
          {
              ffgsvk(infptr, 1, naxis, naxes, fpixel, lpixel, incre, 
                  0, (int *) tiledata,  &anynul, status);
          }

          /* now compress the tile, and write to row of binary table */

          imcomp_compress_tile(outfptr, row, datatype, tiledata, tilelen,
                               status);

          /* set flag if we found any null values */
          if (anynul)
              gotnulls = 1;

          /* check for any error in the previous operations */
          if (*status > 0)
          {
              ffpmsg("Error writing compressed image to table");
              free(tiledata);
              return (*status);
          }

	  row++;
         }
        }
       }
      }
     }
    }

    free (tiledata);  /* finished with this buffer */

    /* insert ZBLANK keyword if necessary */
    if (gotnulls)
    {
          ffgcrd(outfptr, "ZCMPTYPE", card, status);
          ffikyj(outfptr, "ZBLANK", COMPRESS_NULL_VALUE, 
             "null value in the compressed integer array", status);
    }

    if (datatype >= TFLOAT )
    {
       /* check if any data were written to the UNCOMPRESSED_DATA column */
       /* If not, then delete that column from the table */
       for (ii = 1; ii < row; ii++)
       {
          ffgdes (outfptr, (outfptr->Fptr)->cn_uncompressed, ii, 
                  &nelem, &offset, status);
          if (nelem)
              break;   
       }

       if (!nelem)
       {
           tstatus = 0;
           ffgcno(outfptr, CASEINSEN, "UNCOMPRESSED_DATA", &colnum, &tstatus);
           if (tstatus == 0)
           {
              /* make sure table is properly terminated before deleting col */
               /* (in particular, make sure the '1PB(nnn)' keyword is updated */
               ffrdef(outfptr, status);
               ffdcol(outfptr, colnum, status);
           }
       }
    }

    return (*status);
}
/*--------------------------------------------------------------------------*/
int imcomp_compress_tile (fitsfile *outfptr,
    long row, 
    int datatype, 
    void *tiledata, 
    long tilelen,
    int *status)

/*
   This is the main compression routine.

   This routine does the following to the input tile of pixels:
        - if it is a float or double image, then it quantizes the pixels
        - compresses the integer pixel values
        - writes the compressed byte stream to the FITS file.

   If the tile cannot be quantized than the raw float or double values
   are written to the output table.
*/
{
    int *idata;		/* quantized integer data */
    short *cbuf;	/* compressed data */
    int clen;		/* size of cbuf */
    int flag = 1;		/* true if data were quantized */
    int iminval = 0, imaxval = 0;  /* min and max quantized integers */
    double bscale[1], bzero[1];	/* scaling parameters */
    int  nelem = 0;		/* number of bytes */
    size_t gzip_nelem = 0;

    if (*status > 0)
        return(*status);

    idata = tiledata;

    /* if floating point data, allocate a buffer for the quantized values */
    if (datatype >= TFLOAT)
    {
        idata = (int*) calloc (tilelen, sizeof (int));
        if (idata == NULL)
        {
	    ffpmsg("Out of memory. (imcomp_compress_tile)");
	    return (*status = MEMORY_ALLOCATION);
        }

        if (datatype == TFLOAT)
        {
            /* quantize the float values into integers */
            flag = fits_quantize_float ((float *) tiledata, tilelen,
               FLOATNULLVALUE, (outfptr->Fptr)->rice_nbits, idata,
               bscale, bzero, &iminval, &imaxval);
        }
        else if (datatype == TDOUBLE)
        {
            /* quantize the double values into integers */
            flag = fits_quantize_double ((double *) tiledata, tilelen,
               DOUBLENULLVALUE, (outfptr->Fptr)->rice_nbits, idata,
               bscale, bzero, &iminval, &imaxval);
        }
    }

    if (flag)
    {
        /* allocate buffer for the compressed tile bytes */
        clen = (outfptr->Fptr)->maxelem;
        cbuf = (short *) calloc (clen, sizeof (unsigned char));
        if (cbuf == NULL)
        {
            ffpmsg("Out of memory. (imcomp_compress_tile)");
            if (datatype >= TFLOAT)
                free(idata);
	    return (*status = MEMORY_ALLOCATION);
        }

	/* Compress the integer data, then write the compressed bytes */
        if ( (outfptr->Fptr)->compress_type == RICE_1)
        {
  	        nelem = fits_rcomp (idata, tilelen, (unsigned char *) cbuf,
                       clen, (outfptr->Fptr)->rice_blocksize);

	        /* Write the compressed byte stream. */
                ffpclb(outfptr, (outfptr->Fptr)->cn_compressed, row, 1,
                     nelem, (unsigned char *) cbuf, status);
        }
        else if ( (outfptr->Fptr)->compress_type == PLIO_1)
        {
                if (iminval < 0 || imaxval > 16777215)
                {
                   /* plio algorithn only supports positive 24 bit ints */
                   ffpmsg("data out of range for PLIO compression (0 - 2**24)");
                   return(*status = DATA_DECOMPRESSION_ERR);
                }

  	        nelem = pl_p2li (idata, 1, cbuf, tilelen);

	        /* Write the compressed byte stream. */
                ffpcli(outfptr, (outfptr->Fptr)->cn_compressed, row, 1,
                     nelem, cbuf, status);
        }
        else if ( (outfptr->Fptr)->compress_type == GZIP_1)
        {

#if BYTESWAPPED
               ffswap4(idata, tilelen); /* reverse order of bytes */
#endif
                compress2mem_from_mem((char *) idata, tilelen * sizeof(int),
                 (char **) &cbuf, (size_t *) &clen, realloc, 
                 &gzip_nelem, status);

	        /* Write the compressed byte stream. */
                ffpclb(outfptr, (outfptr->Fptr)->cn_compressed, row, 1,
                     gzip_nelem, (unsigned char *) cbuf, status);
        }
        else if ( (outfptr->Fptr)->compress_type == HCOMPRESS_1)
        {
        }

	if (nelem < 0)  /* error condition */
        {
            if (datatype > TFLOAT)
               free(idata);
	    free (cbuf);
            ffpmsg
                ("error compressing row of the image (imcomp_compress_tile)");
            return (*status = DATA_COMPRESSION_ERR);
        }

        if (datatype >= TFLOAT)
        {
              /* write the linear scaling parameters */
	      ffpcld (outfptr, (outfptr->Fptr)->cn_zscale, row, 1, 1,
                      bscale, status);
	      ffpcld (outfptr, (outfptr->Fptr)->cn_zzero,  row, 1, 1,
                      bzero,  status);
         }

         free(cbuf);  /* finished with this buffer */
    } 
    else     /* floating point data couldn't be quantized */
    {
	 /* Write the original floating point data. */
         if (datatype == TFLOAT)
         {
              ffpcle (outfptr, (outfptr->Fptr)->cn_uncompressed, row, 1,
                      tilelen, (float *)tiledata, status);
         }
         else if (datatype == TDOUBLE)
         {
              ffpcld (outfptr, (outfptr->Fptr)->cn_uncompressed, row, 1,
                      tilelen, (double *)tiledata, status);
         }
    }

    if (datatype >= TFLOAT)
       free(idata);

    return (*status);
}
/* ######################################################################## */
/* ###                 Image Decompression Routines                     ### */
/* ######################################################################## */

/*--------------------------------------------------------------------------*/
int fits_decomp_img (fitsfile *infptr, /* image (bintable) to be uncompressed */
              fitsfile *outfptr,   /* empty HDU for output uncompressed image */
              int *status)         /* IO - error status               */

/* 
  This routine decompresses the whole image and writes it to the output file.
*/

{
    double *data;
    int ii, datatype = 0, byte_per_pix = 0;
    int nullcheck, anynul;
    long fpixel[MAX_COMPRESS_DIM], lpixel[MAX_COMPRESS_DIM];
    long inc[MAX_COMPRESS_DIM];
    long imgsize, memsize;
    float *nulladdr, fnulval;
    double dnulval;

    if (*status > 0)
        return(*status);

    if (!fits_is_compressed_image(infptr, status) )
    {
        ffpmsg("CHDU is not a compressed image (fits_decomp_img)");
        return(*status = DATA_DECOMPRESSION_ERR);
    }

    /* create an empty output image with the correct dimensions */
    if (ffcrim(outfptr, (infptr->Fptr)->zbitpix, (infptr->Fptr)->zndim, 
       (infptr->Fptr)->znaxis, status) > 0)
    {
        ffpmsg("error creating output decompressed image HDU");
    	return (*status);
    }
    /* Copy the table header to the image header. */
    if (imcomp_copy_imheader(infptr, outfptr, status) > 0)
    {
        ffpmsg("error copying header of compressed image");
    	return (*status);
    }

    /* force a rescan of the output header keywords, then reset the scaling */
    /* in case the BSCALE and BZERO keywords are present, so that the       */
    /* decompressed values won't be scaled when written to the output image */
    ffrdef(outfptr, status);
    ffpscl(outfptr, 1.0, 0.0, status);

    /* initialize; no null checking is needed for integer images */
    nullcheck = 0;
    nulladdr =  &fnulval;

    /* determine datatype for image */
    if ((infptr->Fptr)->zbitpix == BYTE_IMG)
    {
        datatype = TBYTE;
        byte_per_pix = 1;
    }
    else if ((infptr->Fptr)->zbitpix == SHORT_IMG)
    {
        datatype = TSHORT;
        byte_per_pix = sizeof(short);
    }
    else if ((infptr->Fptr)->zbitpix == LONG_IMG)
    {
        datatype = TINT;
        byte_per_pix = sizeof(int);
    }
    else if ((infptr->Fptr)->zbitpix == FLOAT_IMG)
    {
        /* In the case of float images we must check for NaNs  */
        nullcheck = 1;
        fnulval = FLOATNULLVALUE;
        nulladdr =  &fnulval;
        datatype = TFLOAT;
        byte_per_pix = sizeof(float);
    }
    else if ((infptr->Fptr)->zbitpix == DOUBLE_IMG)
    {
        /* In the case of double images we must check for NaNs  */
        nullcheck = 1;
        dnulval = DOUBLENULLVALUE;
        nulladdr = (float *) &dnulval;
        datatype = TDOUBLE;
        byte_per_pix = sizeof(double);
    }

    /* calculate size of the image (in pixels) */
    imgsize = 1;
    for (ii = 0; ii < (infptr->Fptr)->zndim; ii++)
    {
        imgsize *= (infptr->Fptr)->znaxis[ii];
        fpixel[ii] = 1;              /* Set first and last pixel to */
        lpixel[ii] = (infptr->Fptr)->znaxis[ii]; /* include the entire image. */
        inc[ii] = 1;
    }
    /* Calc equivalent number of double pixels same size as whole the image. */
    /* We use double datatype to force the memory to be aligned properly */
    memsize = ((imgsize * byte_per_pix) - 1) / sizeof(double) + 1;

    /* allocate memory for the image */
    data = (double*) calloc (memsize, sizeof(double));
    if (!data)
    { 
        ffpmsg("Couldn't allocate memory for the uncompressed image");
        return(*status = MEMORY_ALLOCATION);
    }

    /* uncompress the entire image into memory */
    /* This routine should be enhanced sometime to only need enough */
    /* memory to uncompress one tile at a time.  */

    fits_read_compressed_img(infptr, datatype, fpixel, lpixel, inc,  
            nullcheck, nulladdr, data, NULL, &anynul, status);

    /* write the image to the output file */
    if (anynul)
        fits_write_imgnull(outfptr, datatype, 1, imgsize, data, nulladdr, 
                          status);
    else
        fits_write_img(outfptr, datatype, 1, imgsize, data, status);

    free(data);
    return (*status);
}
/*---------------------------------------------------------------------------*/
int fits_read_compressed_img(fitsfile *fptr,   /* I - FITS file pointer      */
            int  datatype,  /* I - datatype of the array to be returned      */
            long  *fpixel,  /* I - 'bottom left corner' of the subsection    */
            long  *lpixel,  /* I - 'top right corner' of the subsection      */
            long  *inc,     /* I - increment to be applied in each dimension */
            int  nullcheck,  /* I - 0 for no null checking                   */
                              /*     1: set undefined pixels = nullval       */
                              /*     2: set nullarray=1 for undefined pixels */
            void *nullval,    /* I - value for undefined pixels              */
            void *array,      /* O - array of values that are returned       */
            char *nullarray,  /* O - array of flags = 1 if nullcheck = 2     */
            int  *anynul,     /* O - set to 1 if any values are null; else 0 */
            int  *status)     /* IO - error status                           */
/*
   Read a section of a compressed image;  Note: lpixel may be larger than the 
   size of the uncompressed image.  Only the pixels within the image will be
   returned.
*/
{
    int naxis[MAX_COMPRESS_DIM], tiledim[MAX_COMPRESS_DIM];
    long tilesize[MAX_COMPRESS_DIM], thistilesize[MAX_COMPRESS_DIM];
    long ftile[MAX_COMPRESS_DIM], ltile[MAX_COMPRESS_DIM];
    long tfpixel[MAX_COMPRESS_DIM], tlpixel[MAX_COMPRESS_DIM];
    long rowdim[MAX_COMPRESS_DIM], offset[MAX_COMPRESS_DIM],ntemp;
    int ii, i5, i4, i3, i2, i1, i0, ndim, irow, pixlen, tilenul;
    void *buffer;
    char *bnullarray = 0;

    if (*status > 0) 
        return(*status);

    buffer = inc; /* dummy statement to stop warning about unused variable */

    if (!fits_is_compressed_image(fptr, status) )
    {
        ffpmsg("CHDU is not a compressed image (fits_decomp_img)");
        return(*status = DATA_DECOMPRESSION_ERR);
    }

    /* get temporary space for uncompressing one image tile */
    if (datatype == TSHORT)
    {
       buffer =  calloc ((fptr->Fptr)->maxtilelen, sizeof (short));
       pixlen = sizeof(short);
    }
    else if (datatype == TINT)
    {
       buffer =  calloc ((fptr->Fptr)->maxtilelen, sizeof (int));
       pixlen = sizeof(int);
    }
    else if (datatype == TLONG)
    {
       buffer =  calloc ((fptr->Fptr)->maxtilelen, sizeof (long));
       pixlen = sizeof(long);
    }
    else if (datatype == TFLOAT)
    {
       buffer =  calloc ((fptr->Fptr)->maxtilelen, sizeof (float));
       pixlen = sizeof(float);
    }
    else if (datatype == TDOUBLE)
    {
       buffer =  calloc ((fptr->Fptr)->maxtilelen, sizeof (double));
       pixlen = sizeof(double);
    }
    else if (datatype == TUSHORT)
    {
       buffer =  calloc ((fptr->Fptr)->maxtilelen, sizeof (unsigned short));
       pixlen = sizeof(short);
    }
    else if (datatype == TUINT)
    {
       buffer =  calloc ((fptr->Fptr)->maxtilelen, sizeof (unsigned int));
       pixlen = sizeof(int);
    }
    else if (datatype == TULONG)
    {
       buffer =  calloc ((fptr->Fptr)->maxtilelen, sizeof (unsigned long));
       pixlen = sizeof(long);
    }
    else if (datatype == TBYTE)
    {
       buffer =  calloc ((fptr->Fptr)->maxtilelen, sizeof (char));
       pixlen = 1;
    }
    else
    {
        ffpmsg("unsupported datatype for uncompressing image");
        return(*status = BAD_DATATYPE);
    }

    if (buffer == NULL)
    {
	    ffpmsg("Out of memory (fits_read_compress_img)");
	    return (*status = MEMORY_ALLOCATION);
    }
    /* allocate memory for a null flag array, if needed */
    if (nullcheck == 2)
    {
        bnullarray = calloc ((fptr->Fptr)->maxtilelen, sizeof (char));

        if (bnullarray == NULL)
        {
	    ffpmsg("Out of memory (fits_read_compress_img)");
            free(buffer);
	    return (*status = MEMORY_ALLOCATION);
        }
    }

    /* initialize all the arrays */
    for (ii = 0; ii < MAX_COMPRESS_DIM; ii++)
    {
        naxis[ii] = 1;
        tiledim[ii] = 1;
        tilesize[ii] = 1;
        ftile[ii] = 1;
        ltile[ii] = 1;
        rowdim[ii] = 1;
    }

    ndim = (fptr->Fptr)->zndim;
    ntemp = 1;
    for (ii = 0; ii < ndim; ii++)
    {
        /* calc number of tiles in each dimension, and tile containing */
        /* the first and last pixel we want to read in each dimension  */
        naxis[ii] = (fptr->Fptr)->znaxis[ii];
        if (fpixel[ii] < 1)
        {
            if (nullcheck == 2)
            {
                free(bnullarray);
            }
            free(buffer);
            return(*status = BAD_PIX_NUM);
        }

        tilesize[ii] = (fptr->Fptr)->tilesize[ii];
        tiledim[ii] = (naxis[ii] - 1) / tilesize[ii] + 1;
        ftile[ii]   = (fpixel[ii] - 1)   / tilesize[ii] + 1;
        ltile[ii]   = minvalue((lpixel[ii] - 1) / tilesize[ii] + 1, 
                                tiledim[ii]);
        rowdim[ii]  = ntemp;  /* total tiles in each dimension */
        ntemp *= tiledim[ii];
    }

    *anynul = 0;  /* initialize */

    /* support up to 6 dimensions for now */
    /* tfpixel and tlpixel are the first and last image pixels */
    /* along each dimension of the compression tile */
    for (i5 = ftile[5]; i5 <= ltile[5]; i5++)
    {
     tfpixel[5] = (i5 - 1) * tilesize[5] + 1;
     tlpixel[5] = minvalue(tfpixel[5] + tilesize[5] - 1, 
                            naxis[5]);
     thistilesize[5] = tlpixel[5] - tfpixel[5] + 1;
     offset[5] = (i5 - 1) * rowdim[5];
     for (i4 = ftile[4]; i4 <= ltile[4]; i4++)
     {
      tfpixel[4] = (i4 - 1) * tilesize[4] + 1;
      tlpixel[4] = minvalue(tfpixel[4] + tilesize[4] - 1, 
                            naxis[4]);
      thistilesize[4] = thistilesize[5] * (tlpixel[4] - tfpixel[4] + 1);
      offset[4] = (i4 - 1) * rowdim[4] + offset[5];
      for (i3 = ftile[3]; i3 <= ltile[3]; i3++)
      {
        tfpixel[3] = (i3 - 1) * tilesize[3] + 1;
        tlpixel[3] = minvalue(tfpixel[3] + tilesize[3] - 1, 
                              naxis[3]);
        thistilesize[3] = thistilesize[4] * (tlpixel[3] - tfpixel[3] + 1);
        offset[3] = (i3 - 1) * rowdim[3] + offset[4];
        for (i2 = ftile[2]; i2 <= ltile[2]; i2++)
        {
          tfpixel[2] = (i2 - 1) * tilesize[2] + 1;
          tlpixel[2] = minvalue(tfpixel[2] + tilesize[2] - 1, 
                                naxis[2]);
          thistilesize[2] = thistilesize[3] * (tlpixel[2] - tfpixel[2] + 1);
          offset[2] = (i2 - 1) * rowdim[2] + offset[3];
          for (i1 = ftile[1]; i1 <= ltile[1]; i1++)
          {
            tfpixel[1] = (i1 - 1) * tilesize[1] + 1;
            tlpixel[1] = minvalue(tfpixel[1] + tilesize[1] - 1, 
                                  naxis[1]);
            thistilesize[1] = thistilesize[2] * (tlpixel[1] - tfpixel[1] + 1);
            offset[1] = (i1 - 1) * rowdim[1] + offset[2];
            for (i0 = ftile[0]; i0 <= ltile[0]; i0++)
            {
              tfpixel[0] = (i0 - 1) * tilesize[0] + 1;
              tlpixel[0] = minvalue(tfpixel[0] + tilesize[0] - 1, 
                                    naxis[0]);
              thistilesize[0] = thistilesize[1] * (tlpixel[0] - tfpixel[0] + 1);
              /* calculate row of table containing this tile */
              irow = i0 + offset[1];

              /* read and uncompress this row (tile) of the table */
              /* also do type conversion and undefined pixel substitution */
              /* at this point */

              imcomp_decompress_tile(fptr, irow, thistilesize[0],
                    datatype, nullcheck, nullval, buffer, bnullarray, &tilenul,
                     status);

              if (tilenul && anynul)
                  *anynul = 1;  /* there are null pixels */

              /* copy the intersecting pixels from this tile to the output */
              imcomp_copy_overlap(buffer, pixlen, ndim, tfpixel, tlpixel, 
                     bnullarray, array, fpixel, lpixel, nullcheck, nullarray,
                     status);
            }
          }
        }
      }
     }
    }
    if (nullcheck == 2)
    {
        free(bnullarray);
    }
    free(buffer);

    return(*status);
}
/*--------------------------------------------------------------------------*/
int fits_read_compressed_pixels(fitsfile *fptr, /* I - FITS file pointer    */
            int  datatype,  /* I - datatype of the array to be returned      */
            long   fpixel,  /* I - 'first pixel to read          */
            long   npixel,  /* I - number of pixels to read      */
            int  nullcheck,  /* I - 0 for no null checking                   */
                              /*     1: set undefined pixels = nullval       */
                              /*     2: set nullarray=1 for undefined pixels */
            void *nullval,    /* I - value for undefined pixels              */
            void *array,      /* O - array of values that are returned       */
            char *nullarray,  /* O - array of flags = 1 if nullcheck = 2     */
            int  *anynul,     /* O - set to 1 if any values are null; else 0 */
            int  *status)     /* IO - error status                           */
/*
   Read a consecutive set of pixels from a compressed image.  This routine
   interpretes the n-dimensional image as a long one-dimensional array. 
   This is actually a rather inconvenient way to read compressed images in
   general, and could be rather inefficient if the requested pixels to be
   read are located in many different image compression tiles.    

   The general strategy used here is to read the requested pixels in blocks
   that correspond to rectangular image sections.  
*/
{
    int naxis, ii, bytesperpixel, planenul;
    long naxes[MAX_COMPRESS_DIM], dimsize[MAX_COMPRESS_DIM], nread;
    long inc[MAX_COMPRESS_DIM], tfirst, tlast, last0, last1;
    long nplane, firstcoord[MAX_COMPRESS_DIM], lastcoord[MAX_COMPRESS_DIM];
    char *arrayptr, *nullarrayptr;

    if (*status > 0)
        return(*status);

    arrayptr = (char *) array;
    nullarrayptr = nullarray;

    /* get size of array pixels, in bytes */
    bytesperpixel = ffpxsz(datatype);

    for (ii = 0; ii < MAX_COMPRESS_DIM; ii++)
    {
        naxes[ii] = 1;
        firstcoord[ii] = 0;
        lastcoord[ii] = 0;
        inc[ii] = 1;
    }

    /*  determine the dimensions of the image to be read */
    ffgidm(fptr, &naxis, status);
    ffgisz(fptr, MAX_COMPRESS_DIM, naxes, status);

    /* calc the cumulative number of pixels in each successive dimension */
    dimsize[0] = 1;
    for (ii = 1; ii < MAX_COMPRESS_DIM; ii++)
         dimsize[ii] = dimsize[ii - 1] * naxes[ii - 1];

    /*  determine the coordinate of the first and last pixel in the image */
    /*  Use zero based indexes here */
    tfirst = fpixel - 1;
    tlast = tfirst + npixel - 1;
    for (ii = naxis - 1; ii >= 0; ii--)
    {
        firstcoord[ii] = tfirst / dimsize[ii];
        lastcoord[ii] = tlast / dimsize[ii];
        tfirst = tfirst - firstcoord[ii] * dimsize[ii];
        tlast = tlast - lastcoord[ii] * dimsize[ii];
    }

    /* to simplify things, treat 1-D, 2-D, and 3-D images as separate cases */

    if (naxis == 1)
    {
        /* Simple: just read the requested range of pixels */

        firstcoord[0] = firstcoord[0] + 1;
        lastcoord[0] = lastcoord[0] + 1;
        fits_read_compressed_img(fptr, datatype, firstcoord, lastcoord, inc,
            nullcheck, nullval, array, nullarray, anynul, status);
        return(*status);
    }
    else if (naxis == 2)
    {
        nplane = 0;  /* read 1st (and only) plane of the image */

        fits_read_compressed_img_plane(fptr, datatype, bytesperpixel,
          nplane, firstcoord, lastcoord, inc, naxes, nullcheck, nullval,
          array, nullarray, anynul, &nread, status);
    }
    else if (naxis == 3)
    {
        if (anynul)
            *anynul = 0;  /* initialize */

        /* save last coordinate in temporary variables */
        last0 = lastcoord[0];
        last1 = lastcoord[1];

        if (firstcoord[2] < lastcoord[2])
        {
            /* we will read up to the last pixel in all but the last plane */
            lastcoord[0] = naxes[0] - 1;
            lastcoord[1] = naxes[1] - 1;
        }

        /* read one plane of the cube at a time, for simplicity */
        for (nplane = firstcoord[2]; nplane <= lastcoord[2]; nplane++)
        {
            if (nplane == lastcoord[2])
            {
                lastcoord[0] = last0;
                lastcoord[1] = last1;
            }

            fits_read_compressed_img_plane(fptr, datatype, bytesperpixel,
              nplane, firstcoord, lastcoord, inc, naxes, nullcheck, nullval,
              arrayptr, nullarrayptr, &planenul, &nread, status);

            if (planenul && anynul)
               *anynul = 1;  /* there are null pixels */

            /* for all subsequent planes, we start with the first pixel */
            firstcoord[0] = 0;
            firstcoord[1] = 0;

            /* increment pointers to next elements to be read */
            arrayptr = arrayptr + nread * bytesperpixel;
            if (nullarrayptr && (nullcheck == 2) )
                nullarrayptr = nullarrayptr + nread;
        }
    }
    else
    {
        ffpmsg("only 1D, 2D, or 3D images are currently supported");
        return(*status = DATA_DECOMPRESSION_ERR);
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int fits_read_compressed_img_plane(fitsfile *fptr, /* I - FITS file   */
            int  datatype,  /* I - datatype of the array to be returned      */
            int  bytesperpixel, /* I - number of bytes per pixel in array */
            long   nplane,  /* I - which plane of the cube to read      */
            long *firstcoord,  /* coordinate of first pixel to read */
            long *lastcoord,   /* coordinate of last pixel to read */
            long *inc,         /* increment of pixels to read */
            long *naxes,      /* size of each image dimension */
            int  nullcheck,  /* I - 0 for no null checking                   */
                              /*     1: set undefined pixels = nullval       */
                              /*     2: set nullarray=1 for undefined pixels */
            void *nullval,    /* I - value for undefined pixels              */
            void *array,      /* O - array of values that are returned       */
            char *nullarray,  /* O - array of flags = 1 if nullcheck = 2     */
            int  *anynul,     /* O - set to 1 if any values are null; else 0 */
            long *nread,      /* O - total number of pixels read and returned*/
            int  *status)     /* IO - error status                           */

   /*
           in general we have to read the first partial row of the image,
           followed by the middle complete rows, followed by the last
           partial row of the image.  If the first or last rows are complete,
           then read them at the same time as all the middle rows.
    */
{
     /* bottom left coord. and top right coord. */
    long blc[MAX_COMPRESS_DIM], trc[MAX_COMPRESS_DIM]; 
    char *arrayptr, *nullarrayptr;
    int tnull;

    if (anynul)
        *anynul = 0;

    *nread = 0;

    arrayptr = (char *) array;
    nullarrayptr = nullarray;

    blc[2] = nplane + 1;
    trc[2] = nplane + 1;

    if (firstcoord[0] != 0)
    { 
            /* have to read a partial first row */
            blc[0] = firstcoord[0] + 1;
            blc[1] = firstcoord[1] + 1;
            trc[1] = blc[1];  
            if (lastcoord[1] == firstcoord[1])
               trc[0] = lastcoord[0] + 1; /* 1st and last pixels in same row */
            else
               trc[0] = naxes[0];  /* read entire rest of the row */

            fits_read_compressed_img(fptr, datatype, blc, trc, inc,
                nullcheck, nullval, arrayptr, nullarrayptr, &tnull, status);

            if (tnull && anynul)
               *anynul = 1;  /* there are null pixels */

            if (lastcoord[1] == firstcoord[1])
            {
               return(*status);  /* finished */
            }

            /* set starting coord to beginning of next line */
            firstcoord[0] = 0;
            firstcoord[1] += 1;
            arrayptr = arrayptr + (trc[0] - blc[0] + 1) * bytesperpixel;
            if (nullarrayptr && (nullcheck == 2) )
                nullarrayptr = nullarrayptr + (trc[0] - blc[0] + 1);

            *nread = *nread + trc[0] - blc[0] + 1;
    }

    /* read contiguous complete rows of the image, if any */
    blc[0] = 1;
    blc[1] = firstcoord[1] + 1;
    trc[0] = naxes[0];

    if (lastcoord[0] + 1 == naxes[0])
    {
            /* can read the last complete row, too */
            trc[1] = lastcoord[1] + 1;
    }
    else
    {
            /* last row is incomplete; have to read it separately */
            trc[1] = lastcoord[1];
    }

    if (trc[1] >= blc[1])  /* must have at least one whole line to read */
    {
        fits_read_compressed_img(fptr, datatype, blc, trc, inc,
                nullcheck, nullval, arrayptr, nullarrayptr, &tnull, status);

        if (tnull && anynul)
           *anynul = 1;

        if (lastcoord[1] + 1 == trc[1])
               return(*status);  /* finished */

        /* increment pointers for the last partial row */
        arrayptr = arrayptr + (trc[1] - blc[1] + 1) * naxes[0] * bytesperpixel;
        if (nullarrayptr && (nullcheck == 2) )
                nullarrayptr = nullarrayptr + (trc[1] - blc[1] + 1) * naxes[0];

        *nread = *nread + (trc[1] - blc[1] + 1) * naxes[0];
     }

    if (trc[1] == lastcoord[1] + 1)
        return(*status);           /* all done */

    /* set starting and ending coord to last line */

    trc[0] = lastcoord[0] + 1;
    trc[1] = lastcoord[1] + 1;
    blc[1] = trc[1];

    fits_read_compressed_img(fptr, datatype, blc, trc, inc,
                nullcheck, nullval, arrayptr, nullarrayptr, &tnull, status);

    if (tnull)
       *anynul = 1;

    *nread = *nread + trc[0] - blc[0] + 1;

    return(*status);
}
/*--------------------------------------------------------------------------*/
int imcomp_get_compressed_image_par(fitsfile *infptr, int *status)
 
/* 
    This routine reads keywords from a BINTABLE extension containing a
    compressed image.
*/
{
    char keyword[FLEN_KEYWORD];
    char value[FLEN_VALUE];
    int ii, tstatus;
    long expect_nrows, maxtilelen;

    if (*status > 0)
        return(*status);

    /* Copy relevant header keyword values to structure */
    if (ffgky (infptr, TSTRING, "ZCMPTYPE", value, NULL, status) > 0)
    {
        ffpmsg("required ZCMPTYPE compression keyword not found in");
        ffpmsg(" imcomp_get_compressed_image_par");
        return(*status);
    }

    (infptr->Fptr)->zcmptype[0] = '\0';
    strncat((infptr->Fptr)->zcmptype, value, 11);

    if (!FSTRCMP(value, "RICE_1") )
        (infptr->Fptr)->compress_type = RICE_1;
    else if (!FSTRCMP(value, "HCOMPRESS_1") )
        (infptr->Fptr)->compress_type = HCOMPRESS_1;
    else if (!FSTRCMP(value, "GZIP_1") )
        (infptr->Fptr)->compress_type = GZIP_1;
    else if (!FSTRCMP(value, "PLIO_1") )
        (infptr->Fptr)->compress_type = PLIO_1;
    else
    {
        ffpmsg("Unknown image compression type:");
        ffpmsg(value);
	return (*status = DATA_DECOMPRESSION_ERR);
    }

    if (ffgky (infptr, TINT,  "ZBITPIX",  &(infptr->Fptr)->zbitpix,  
               NULL, status) > 0)
    {
        ffpmsg("required ZBITPIX compression keyword not found");
        return(*status);
    }

    if (ffgky (infptr,TINT, "ZNAXIS", &(infptr->Fptr)->zndim, NULL, status) > 0)
    {
        ffpmsg("required ZNAXIS compression keyword not found");
        return(*status);
    }

    if ((infptr->Fptr)->zndim < 1)
    {
        ffpmsg("Compressed image has no data (ZNAXIS < 1)");
	return (*status = BAD_NAXIS);
    }

    if ((infptr->Fptr)->zndim > MAX_COMPRESS_DIM)
    {
        ffpmsg("Compressed image has too many dimensions");
        return(*status = BAD_NAXIS);
    }

    expect_nrows = 1;
    maxtilelen = 1;
    for (ii = 0;  ii < (infptr->Fptr)->zndim;  ii++)
    {
        /* get image size */
        sprintf (keyword, "ZNAXIS%d", ii+1);
	ffgky (infptr, TLONG,keyword, &(infptr->Fptr)->znaxis[ii],NULL,status);

        if (*status > 0)
        {
            ffpmsg("required ZNAXISn compression keyword not found");
            return(*status);
        }

        /* get compression tile size */
	sprintf (keyword, "ZTILE%d", ii+1);

        /* set default tile size in case keywords are not present */
        if (ii == 0)
            (infptr->Fptr)->tilesize[0] = (infptr->Fptr)->znaxis[0];
        else
            (infptr->Fptr)->tilesize[ii] = 1;

        tstatus = 0;
	ffgky (infptr, TLONG, keyword, &(infptr->Fptr)->tilesize[ii], NULL, 
               &tstatus);

        expect_nrows *= (((infptr->Fptr)->znaxis[ii] - 1) / 
                  (infptr->Fptr)->tilesize[ii]+ 1);
        maxtilelen *= (infptr->Fptr)->tilesize[ii];
    }

    /* check number of rows */
    if (expect_nrows != (infptr->Fptr)->numrows)
    {
        ffpmsg(
        "number of table rows != the number of tiles in compressed image");
        return (*status = DATA_DECOMPRESSION_ERR);
    }

    /* read any algorithm specific parameters */
    if ((infptr->Fptr)->compress_type == RICE_1 )
    {
        if (ffgky(infptr, TINT,"ZVAL1", &(infptr->Fptr)->rice_blocksize,
                  NULL, status) > 0)
        {
            ffpmsg("required ZVAL1 compression keyword not found");
            return(*status);
        }

        if ((infptr->Fptr)->zbitpix < 0)
        {
           /* try to read the floating point quantization parameter */
            tstatus = 0;
            ffgky(infptr, TINT,"ZVAL2", &(infptr->Fptr)->rice_nbits,
                  NULL, &tstatus);
        }
    }
    else
    {
        if ((infptr->Fptr)->zbitpix < 0)
        {
           /* try to read the floating point quantization parameter */
            tstatus = 0;
            ffgky(infptr, TINT,"ZVAL1", &(infptr->Fptr)->rice_nbits,
                  NULL, &tstatus);
        }
    }

    /* store number of pixels in each compression tile, */
    /* and max size of the compressed tile buffer */
    (infptr->Fptr)->maxtilelen = maxtilelen;

    (infptr->Fptr)->maxelem = 
           imcomp_calc_max_elem ((infptr->Fptr)->compress_type, maxtilelen, 
                                 (infptr->Fptr)->rice_blocksize);

    /* Get Column numbers. */
    if (ffgcno(infptr, CASEINSEN, "COMPRESSED_DATA",
         &(infptr->Fptr)->cn_compressed, status) > 0)
    {
        ffpmsg("couldn't find COMPRESSED_DATA column (fits_decomp_img)");
        return(*status = DATA_DECOMPRESSION_ERR);
    }

    tstatus = 0;
    if (ffgcno(infptr,CASEINSEN, "UNCOMPRESSED_DATA",
          &(infptr->Fptr)->cn_uncompressed, &tstatus) > 0)
    {
         ffxmsg(-1, value);  /* clear spurious error message from stack */
    }

    tstatus = 0;
    if (ffgcno(infptr, CASEINSEN, "ZSCALE", &(infptr->Fptr)->cn_zscale,
              &tstatus) > 0)
    {
         ffxmsg(-1, value);  /* clear spurious error message from stack */

        /* CMPSCALE column doesn't exist; see if there is a keyword */
        tstatus = 0;
        if (ffgky(infptr, TDOUBLE, "ZSCALE", &(infptr->Fptr)->zscale, NULL, 
                 &tstatus) <= 0)
            (infptr->Fptr)->cn_zscale = -1;  /* flag for a constant ZSCALE */
    }

    tstatus = 0;
    if (ffgcno(infptr, CASEINSEN, "ZZERO", &(infptr->Fptr)->cn_zzero,
               &tstatus) > 0)
    {
         ffxmsg(-1, value);  /* clear spurious error message from stack */

        /* CMPZERO column doesn't exist; see if there is a keyword */
        tstatus = 0;
        if (ffgky(infptr, TDOUBLE, "ZZERO", &(infptr->Fptr)->zzero, NULL, 
                  &tstatus) <= 0)
            (infptr->Fptr)->cn_zzero = -1;  /* flag for a constant ZZERO */
    }

    tstatus = 0;
    if (ffgcno(infptr, CASEINSEN, "ZBLANK", &(infptr->Fptr)->cn_zblank,
               &tstatus) > 0)
    {
         ffxmsg(-1, value);  /* clear spurious error message from stack */

        /* CMPZERO column doesn't exist; see if there is a keyword */
        tstatus = 0;
        if (ffgky(infptr, TINT, "ZBLANK", &(infptr->Fptr)->zblank, NULL,
                  &tstatus) <= 0)
            (infptr->Fptr)->cn_zblank = -1;  /* flag for a constant ZBLANK */
    }

    return (*status);
}
/*--------------------------------------------------------------------------*/
int imcomp_copy_imheader(fitsfile *infptr, fitsfile *outfptr, int *status)
/*
    This routine reads the header keywords from the input image and
    copies them to the output image;  the manditory structural keywords
    and the checksum keywords are not copied. If the DATE keyword is copied,
    then it is updated with the current date and time.
*/
{
    int nkeys, ii, keyclass;
    char card[FLEN_CARD];	/* a header record */

    if (*status > 0)
        return(*status);

    ffghsp(infptr, &nkeys, NULL, status); /* get number of keywords in image */

    for (ii = 5; ii <= nkeys; ii++)  /* skip the first 4 keywords */
    {
        ffgrec(infptr, ii, card, status);

	keyclass = ffgkcl(card);  /* Get the type/class of keyword */

        /* don't copy structural keywords or checksum keywords */
        if ((keyclass <= TYP_CMPRS_KEY) || (keyclass == TYP_CKSUM_KEY))
	    continue;

        if (FSTRNCMP(card, "DATE ", 5) == 0) /* write current date */
        {
            ffpdat(outfptr, status);
        }
        else if (FSTRNCMP(card, "EXTNAME ", 8) == 0) 
        {
            /* don't copy default EXTNAME keyword from a compressed image */
            if (FSTRNCMP(card, "EXTNAME = 'COMPRESSED_IMAGE'", 28))
            {
                /* if EXTNAME keyword already exists, overwrite it */
                /* otherwise append a new EXTNAME keyword */
                ffucrd(outfptr, "EXTNAME", card, status);
            }
        }
        else
        {
            /* just copy the keyword to the output header */
	    ffprec (outfptr, card, status);
        }

        if (*status > 0)
           return (*status);
    }
    return (*status);
}
/*--------------------------------------------------------------------------*/
int imcomp_decompress_tile (fitsfile *infptr,
          int nrow,            /* I - row of table to read and uncompress */
          int tilelen,         /* I - number of pixels in the tile        */
          int datatype,        /* I - datatype to be returned in 'buffer' */
          int nullcheck,       /* I - 0 for no null checking */
          void *nulval,        /* I - value to be used for undefined pixels */
          void *buffer,        /* O - buffer for returned decompressed values */
          char *bnullarray,    /* O - buffer for returned null flags */
          int *anynul,         /* O - any null values returned?  */
          int *status)

/* This routine decompresses one tile of the image */
{
    int *idata = 0;          /* uncompressed integer data */
    size_t idatalen, tilebytesize;
    int ii, tnull;        /* value in the data which represents nulls */
    unsigned char *cbuf = 0; /* compressed data */
    unsigned char charnull = 0;
    short *sbuf = 0;
    short snull = 0;
    int blocksize;
    double bscale, bzero, dummy = 0;    /* scaling parameters */
    long nelem, offset;      /* number of bytes */

    if (*status > 0)
       return(*status);

    /* get length of the compressed byte stream */
    ffgdes (infptr, (infptr->Fptr)->cn_compressed, nrow, &nelem, &offset, 
            status);

    /* **************************************************************** */
    if (nelem == 0)  /* tile was not compressed; read uncompressed data */
    {
        if ((infptr->Fptr)->cn_uncompressed < 1 )
        {
	    ffpmsg("No compressed data and no uncompressed data column");
	    return (*status = DATA_DECOMPRESSION_ERR);
        }

        /* no compressed data, so simply read the uncompressed data */
        /* directly from the UNCOMPRESSED_DATA column, then return */   
        ffgdes (infptr, (infptr->Fptr)->cn_uncompressed, nrow, &nelem,
               &offset, status);

        if (nullcheck <= 1)  
            fits_read_col(infptr, datatype, (infptr->Fptr)->cn_uncompressed,
               nrow, 1, nelem, nulval, buffer, anynul, status);
        else
            fits_read_colnull(infptr, datatype, (infptr->Fptr)->cn_uncompressed,
               nrow, 1, nelem, buffer, bnullarray, anynul, status);

        return(*status);
    }
    
    /* **************************************************************** */

    if (nullcheck == 2)
    {
        for (ii = 0; ii < tilelen; ii++)  /* initialize the null array */
            bnullarray[ii] = 0;
    }

    if (anynul)
       *anynul = 0;
    
    /* get linear scaling and offset values, if they exist */
    if ((infptr->Fptr)->cn_zscale == 0)
    {
         /* set default scaling, if scaling is not defined */
         bscale = 1.;
         bzero = 0.;
    }
    else if ((infptr->Fptr)->cn_zscale == -1)
    {
        bscale = (infptr->Fptr)->zscale;
        bzero = (infptr->Fptr)->zzero;
    }
    else
    {
        /* read the linear scale and offset values for this row */
	ffgcvd (infptr, (infptr->Fptr)->cn_zscale, nrow, 1, 1, 0.,
				&bscale, NULL, status);
	ffgcvd (infptr, (infptr->Fptr)->cn_zzero, nrow, 1, 1, 0.,
				&bzero, NULL, status);
        if (*status > 0)
        {
          ffpmsg("error reading scaling factor and offset for compressed tile");
          free(idata);
          free (cbuf);
          return (*status);
        }
    }

    /* ************************************************************* */

    /* get the value used to represent nulls in the int array */
    if ((infptr->Fptr)->cn_zblank == 0)
    {
        nullcheck = 0;  /* no null value; don't check for nulls */
    }
    else if ((infptr->Fptr)->cn_zblank == -1)
    {
        tnull = (infptr->Fptr)->zblank;  /* use the the ZBLANK keyword */
    }
    else
    {
        /* read the null value for this row */
	ffgcvk (infptr, (infptr->Fptr)->cn_zblank, nrow, 1, 1, 0.,
				&tnull, NULL, status);
        if (*status > 0)
        {
            ffpmsg("error reading null value for compressed tile");
            free(idata);
	    free (cbuf);
            return (*status);
        }
    }

    /* ************************************************************* */

    /* allocate memory for uncompressed integers */
    idata = (int*) calloc (tilelen, sizeof (int));
    if (idata == NULL)
    {
	    ffpmsg("Out of memory for idata. (imcomp_decompress_tile)");
	    return (*status = MEMORY_ALLOCATION);
    }

    /* ************************************************************* */

    if ((infptr->Fptr)->compress_type == RICE_1)
    {
        cbuf = (unsigned char *) calloc (nelem, sizeof (unsigned char));
        if (cbuf == NULL)
        {
	    ffpmsg("Out of memory for cbuf. (imcomp_decompress_tile)");
            free(idata);
	    return (*status = MEMORY_ALLOCATION);
        }

        /* read array of compressed bytes */
        if (fits_read_col(infptr, TBYTE, (infptr->Fptr)->cn_compressed, nrow,
             1, nelem, &charnull, cbuf, NULL, status) > 0)
        {
            ffpmsg("error reading compressed byte stream from binary table");
            free(idata);
	    free (cbuf);
            return (*status);
        }

        /* uncompress the data */
        blocksize = (infptr->Fptr)->rice_blocksize;
        if ((*status = fits_rdecomp (cbuf, nelem, (unsigned int *)idata,
                tilelen, blocksize)))
        {
            free(idata);
            free (cbuf);
            return (*status);
        }

        free(cbuf);
    }

    /* ************************************************************* */

    else if ((infptr->Fptr)->compress_type == PLIO_1)
    {
        sbuf = (short *) calloc (nelem, sizeof (short));
        if (sbuf == NULL)
        {
	    ffpmsg("Out of memory for sbuf. (imcomp_decompress_tile)");
            free(idata);
	    return (*status = MEMORY_ALLOCATION);
        }

        /* read array of compressed bytes */
        if (fits_read_col(infptr, TSHORT, (infptr->Fptr)->cn_compressed, nrow,
             1, nelem, &snull, sbuf, NULL, status) > 0)
        {
            ffpmsg("error reading compressed byte stream from binary table");
            free(idata);
	    free (sbuf);
            return (*status);
        }

        pl_l2pi (sbuf, 1, idata, tilelen);  /* uncompress the data */

        free(sbuf);
    }

    /* ************************************************************* */

    else if ((infptr->Fptr)->compress_type == GZIP_1)
    {
        cbuf = (unsigned char *) calloc (nelem, sizeof (unsigned char));
        if (cbuf == NULL)
        {
	    ffpmsg("Out of memory for cbuf. (imcomp_decompress_tile)");
            free(idata);
	    return (*status = MEMORY_ALLOCATION);
        }

        /* read array of compressed bytes */
        if (fits_read_col(infptr, TBYTE, (infptr->Fptr)->cn_compressed, nrow,
             1, nelem, &charnull, cbuf, NULL, status) > 0)
        {
            ffpmsg("error reading compressed byte stream from binary table");
            free(idata);
	    free (cbuf);
            return (*status);
        }

        /* uncompress the data */
        idatalen = tilelen * sizeof(int);
        if (uncompress2mem_from_mem ((char *)cbuf, nelem,
             (char **) &idata, &idatalen, realloc, &tilebytesize, status))
        {
            ffpmsg("uncompress2mem_from_mem returned with an error");
            free(idata);
            free (cbuf);
            return (*status);
        }

#if BYTESWAPPED
         ffswap4(idata, tilelen); /* reverse order of bytes */
#endif

        if (idatalen != tilebytesize)
        {
            ffpmsg("error: uncompressed tile has wrong size");
            free(idata);
            free (cbuf);
            return (*status = DATA_DECOMPRESSION_ERR);
        }

        free(cbuf);
    }

    /* ************************************************************* */
    else
    {
        ffpmsg("unknown compression algorithm");
        free(idata);
        return (*status = DATA_DECOMPRESSION_ERR);
    }

    /* ************************************************************* */
    /* copy the uncompressed tile data to the output buffer, doing */
    /* null checking, datatype conversion and linear scaling, if necessary */


    if (nulval == 0)
         nulval = &dummy;  /* set address to dummy value */

    if (datatype == TSHORT)
    {
            fffi4i2(idata, tilelen, bscale, bzero, nullcheck, tnull,
             *(short *) nulval, bnullarray, anynul,
             (short *) buffer, status);
    }
    else if (datatype == TINT)
    {
        fffi4int(idata, (long) tilelen, bscale, bzero, nullcheck, tnull,
         *(int *) nulval, bnullarray, anynul,
        (int *) buffer, status);
    }
    else if (datatype == TLONG)
    {
        fffi4i4(idata, tilelen, bscale, bzero, nullcheck, tnull,
         *(long *) nulval, bnullarray, anynul,
          (long *) buffer, status);
    }
    else if (datatype == TFLOAT)
    {
        fffi4r4(idata, tilelen, bscale, bzero, nullcheck, tnull,
         *(float *) nulval, bnullarray, anynul,
          (float *) buffer, status);
    }
    else if (datatype == TDOUBLE)
    {
        fffi4r8(idata, tilelen, bscale, bzero, nullcheck, tnull,
         *(double *) nulval, bnullarray, anynul,
          (double *) buffer, status);
    }
    else if (datatype == TBYTE)
    {
        fffi4i1(idata, tilelen, bscale, bzero, nullcheck, tnull,
         *(unsigned char *) nulval, bnullarray, anynul,
          (unsigned char *) buffer, status);
    }
    else if (datatype == TUSHORT)
    {
        fffi4u2(idata, tilelen, bscale, bzero, nullcheck, tnull,
         *(unsigned short *) nulval, bnullarray, anynul,
          (unsigned short *) buffer, status);
    }
    else if (datatype == TUINT)
    {
        fffi4uint(idata, tilelen, bscale, bzero, nullcheck, tnull,
         *(unsigned int *) nulval, bnullarray, anynul,
          (unsigned int *) buffer, status);
    }
    else if (datatype == TULONG)
    {
        fffi4u4(idata, tilelen, bscale, bzero, nullcheck, tnull,
         *(unsigned long *) nulval, bnullarray, anynul, 
          (unsigned long *) buffer, status);
    }
    else
         *status = BAD_DATATYPE;

    free(idata);

    return (*status);
}
/*--------------------------------------------------------------------------*/
int imcomp_copy_overlap (
    char *tile,         /* I - multi dimensional array of tile pixels */
    int pixlen,         /* I - number of bytes in each tile or image pixel */
    int ndim,           /* I - number of dimension in the tile and image */
    long *tfpixel,      /* I - first pixel number in each dim. of the tile */
    long *tlpixel,      /* I - last pixel number in each dim. of the tile */
    char *bnullarray,   /* I - array of null flags; used if nullcheck = 2 */
    char *image,        /* O - multi dimensional output image */
    long *fpixel,       /* I - first pixel number in each dim. of the image */
    long *lpixel,       /* I - last pixel number in each dim. of the image */
    int nullcheck,      /* I - 0, 1: do nothing; 2: set nullarray for nulls */
    char *nullarray, 
    int *status)

/* 
  copy the intersecting pixels from a decompressed tile to the output image. 
  Both the tile and the image must have the same number of dimensions. 
*/
{
    long imgdim[MAX_COMPRESS_DIM]; /* length of each axis of the output image */
    long tiledim[MAX_COMPRESS_DIM]; /* length of each axis of the input tile */
    long imgfpix[MAX_COMPRESS_DIM]; /* 1st image pix overlapping tile: 0 base */
    long imglpix[MAX_COMPRESS_DIM]; /* last img pix overlapping tile: 0 base */
    long tilefpix[MAX_COMPRESS_DIM]; /* 1st tile pix overlapping img: 0 base */
    long i1, i2, i3, i4;   /* offset along each axis of the image */
    long im2, im3, im4;
    long t2, t3, t4;   /* offset along each axis of the tile */
    long tilepix, imgpix;
    int ii, overlap_bytes, overlap_flags;

    if (*status > 0)
        return(*status);

    for (ii = 0; ii < MAX_COMPRESS_DIM; ii++)
    {
        imgdim[ii] = 1;
        tiledim[ii] = 1;
        imgfpix[ii] = 0;
        imglpix[ii] = 0;
        tilefpix[ii] = 0;
    }

    /* calc amount of overlap in each dimension; if there is zero */
    /* overlap in any dimension then just return  */
    
    for (ii = 0; ii < ndim; ii++)
    {
        if (tlpixel[ii] < fpixel[ii] || tfpixel[ii] > lpixel[ii])
            return(*status);  /* there are no overlapping pixels */

        /* calc dimensions of the output image section */
        imgdim[ii] = lpixel[ii] - fpixel[ii] + 1;
        if (imgdim[ii] < 1)
            return(*status = NEG_AXIS);

        /* calc dimensions of the tile */
        tiledim[ii] = tlpixel[ii] - tfpixel[ii] + 1;
        if (tiledim[ii] < 1)
            return(*status = NEG_AXIS);

        if (ii > 0)
           tiledim[ii] *= tiledim[ii - 1];  /* product of dimensions */

        /* first and last pixels in image that overlap with the tile (0 base) */
        imgfpix[ii] = maxvalue(tfpixel[ii] - fpixel[ii] , 0);
        imglpix[ii] = minvalue(tlpixel[ii] - fpixel[ii] , imgdim[ii] - 1);

        /* first pixel in the tile that overlaps with the image (0 base) */
        tilefpix[ii] = maxvalue(fpixel[ii] - tfpixel[ii], 0);

        if (ii > 0)
           imgdim[ii] *= imgdim[ii - 1];  /* product of dimensions */
    }
    /* calc number of pixels in each row (first dimension) that overlap */
    /* multiply by pixlen to get number of bytes to copy in each loop */

    overlap_flags = imglpix[0] - imgfpix[0] + 1;
    overlap_bytes = overlap_flags * pixlen;

    /* support up to 5 dimensions for now */
    for (i4 = 0; i4 <= imglpix[4] - imgfpix[4]; i4++)
    {
      im4 = (i4 + imgfpix[4]) * imgdim[3];
      t4 = (tilefpix[4] + i4) * tiledim[3];
      for (i3 = 0; i3 <= imglpix[3] - imgfpix[3]; i3++)
      {
       im3 = (i3 + imgfpix[3]) * imgdim[2] + im4;
       t3 = (tilefpix[3] + i3) * tiledim[2] + t4;
        for (i2 = 0; i2 <= imglpix[2] - imgfpix[2]; i2++)
        {
          im2 = (i2 + imgfpix[2]) * imgdim[1] + im3;
          t2 = (tilefpix[2] + i2) * tiledim[1] + t3;
          for (i1 = 0; i1 <= imglpix[1] - imgfpix[1]; i1++)
          {
             /* calc position of first pixel in tile to be copied */
             tilepix = tilefpix[0] + (tilefpix[1] + i1) * tiledim[0] + t2;

             /* calc position in image to copy to */
             imgpix = imgfpix[0] + (i1 + imgfpix[1]) * imgdim[0] + im2;
 
             /* copy overlapping null flags from tile to image */
             if (nullcheck == 2)
                 memcpy(nullarray + imgpix, bnullarray + tilepix,
                        overlap_flags);

             /* convert from image pixel to byte offset */
             tilepix *= pixlen;
             imgpix  *= pixlen;

             /* copy overlapping row of pixels from tile to image */
             memcpy(image + imgpix, tile + tilepix, overlap_bytes);
          }
        }
      }
    }
    return(*status);
}
