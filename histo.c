/*   Globally defined histogram parameters */
#include <string.h>
#include <stdlib.h>
#include "fitsio2.h"

char   *histb;
short  *histi;
int    *histj;
float  *histr;
double *histd;

int hcolnum[4], haxis, haxis1, haxis2, haxis3, haxis4, himagetype;
float amin1, amin2, amin3, amin4;
float binsize1, binsize2, binsize3, binsize4;
fitsfile *tblptr;

/*--------------------------------------------------------------------------*/
int ffhist(fitsfile **fptr,  /* IO - pointer to table with X and Y cols;    */
                             /*     on output, points to histogram image    */
           int imagetype,    /* I - datatype for image: TINT, TSHORT, etc  */
           int naxis,        /* I - number of axes in the histogram image   */
           char colname[4][FLEN_VALUE],   /* I - column names            */
           double *minin,     /* I - minimum histogram value, for each axis  */
           double *maxin,     /* I - maximum histogram value, for each axis  */
           double *binsizein, /* I - bin size along each axis */
           int *status)
{
    int ii, datatype, repeat, imin, imax, ibin, bitpix, tstatus;
    long haxes[4];
    fitsfile *histptr;
    char errmsg[FLEN_ERRMSG], keyname[FLEN_KEYWORD];
    tcolumn *colptr;
    iteratorCol imagepars[1];
    int n_cols = 1;
    long  offset = 0;
    long n_per_loop = -1;  /* force whole array to be passed at one time */
    float amin[4], amax, binsize[4];
    char svalue[FLEN_VALUE];
    double dvalue;

    if (*status > 0)
        return(*status);

    if (naxis > 4)
    {
        ffpmsg("histogram has more than 4 dimensions");
        return(*status = BAD_DIMEN);
    }

    /* reset position to the correct HDU if necessary */
    if ((*fptr)->HDUposition != ((*fptr)->Fptr)->curhdu)
        ffmahd(*fptr, ((*fptr)->HDUposition) + 1, NULL, status);

    tblptr = *fptr;
    himagetype = imagetype;
    haxis = naxis;
    if (imagetype == TBYTE)
        bitpix = BYTE_IMG;
    else if (imagetype == TSHORT)
        bitpix = SHORT_IMG;
    else if (imagetype == TINT)
        bitpix = LONG_IMG;
    else if (imagetype == TFLOAT)
        bitpix = FLOAT_IMG;
    else if (imagetype == TDOUBLE)
        bitpix = DOUBLE_IMG;
    else
        return(*status = BAD_DATATYPE);

    for (ii = 0; ii < naxis; ii++)
    {
      if (binsizein[ii] == 0.)
      {
        ffpmsg("histogram binsize = 0");
        return(*status = ZERO_SCALE);
      }

      /* Get the default column name if not already specified. The */
      /* CPREFn keyword, if it exists, gives the preferred  column  */
      /* for that axis.  Otherwise, assume "X", "Y", "Z", and "T"  */

      if (*colname[ii] == '\0')
      {
         tstatus = 0;
         ffkeyn("CPREF", ii + 1, keyname, &tstatus);
         ffgky(*fptr, TSTRING, keyname, colname[ii], NULL, &tstatus);

         if (tstatus || *colname[ii] == '\0')
         {
           if (ii == 0)
              strcpy(colname[ii], "X");
           else if (ii == 1)
              strcpy(colname[ii], "Y");
           else if (ii == 2)
              strcpy(colname[ii], "Z");
           else if (ii == 3)
              strcpy(colname[ii], "T");
         }
      }

      /* get the column number in the table */
      if (ffgcno(*fptr, CASEINSEN, colname[ii], hcolnum+ii, status) > 0)
      {
        strcpy(errmsg, "column for binned image doesn't exist: ");
        strcat(errmsg, colname[ii]);
        ffpmsg(errmsg);
        return(*status);
      }

      colptr = ((*fptr)->Fptr)->tableptr;
      colptr += (hcolnum[ii] - 1);

      repeat = colptr->trepeat;  /* vector repeat factor of the column */
      if (repeat > 1)
      {
        strcpy(errmsg, "Can't bin a vector column: ");
        strcat(errmsg, colname[ii]);
        ffpmsg(errmsg);
        return(*status = BAD_DATATYPE);
      }

      datatype = colptr->tdatatype;
      if (datatype < 0 || datatype == TSTRING)
      {
        strcpy(errmsg, "Inappropriate datatype; can't bin column: ");
        strcat(errmsg, colname[ii]);
        ffpmsg(errmsg);
        return(*status = BAD_DATATYPE);
      }

      /* use TLMINn and TLMAXn keyword values if min and max were not given */
      if (minin[ii] == FLOATNULLVALUE)
      {
        ffkeyn("TLMIN", hcolnum[ii], keyname, status);
        if (ffgky(*fptr, TFLOAT, keyname, amin+ii, NULL, status) > 0)
        {
            strcpy(errmsg, "Missing TLMINn keyword for binning column: ");
            strcat(errmsg, colname[ii]);
            ffpmsg(errmsg);
            return(*status);
        }
      }
      else
      {
        amin[ii] = minin[ii];
      }

      if (minin[ii] == FLOATNULLVALUE)
      {
        ffkeyn("TLMAX", hcolnum[ii], keyname, status);
        if (ffgky(*fptr, TFLOAT, keyname, &amax, NULL, status) > 0)
        {
            strcpy(errmsg, "Missing TLMAXn keyword for binning column: ");
            strcat(errmsg, colname[ii]);
            ffpmsg(errmsg);
            return(*status);
        }
      }
      else
      {
        amax = maxin[ii];
      }

      /* use TDBINn keyword or else 1 if bin size is not given */
      if (binsizein[ii] == FLOATNULLVALUE)
      {
         tstatus = 0;
         ffkeyn("TDBIN", hcolnum[ii], keyname, &tstatus);
         if (ffgky(*fptr, TFLOAT, keyname, binsizein + ii, NULL, &tstatus) > 0)
         {
            binsizein[ii] = 1.;  /* use default bin size */
         }
      }

      /* Determine the range and number of bins in the histogram. This  */
      /* depends on whether the input columns are integer or floats, so */
      /* treat each case separately.                                    */

      if ( (amin[ii] > amax && binsizein[ii] > 0. ) ||
           (amin[ii] < amax && binsizein[ii] < 0. ) )
          binsize[ii] = -binsizein[ii];  /* reverse the sign of binsize */
      else
          binsize[ii] = binsizein[ii];  /* binsize has the correct sign */

      ibin = binsize[ii];
      imin = amin[ii];
      imax = amax;

      if (datatype <= TLONG && (float) imin == amin[ii] &&
                               (float) imax == amax &&
                               (float) ibin == binsize[ii] )
      {
        /* this is an integer column and integer limits were entered */
        haxes[ii] = (imax - imin + ibin) / ibin;
 
        /* shift limits by 1/2 to make sure they are inclusive */
        if (amin[ii] < amax)
        {
          amin[ii] = amin[ii] - 0.5;
          amax = amax + 0.5;
        }
        else
        {
          amin[ii] = amin[ii] + 0.5;
          amax = amax - 0.5;
        }
      }
      else  /*  float datatype column and/or limits */
      {
        /* the lower limit is inclusive, but upper limit is exclusive */
        haxes[ii] = (amax - amin[ii]) / binsize[ii];
      }
    }

    /* size of histogram is now known, so create temp output file in memory */
    if (ffinit(&histptr, "mem://", status) > 0)
    {
        ffpmsg("failed to create memory file for histogram");
        return(*status);
    }

    if (ffcrim(histptr, bitpix, haxis, haxes, status) > 0)
    {
        ffpmsg("failed to create primary array histogram in memory");
        ffclos(histptr, status);
        return(*status);
    }

    /* Set global variables with histogram parameter values.    */
    /* Use separate scalar variables rather than arrays because */
    /* it is more efficient when computing the histogram.       */

    amin1 = amin[0];
    binsize1 = binsize[0];
    haxis1 = haxes[0];

    if (haxis > 1)
    {
      amin2 = amin[1];
      binsize2 = binsize[1];
      haxis2 = haxes[1];

      if (haxis > 2)
      {
        amin3 = amin[2];
        binsize3 = binsize[2];
        haxis3 = haxes[2];

        if (haxis > 3)
        {
          amin4 = amin[3];
          binsize4 = binsize[3];
          haxis4 = haxes[3];
        }
      }
    }
    
    /* define parameters of image for the iterator function */
    fits_iter_set_file(imagepars, histptr);        /* pointer to image */
    fits_iter_set_datatype(imagepars, imagetype);  /* image datatype   */
    fits_iter_set_iotype(imagepars, OutputCol);    /* image is output  */

    /* call the iterator function to write out the histogram image */
    if (fits_iterate_data(n_cols, imagepars, offset, n_per_loop,
                          ffwritehisto, NULL, status) )
         return(*status);

    /* write the World Coordinate System (WCS) keywords */
    /* create default values if WCS keywords are not present in the table */
    for (ii = 0; ii < haxis; ii++)
    {
       tstatus = 0;
       ffkeyn("TCTYP", hcolnum[ii], keyname, &tstatus);
       ffgky(*fptr, TSTRING, keyname, svalue, NULL, &tstatus);
       if (tstatus)
       {               /* just use column name as the type */
          tstatus = 0;
          ffkeyn("TTYPE", hcolnum[ii], keyname, &tstatus);
          ffgky(*fptr, TSTRING, keyname, svalue, NULL, &tstatus);
       }

       if (!tstatus)
       {
        ffkeyn("CTYPE", ii + 1, keyname, &tstatus);
        ffpky(histptr, TSTRING, keyname, svalue, "Coordinate Type", &tstatus);
       }
       else
          tstatus = 0;

       ffkeyn("TCUNI", hcolnum[ii], keyname, &tstatus);
       ffgky(*fptr, TSTRING, keyname, svalue, NULL, &tstatus);
       if (tstatus)
       {         /* use the column units */
          tstatus = 0;
          ffkeyn("TCUNI", hcolnum[ii], keyname, &tstatus);
          ffgky(*fptr, TSTRING, keyname, svalue, NULL, &tstatus);
       }

       if (!tstatus)
       {
        ffkeyn("CUNIT", ii + 1, keyname, &tstatus);
        ffpky(histptr, TSTRING, keyname, svalue, "Coordinate Units", &tstatus);
       }
       else
         tstatus = 0;

       ffkeyn("TCRPX", hcolnum[ii], keyname, &tstatus);
       ffgky(*fptr, TDOUBLE, keyname, &dvalue, NULL, &tstatus);
       if (tstatus)
       {
         dvalue = 1.0;  /* pick a default reference pixel */
         tstatus = 0;
       }

       dvalue = (dvalue - amin[ii] + .5) / binsize[ii] + .5;
       ffkeyn("CRPIX", ii + 1, keyname, &tstatus);
       ffpky(histptr, TDOUBLE, keyname, &dvalue, "Reference Pixel", &tstatus);

       ffkeyn("TCRVL", hcolnum[ii], keyname, &tstatus);
       ffgky(*fptr, TDOUBLE, keyname, &dvalue, NULL, &tstatus);
       if (tstatus)
       {
         dvalue = 1.0;  /* pick a default reference value */
         tstatus = 0;
       }

       ffkeyn("CRVAL", ii + 1, keyname, &tstatus);
       ffpky(histptr, TDOUBLE, keyname, &dvalue, "Reference Value", &tstatus);

       ffkeyn("TCDLT", hcolnum[ii], keyname, &tstatus);
       ffgky(*fptr, TDOUBLE, keyname, &dvalue, NULL, &tstatus);
       if (tstatus)
       {
         dvalue = 1.0;  /* use default pixel size */
         tstatus = 0;
       }

       dvalue = dvalue * binsize[ii];
       ffkeyn("CDELT", ii + 1, keyname, &tstatus);
       ffpky(histptr, TDOUBLE, keyname, &dvalue, "Pixel size", &tstatus);

       ffkeyn("TCROT", hcolnum[ii], keyname, &tstatus);
       ffgky(*fptr, TDOUBLE, keyname, &dvalue, NULL, &tstatus);
       if (!tstatus && dvalue != 0.)  /* only write keyword if angle != 0 */
       {
          ffkeyn("CROTA", ii + 1, keyname, &tstatus);
          ffpky(histptr, TDOUBLE, keyname, &dvalue, "Rotation angle", &tstatus);
       }
    }

    /* finally, close the original file and return ptr to the new image */
    ffclos(*fptr, status);
    *fptr = histptr;

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffwritehisto(long totaln, long pixoffset, long firstn, long nvalues,
             int narrays, iteratorCol *imagepars, void *userPointer)
/*
   Interator work function that writes out the histogram.
   The histogram values are calculated by another work function, ffcalchisto.
   This work function only gets called once, and totaln = nvalues.
*/
{
    iteratorCol colpars[4];
    int ii, status = 0;
    long rows_per_loop = 0, offset = 0;

    /* store pointer to the histogram array, and initialize to zero */
    if (himagetype == TSHORT)
        histi = (short *) fits_iter_get_array(imagepars);
    else if (himagetype == TINT)
        histj = (int *) fits_iter_get_array(imagepars);  /* TINT datatype */
    else if (himagetype == TBYTE)
        histb = (char *) fits_iter_get_array(imagepars);
    else if (himagetype == TFLOAT)
        histr = (float *) fits_iter_get_array(imagepars);
    else if (himagetype == TDOUBLE)
        histd = (double *) fits_iter_get_array(imagepars);

    /* set the column parameters for the iterator function */
    for (ii = 0; ii < haxis; ii++)
    {
      fits_iter_set_by_num(&colpars[ii], tblptr, hcolnum[ii], TFLOAT,InputCol);
    }

    /* call iterator function to calc the histogram pixel values */
    fits_iterate_data(haxis, colpars, offset, rows_per_loop,
                          ffcalchist, NULL, &status);

    return(status);
}
/*--------------------------------------------------------------------------*/
int ffcalchist(long totalrows, long offset, long firstrow, long nrows,
             int ncols, iteratorCol *colpars, void *userPointer)
/*
   Interator work function that calculates values for the 2D histogram.
*/
{
    long ii, ipix, axisbin;
    static float *col1, *col2, *col3, *col4; /* static to preserve values */
    static int incr2, incr3, incr4;

    /*  Initialization procedures: execute on the first call  */
    if (firstrow == 1)
    {
      /* assign the input array pointers to local pointers */
      col1 = (float *) fits_iter_get_array(&colpars[0]);
      if (haxis > 1)
      {
        col2 = (float *) fits_iter_get_array(&colpars[1]);
        incr2 = haxis1;

        if (haxis > 2)
        {
          col3 = (float *) fits_iter_get_array(&colpars[2]);
          incr3 = incr2 * haxis2;

          if (haxis > 3)
          {
            col4 = (float *) fits_iter_get_array(&colpars[3]);
            incr4 = incr3 * haxis3;
          }
        }
      }
    }   /* end of Initialization procedures */

    /*  Main loop: increment the histogram at position of each event */
    for (ii = 1; ii <= nrows; ii++) 
    {
      if (col1[ii] == FLOATNULLVALUE)  /* test for null value */
          break;

      /* add 1 because the 1st pixel is the null pixel value */
      ipix = (col1[ii] - amin1) / binsize1 + 1;
      if (ipix < 1 || ipix > haxis1)   /* test if bin is within range */
          break;

      if (haxis > 1)
      {
        if (col2[ii] == FLOATNULLVALUE)
          break;

        axisbin = (col2[ii] - amin2) / binsize2;
        if (axisbin < 0 || axisbin >= haxis2)
          break;

        ipix += (axisbin * incr2);

        if (haxis > 2)
        {
          if (col3[ii] == FLOATNULLVALUE)
            break;

          axisbin = (col3[ii] - amin3) / binsize3;
          if (axisbin < 0 || axisbin >= haxis3)
            break;

          ipix += (axisbin * incr3);

          if (haxis > 3)
          {
            if (col4[ii] == FLOATNULLVALUE)
              break;

            axisbin = (col4[ii] - amin4) / binsize4;
            if (axisbin < 0 || axisbin >= haxis4)
              break;

            ipix += (axisbin * incr4);
          }
        }
      }

      /* increment the histogram pixel */
      if (himagetype == TINT)
          histj[ipix]++;
      else if (himagetype == TSHORT)
          histi[ipix]++;
      else if (himagetype == TFLOAT)
          histr[ipix]++;
      else if (himagetype == TDOUBLE)
          histd[ipix]++;
      else if (himagetype == TBYTE)
          histb[ipix]++;
    }

    return(0);
}

