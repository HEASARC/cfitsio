/*  This file, putcol.c, contains routines that write data elements to     */
/*  a FITS image or table.                                                 */

/*  The FITSIO software was written by William Pence at the High Energy    */
/*  Astrophysic Science Archive Research Center (HEASARC) at the NASA      */
/*  Goddard Space Flight Center.  Users shall not, without prior written   */
/*  permission of the U.S. Government,  establish a claim to statutory     */
/*  copyright.  The Government and others acting on its behalf, shall have */
/*  a royalty-free, non-exclusive, irrevocable,  worldwide license for     */
/*  Government purposes to publish, distribute, translate, copy, exhibit,  */
/*  and perform such material.                                             */

#include <string.h>
#include <stdlib.h>
#include "fitsio2.h"

#if MACHINE == ALPHAVMS
  static float testfloat = TESTFLOAT;  /* use to test floating pt format */
#endif

/*--------------------------------------------------------------------------*/
int ffppr(  fitsfile *fptr,  /* I - FITS file pointer                       */
            int  datatype,   /* I - datatype of the value                   */
            long  firstelem, /* I - first vector element to write(1 = 1st)  */
            long  nelem,     /* I - number of values to write               */
            void  *array,    /* I - array of values that are written        */
            int  *status)    /* IO - error status                           */
/*
  Write an array of values to the primary array.  The datatype of the
  input array is defined by the 2nd argument. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of
  the FITS array is not the same as the array being written).
*/
{
    long row = 1;

    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */

    if (datatype == TBYTE)
    {
      ffpclb(fptr, 2, row, firstelem, nelem, (unsigned char *) array, status);
    }
    else if (datatype == TSHORT)
    {
      ffpcli(fptr, 2, row, firstelem, nelem, (short *) array, status);
    }
    else if (datatype == TINT)
    {
      if (sizeof(int) == sizeof(short) )
        ffpcli(fptr, 2, row, firstelem, nelem, (short *) array, status);
      else if (sizeof(int) == sizeof(long) )
        ffpclj(fptr, 2, row, firstelem, nelem, (long *) array, status);
      else
        *status = BAD_DATATYPE;
    }
    else if (datatype == TLONG)
    {
      ffpclj(fptr, 2, row, firstelem, nelem, (long *) array, status);
    }
    else if (datatype == TFLOAT)
    {
      ffpcle(fptr, 2, row, firstelem, nelem, (float *) array, status);
    }
    else if (datatype == TDOUBLE)
    {
      ffpcld(fptr, 2, row, firstelem, nelem, (double *) array, status);
    }
    else
      *status = BAD_DATATYPE;

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpprb( fitsfile *fptr,  /* I - FITS file pointer                       */
            long  group,     /* I - group to write(1 = 1st group)           */
            long  firstelem, /* I - first vector element to write(1 = 1st)  */
            long  nelem,     /* I - number of values to write               */
            unsigned char *array, /* I - array of values that are written   */
            int  *status)    /* IO - error status                           */
/*
  Write an array of values to the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of
  the FITS array is not the same as the array being written).
*/
{
    long row;

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */

    row=maxvalue(1,group);

    ffpclb(fptr, 2, row, firstelem, nelem, array, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffppri( fitsfile *fptr,  /* I - FITS file pointer                       */
            long  group,     /* I - group to write (1 = 1st group)          */
            long  firstelem, /* I - first vector element to write (1 = 1st) */
            long  nelem,     /* I - number of values to write               */
            short *array,    /* I - array of values that are written        */
            int  *status)    /* IO - error status                           */
/*
  Write an array of values to the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of
  the FITS array is not the same as the array being written).
*/
{
    long row;

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */

    row=maxvalue(1,group);

    ffpcli(fptr, 2, row, firstelem, nelem, array, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpprj( fitsfile *fptr,  /* I - FITS file pointer                       */
            long  group,     /* I - group to write(1 = 1st group)           */
            long  firstelem, /* I - first vector element to write(1 = 1st)  */
            long  nelem,     /* I - number of values to write               */
            long  *array,    /* I - array of values that are written        */
            int  *status)    /* IO - error status                           */
/*
  Write an array of values to the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of
  the FITS array is not the same as the array being written).
*/
{
    long row;

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */

    row=maxvalue(1,group);

    ffpclj(fptr, 2, row, firstelem, nelem, array, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffppre( fitsfile *fptr,  /* I - FITS file pointer                       */
            long  group,     /* I - group to write(1 = 1st group)           */
            long  firstelem, /* I - first vector element to write(1 = 1st)  */
            long  nelem,     /* I - number of values to write               */
            float *array,    /* I - array of values that are written        */
            int  *status)    /* IO - error status                           */
/*
  Write an array of values to the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of
  the FITS array is not the same as the array being written).
*/
{
    long row;

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */

    row=maxvalue(1,group);

    ffpcle(fptr, 2, row, firstelem, nelem, array, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpprd( fitsfile *fptr,  /* I - FITS file pointer                       */
            long  group,     /* I - group to write(1 = 1st group)           */
            long  firstelem, /* I - first vector element to write(1 = 1st)  */
            long  nelem,     /* I - number of values to write               */
            double *array,   /* I - array of values that are written        */
            int  *status)    /* IO - error status                           */
/*
  Write an array of values to the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of
  the FITS array is not the same as the array being written).
*/
{
    long row;

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */

    row=maxvalue(1,group);

    ffpcld(fptr, 2, row, firstelem, nelem, array, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffppnb( fitsfile *fptr,  /* I - FITS file pointer                       */
            long  group,     /* I - group to write(1 = 1st group)           */
            long  firstelem, /* I - first vector element to write(1 = 1st)  */
            long  nelem,     /* I - number of values to write               */
            unsigned char *array, /* I - array of values that are written   */
            unsigned char nulval, /* I - undefined pixel value              */
            int  *status)    /* IO - error status                           */
/*
  Write an array of values to the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of the
  FITS array is not the same as the array being written).  Any array values
  that are equal to the value of nulval will be replaced with the null
  pixel value that is appropriate for this column.
*/
{
    long row;

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */

    row=maxvalue(1,group);

    ffpcnb(fptr, 2, row, firstelem, nelem, array, nulval, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffppni( fitsfile *fptr,  /* I - FITS file pointer                       */
            long  group,     /* I - group to write(1 = 1st group)           */
            long  firstelem, /* I - first vector element to write(1 = 1st)  */
            long  nelem,     /* I - number of values to write               */
            short *array,    /* I - array of values that are written        */
            short nulval,    /* I - undefined pixel value                   */
            int  *status)    /* IO - error status                           */
/*
  Write an array of values to the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of the
  FITS array is not the same as the array being written).  Any array values
  that are equal to the value of nulval will be replaced with the null
  pixel value that is appropriate for this column.
*/
{
    long row;

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */

    row=maxvalue(1,group);

    ffpcni(fptr, 2, row, firstelem, nelem, array, nulval, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffppnj( fitsfile *fptr,  /* I - FITS file pointer                       */
            long  group,     /* I - group to write(1 = 1st group)           */
            long  firstelem, /* I - first vector element to write(1 = 1st)  */
            long  nelem,     /* I - number of values to write               */
            long  *array,    /* I - array of values that are written        */
            long  nulval,    /* I - undefined pixel value                   */
            int  *status)    /* IO - error status                           */
/*
  Write an array of values to the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of the
  FITS array is not the same as the array being written).  Any array values
  that are equal to the value of nulval will be replaced with the null
  pixel value that is appropriate for this column.
*/
{
    long row;

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */

    row=maxvalue(1,group);

    ffpcnj(fptr, 2, row, firstelem, nelem, array, nulval, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffppne( fitsfile *fptr,  /* I - FITS file pointer                       */
            long  group,     /* I - group to write(1 = 1st group)           */
            long  firstelem, /* I - first vector element to write(1 = 1st)  */
            long  nelem,     /* I - number of values to write               */
            float *array,    /* I - array of values that are written        */
            float nulval,    /* I - undefined pixel value                   */
            int  *status)    /* IO - error status                           */
/*
  Write an array of values to the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of the
  FITS array is not the same as the array being written).  Any array values
  that are equal to the value of nulval will be replaced with the null
  pixel value that is appropriate for this column.
*/
{
    long row;

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */

    row=maxvalue(1,group);

    ffpcne(fptr, 2, row, firstelem, nelem, array, nulval, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffppnd( fitsfile *fptr,   /* I - FITS file pointer                       */
            long  group,      /* I - group to write(1 = 1st group)           */
            long  firstelem,  /* I - first vector element to write(1 = 1st)  */
            long  nelem,      /* I - number of values to write               */
            double *array,    /* I - array of values that are written        */
            double nulval,    /* I - undefined pixel value                   */
            int  *status)     /* IO - error status                           */
/*
  Write an array of values to the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of the
  FITS array is not the same as the array being written).  Any array values
  that are equal to the value of nulval will be replaced with the null
  pixel value that is appropriate for this column.
*/
{
    long row;

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */

    row=maxvalue(1,group);

    ffpcnd(fptr, 2, row, firstelem, nelem, array, nulval, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffp2db(fitsfile *fptr,   /* I - FITS file pointer                     */
           long  group,      /* I - group to write(1 = 1st group)         */
           long  ncols,      /* I - number of pixels in each row of array */
           long  naxis1,     /* I - FITS image NAXIS1 value               */
           long  naxis2,     /* I - FITS image NAXIS2 value               */
           unsigned char *array, /* I - array to be written               */
           int  *status)     /* IO - error status                         */
/*
  Write an entire 2-D array of values to the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of the
  FITS array is not the same as the array being written).
*/
{
    long tablerow, nfits, narray, ii;

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */
    tablerow=maxvalue(1,group);

    if (ncols == naxis1)  /* arrays have same row length? */
    {
       /* all the image pixels are contiguous, so write all at once */
        ffpclb(fptr, 2, tablerow, 1L, naxis1 * naxis2, array, status);
        return(*status);
    }

    /* loop over the naxis2 rows in the FITS image, */
    /* writing naxis1 pixels to each row            */

    nfits = 1;   /* next pixel in FITS image to write to */
    narray = 0;  /* next pixel in input array to be written */

    for (ii = 0; ii < naxis2; ii++)
    {
      if (ffpclb(fptr, 2, tablerow, nfits, naxis1, &array[narray], status) > 0)
          return(*status);

       nfits += naxis1;
       narray += ncols;
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffp2di(fitsfile *fptr,   /* I - FITS file pointer                     */
           long  group,      /* I - group to write(1 = 1st group)         */
           long  ncols,      /* I - number of pixels in each row of array */
           long  naxis1,     /* I - FITS image NAXIS1 value               */
           long  naxis2,     /* I - FITS image NAXIS2 value               */
           short *array,     /* I - array to be written                   */
           int  *status)     /* IO - error status                         */
/*
  Write an entire 2-D array of values to the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of the
  FITS array is not the same as the array being written).
*/
{
    long tablerow, nfits, narray, ii;

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */
    tablerow=maxvalue(1,group);

    if (ncols == naxis1)  /* arrays have same row length? */
    {
       /* all the image pixels are contiguous, so write all at once */
        ffpcli(fptr, 2, tablerow, 1L, naxis1 * naxis2, array, status);
        return(*status);
    }

    /* loop over the naxis2 rows in the FITS image, */
    /* writing naxis1 pixels to each row            */

    nfits = 1;   /* next pixel in FITS image to write to */
    narray = 0;  /* next pixel in input array to be written */

    for (ii = 0; ii < naxis2; ii++)
    {
     if (ffpcli(fptr, 2, tablerow, nfits, naxis1, &array[narray], status) > 0)
          return(*status);

       nfits += naxis1;
       narray += ncols;
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffp2dj(fitsfile *fptr,   /* I - FITS file pointer                     */
           long  group,      /* I - group to write(1 = 1st group)         */
           long  ncols,      /* I - number of pixels in each row of array */
           long  naxis1,     /* I - FITS image NAXIS1 value               */
           long  naxis2,     /* I - FITS image NAXIS2 value               */
           long  *array,     /* I - array to be written                   */
           int  *status)     /* IO - error status                         */
/*
  Write an entire 2-D array of values to the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of the
  FITS array is not the same as the array being written).
*/
{
    long tablerow, nfits, narray, ii;

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */
    tablerow=maxvalue(1,group);

    if (ncols == naxis1)  /* arrays have same row length? */
    {
       /* all the image pixels are contiguous, so write all at once */
        ffpclj(fptr, 2, tablerow, 1L, naxis1 * naxis2, array, status);
        return(*status);
    }

    /* loop over the naxis2 rows in the FITS image, */
    /* writing naxis1 pixels to each row            */

    nfits = 1;   /* next pixel in FITS image to write to */
    narray = 0;  /* next pixel in input array to be written */

    for (ii = 0; ii < naxis2; ii++)
    {
     if (ffpclj(fptr, 2, tablerow, nfits, naxis1, &array[narray], status) > 0)
          return(*status);

       nfits += naxis1;
       narray += ncols;
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffp2de(fitsfile *fptr,   /* I - FITS file pointer                     */
           long  group,      /* I - group to write(1 = 1st group)         */
           long  ncols,      /* I - number of pixels in each row of array */
           long  naxis1,     /* I - FITS image NAXIS1 value               */
           long  naxis2,     /* I - FITS image NAXIS2 value               */
           float *array,     /* I - array to be written                   */
           int  *status)     /* IO - error status                         */
/*
  Write an entire 2-D array of values to the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of the
  FITS array is not the same as the array being written).
*/
{
    long tablerow, nfits, narray, ii;

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */
    tablerow=maxvalue(1,group);

    if (ncols == naxis1)  /* arrays have same row length? */
    {
       /* all the image pixels are contiguous, so write all at once */
        ffpcle(fptr, 2, tablerow, 1L, naxis1 * naxis2, array, status);
        return(*status);
    }

    /* loop over the naxis2 rows in the FITS image, */
    /* writing naxis1 pixels to each row            */

    nfits = 1;   /* next pixel in FITS image to write to */
    narray = 0;  /* next pixel in input array to be written */

    for (ii = 0; ii < naxis2; ii++)
    {
     if (ffpcle(fptr, 2, tablerow, nfits, naxis1, &array[narray], status) > 0)
          return(*status);

       nfits += naxis1;
       narray += ncols;
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffp2dd(fitsfile *fptr,   /* I - FITS file pointer                     */
           long  group,      /* I - group to write(1 = 1st group)         */
           long  ncols,      /* I - number of pixels in each row of array */
           long  naxis1,     /* I - FITS image NAXIS1 value               */
           long  naxis2,     /* I - FITS image NAXIS2 value               */
           double *array,    /* I - array to be written                   */
           int  *status)     /* IO - error status                         */
/*
  Write an entire 2-D array of values to the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of the
  FITS array is not the same as the array being written).
*/
{
    long tablerow, nfits, narray, ii;

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */
    tablerow=maxvalue(1,group);

    if (ncols == naxis1)  /* arrays have same row length? */
    {
       /* all the image pixels are contiguous, so write all at once */
        ffpcld(fptr, 2, tablerow, 1L, naxis1 * naxis2, array, status);
        return(*status);
    }

    /* loop over the naxis2 rows in the FITS image, */
    /* writing naxis1 pixels to each row            */

    nfits = 1;   /* next pixel in FITS image to write to */
    narray = 0;  /* next pixel in input array to be written */

    for (ii = 0; ii < naxis2; ii++)
    {
     if (ffpcld(fptr, 2, tablerow, nfits, naxis1, &array[narray], status) > 0)
          return(*status);

       nfits += naxis1;
       narray += ncols;
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffp3db(fitsfile *fptr,   /* I - FITS file pointer                     */
           long  group,      /* I - group to write(1 = 1st group)         */
           long  ncols,      /* I - number of pixels in each row of array */
           long  nrows,      /* I - number of rows in each plane of array */
           long  naxis1,     /* I - FITS image NAXIS1 value               */
           long  naxis2,     /* I - FITS image NAXIS2 value               */
           long  naxis3,     /* I - FITS image NAXIS3 value               */
           unsigned char *array, /* I - array to be written               */
           int  *status)     /* IO - error status                         */
/*
  Write an entire 3-D cube of values to the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of the
  FITS array is not the same as the array being written).
*/
{
    long tablerow, nfits, narray, ii, jj;

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */
    tablerow=maxvalue(1,group);

    if (ncols == naxis1 && nrows == naxis2)  /* arrays have same size? */
    {
      /* all the image pixels are contiguous, so write all at once */
      ffpclb(fptr, 2, tablerow, 1L, naxis1 * naxis2 * naxis3, array, status);
      return(*status);
    }

    nfits = 1;   /* next pixel in FITS image to write to */
    narray = 0;  /* next pixel in input array to be written */

    /* loop over naxis3 planes in the data cube */
    for (jj = 0; jj < naxis3; jj++)
    {
      /* loop over the naxis2 rows in the FITS image, */
      /* writing naxis1 pixels to each row            */

      for (ii = 0; ii < naxis2; ii++)
      {
       if (ffpclb(fptr, 2, tablerow, nfits, naxis1,&array[narray],status) > 0)
         return(*status);

       nfits += naxis1;
       narray += ncols;
      }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffp3di(fitsfile *fptr,   /* I - FITS file pointer                     */
           long  group,      /* I - group to write(1 = 1st group)         */
           long  ncols,      /* I - number of pixels in each row of array */
           long  nrows,      /* I - number of rows in each plane of array */
           long  naxis1,     /* I - FITS image NAXIS1 value               */
           long  naxis2,     /* I - FITS image NAXIS2 value               */
           long  naxis3,     /* I - FITS image NAXIS3 value               */
           short *array,     /* I - array to be written                   */
           int  *status)     /* IO - error status                         */
/*
  Write an entire 3-D cube of values to the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of the
  FITS array is not the same as the array being written).
*/
{
    long tablerow, nfits, narray, ii, jj;

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */
    tablerow=maxvalue(1,group);

    if (ncols == naxis1 && nrows == naxis2)  /* arrays have same size? */
    {
      /* all the image pixels are contiguous, so write all at once */
      ffpcli(fptr, 2, tablerow, 1L, naxis1 * naxis2 * naxis3, array, status);
      return(*status);
    }

    nfits = 1;   /* next pixel in FITS image to write to */
    narray = 0;  /* next pixel in input array to be written */

    /* loop over naxis3 planes in the data cube */
    for (jj = 0; jj < naxis3; jj++)
    {
      /* loop over the naxis2 rows in the FITS image, */
      /* writing naxis1 pixels to each row            */

      for (ii = 0; ii < naxis2; ii++)
      {
       if (ffpcli(fptr, 2, tablerow, nfits, naxis1,&array[narray],status) > 0)
         return(*status);

       nfits += naxis1;
       narray += ncols;
      }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffp3dj(fitsfile *fptr,   /* I - FITS file pointer                     */
           long  group,      /* I - group to write(1 = 1st group)         */
           long  ncols,      /* I - number of pixels in each row of array */
           long  nrows,      /* I - number of rows in each plane of array */
           long  naxis1,     /* I - FITS image NAXIS1 value               */
           long  naxis2,     /* I - FITS image NAXIS2 value               */
           long  naxis3,     /* I - FITS image NAXIS3 value               */
           long  *array,     /* I - array to be written                   */
           int  *status)     /* IO - error status                         */
/*
  Write an entire 3-D cube of values to the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of the
  FITS array is not the same as the array being written).
*/
{
    long tablerow, nfits, narray, ii, jj;

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */
    tablerow=maxvalue(1,group);

    if (ncols == naxis1 && nrows == naxis2)  /* arrays have same size? */
    {
      /* all the image pixels are contiguous, so write all at once */
      ffpclj(fptr, 2, tablerow, 1L, naxis1 * naxis2 * naxis3, array, status);
      return(*status);
    }

    nfits = 1;   /* next pixel in FITS image to write to */
    narray = 0;  /* next pixel in input array to be written */

    /* loop over naxis3 planes in the data cube */
    for (jj = 0; jj < naxis3; jj++)
    {
      /* loop over the naxis2 rows in the FITS image, */
      /* writing naxis1 pixels to each row            */

      for (ii = 0; ii < naxis2; ii++)
      {
       if (ffpclj(fptr, 2, tablerow, nfits, naxis1,&array[narray],status) > 0)
         return(*status);

       nfits += naxis1;
       narray += ncols;
      }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffp3de(fitsfile *fptr,   /* I - FITS file pointer                     */
           long  group,      /* I - group to write(1 = 1st group)         */
           long  ncols,      /* I - number of pixels in each row of array */
           long  nrows,      /* I - number of rows in each plane of array */
           long  naxis1,     /* I - FITS image NAXIS1 value               */
           long  naxis2,     /* I - FITS image NAXIS2 value               */
           long  naxis3,     /* I - FITS image NAXIS3 value               */
           float *array,     /* I - array to be written                   */
           int  *status)     /* IO - error status                         */
/*
  Write an entire 3-D cube of values to the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of the
  FITS array is not the same as the array being written).
*/
{
    long tablerow, nfits, narray, ii, jj;

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */
    tablerow=maxvalue(1,group);

    if (ncols == naxis1 && nrows == naxis2)  /* arrays have same size? */
    {
      /* all the image pixels are contiguous, so write all at once */
      ffpcle(fptr, 2, tablerow, 1L, naxis1 * naxis2 * naxis3, array, status);
      return(*status);
    }

    nfits = 1;   /* next pixel in FITS image to write to */
    narray = 0;  /* next pixel in input array to be written */

    /* loop over naxis3 planes in the data cube */
    for (jj = 0; jj < naxis3; jj++)
    {
      /* loop over the naxis2 rows in the FITS image, */
      /* writing naxis1 pixels to each row            */

      for (ii = 0; ii < naxis2; ii++)
      {
       if (ffpcle(fptr, 2, tablerow, nfits, naxis1,&array[narray],status) > 0)
         return(*status);

       nfits += naxis1;
       narray += ncols;
      }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffp3dd(fitsfile *fptr,   /* I - FITS file pointer                     */
           long  group,      /* I - group to write(1 = 1st group)         */
           long  ncols,      /* I - number of pixels in each row of array */
           long  nrows,      /* I - number of rows in each plane of array */
           long  naxis1,     /* I - FITS image NAXIS1 value               */
           long  naxis2,     /* I - FITS image NAXIS2 value               */
           long  naxis3,     /* I - FITS image NAXIS3 value               */
           double *array,    /* I - array to be written                   */
           int  *status)     /* IO - error status                         */
/*
  Write an entire 3-D cube of values to the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of the
  FITS array is not the same as the array being written).
*/
{
    long tablerow, nfits, narray, ii, jj;

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */
    tablerow=maxvalue(1,group);

    if (ncols == naxis1 && nrows == naxis2)  /* arrays have same size? */
    {
      /* all the image pixels are contiguous, so write all at once */
      ffpcld(fptr, 2, tablerow, 1L, naxis1 * naxis2 * naxis3, array, status);
      return(*status);
    }

    nfits = 1;   /* next pixel in FITS image to write to */
    narray = 0;  /* next pixel in input array to be written */

    /* loop over naxis3 planes in the data cube */
    for (jj = 0; jj < naxis3; jj++)
    {
      /* loop over the naxis2 rows in the FITS image, */
      /* writing naxis1 pixels to each row            */

      for (ii = 0; ii < naxis2; ii++)
      {
       if (ffpcld(fptr, 2, tablerow, nfits, naxis1,&array[narray],status) > 0)
         return(*status);

       nfits += naxis1;
       narray += ncols;
      }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpssb(fitsfile *fptr,   /* I - FITS file pointer                       */
           long  group,      /* I - group to write(1 = 1st group)           */
           long  naxis,      /* I - number of data axes in array            */
           long  *naxes,     /* I - size of each FITS axis                  */
           long  *fpixel,    /* I - 1st pixel in each axis to write (1=1st) */
           long  *lpixel,    /* I - last pixel in each axis to write        */
           unsigned char *array, /* I - array to be written                 */
           int  *status)     /* IO - error status                           */
/*
  Write a subsection of pixels to the primary array or image.
  A subsection is defined to be any contiguous rectangular
  array of pixels within the n-dimensional FITS data file.
  Data conversion and scaling will be performed if necessary 
  (e.g, if the datatype of the FITS array is not the same as
  the array being written).
*/
{
    long tablerow;
    long fpix[7], irange[7], dimen[7], astart, pstart;
    long off2, off3, off4, off5, off6, off7;
    long st10, st20, st30, st40, st50, st60, st70;
    long st1, st2, st3, st4, st5, st6, st7;
    long ii, i1, i2, i3, i4, i5, i6, i7;

    if (*status > 0)
        return(*status);

    if (naxis < 1 || naxis > 7)
      return(*status = BAD_DIMEN);

    tablerow=maxvalue(1,group);

     /* calculate the size and number of loops to perform in each dimension */
    for (ii = 0; ii < 7; ii++)
    {
      fpix[ii]=1;
      irange[ii]=1;
      dimen[ii]=1;
    }

    for (ii = 0; ii < naxis; ii++)
    {    
      fpix[ii]=fpixel[ii];
      irange[ii]=lpixel[ii]-fpixel[ii]+1;
      dimen[ii]=naxes[ii];
    }

    i1=irange[0];

    /* compute the pixel offset between each dimension */
    off2 =     dimen[0];
    off3 = off2 * dimen[1];
    off4 = off3 * dimen[2];
    off5 = off4 * dimen[3];
    off6 = off5 * dimen[4];
    off7 = off6 * dimen[5];

    st10 = fpix[0];
    st20 = (fpix[1] - 1) * off2;
    st30 = (fpix[2] - 1) * off3;
    st40 = (fpix[3] - 1) * off4;
    st50 = (fpix[4] - 1) * off5;
    st60 = (fpix[5] - 1) * off6;
    st70 = (fpix[6] - 1) * off7;

    /* store the initial offset in each dimension */
    st1 = st10;
    st2 = st20;
    st3 = st30;
    st4 = st40;
    st5 = st50;
    st6 = st60;
    st7 = st70;

    astart = 0;

    for (i7 = 0; i7 < irange[6]; i7++)
    {
     for (i6 = 0; i6 < irange[5]; i6++)
     {
      for (i5 = 0; i5 < irange[4]; i5++)
      {
       for (i4 = 0; i4 < irange[3]; i4++)
       {
        for (i3 = 0; i3 < irange[2]; i3++)
        {
         pstart = st1 + st2 + st3 + st4 + st5 + st6 + st7;
         for (i2 = 0; i2 < irange[1]; i2++)
         {
           if (ffpclb(fptr, 2, tablerow, pstart, i1, &array[astart],
              status) > 0)
              return(*status);

           astart += i1;
           pstart += off2;
         }
         st2 = st20;
         st3 = st3+off3;    
        }
        st3 = st30;
        st4 = st4+off4;
       }
       st4 = st40;
       st5 = st5+off5;
      }
      st5 = st50;
      st6 = st6+off6;
     }
     st6 = st60;
     st7 = st7+off7;
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpssi(fitsfile *fptr,   /* I - FITS file pointer                       */
           long  group,      /* I - group to write(1 = 1st group)           */
           long  naxis,      /* I - number of data axes in array            */
           long  *naxes,     /* I - size of each FITS axis                  */
           long  *fpixel,    /* I - 1st pixel in each axis to write (1=1st) */
           long  *lpixel,    /* I - last pixel in each axis to write        */
           short *array,     /* I - array to be written                     */
           int  *status)     /* IO - error status                           */
/*
  Write a subsection of pixels to the primary array or image.
  A subsection is defined to be any contiguous rectangular
  array of pixels within the n-dimensional FITS data file.
  Data conversion and scaling will be performed if necessary 
  (e.g, if the datatype of the FITS array is not the same as
  the array being written).
*/
{
    long tablerow;
    long fpix[7], irange[7], dimen[7], astart, pstart;
    long off2, off3, off4, off5, off6, off7;
    long st10, st20, st30, st40, st50, st60, st70;
    long st1, st2, st3, st4, st5, st6, st7;
    long ii, i1, i2, i3, i4, i5, i6, i7;

    if (*status > 0)
        return(*status);

    if (naxis < 1 || naxis > 7)
      return(*status = BAD_DIMEN);

    tablerow=maxvalue(1,group);

     /* calculate the size and number of loops to perform in each dimension */
    for (ii = 0; ii < 7; ii++)
    {
      fpix[ii]=1;
      irange[ii]=1;
      dimen[ii]=1;
    }

    for (ii = 0; ii < naxis; ii++)
    {    
      fpix[ii]=fpixel[ii];
      irange[ii]=lpixel[ii]-fpixel[ii]+1;
      dimen[ii]=naxes[ii];
    }

    i1=irange[0];

    /* compute the pixel offset between each dimension */
    off2 =     dimen[0];
    off3 = off2 * dimen[1];
    off4 = off3 * dimen[2];
    off5 = off4 * dimen[3];
    off6 = off5 * dimen[4];
    off7 = off6 * dimen[5];

    st10 = fpix[0];
    st20 = (fpix[1] - 1) * off2;
    st30 = (fpix[2] - 1) * off3;
    st40 = (fpix[3] - 1) * off4;
    st50 = (fpix[4] - 1) * off5;
    st60 = (fpix[5] - 1) * off6;
    st70 = (fpix[6] - 1) * off7;

    /* store the initial offset in each dimension */
    st1 = st10;
    st2 = st20;
    st3 = st30;
    st4 = st40;
    st5 = st50;
    st6 = st60;
    st7 = st70;

    astart = 0;

    for (i7 = 0; i7 < irange[6]; i7++)
    {
     for (i6 = 0; i6 < irange[5]; i6++)
     {
      for (i5 = 0; i5 < irange[4]; i5++)
      {
       for (i4 = 0; i4 < irange[3]; i4++)
       {
        for (i3 = 0; i3 < irange[2]; i3++)
        {
         pstart = st1 + st2 + st3 + st4 + st5 + st6 + st7;
         for (i2 = 0; i2 < irange[1]; i2++)
         {
           if (ffpcli(fptr, 2, tablerow, pstart, i1, &array[astart],
              status) > 0)
              return(*status);

           astart += i1;
           pstart += off2;
         }
         st2 = st20;
         st3 = st3+off3;    
        }
        st3 = st30;
        st4 = st4+off4;
       }
       st4 = st40;
       st5 = st5+off5;
      }
      st5 = st50;
      st6 = st6+off6;
     }
     st6 = st60;
     st7 = st7+off7;
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpssj(fitsfile *fptr,   /* I - FITS file pointer                       */
           long  group,      /* I - group to write(1 = 1st group)           */
           long  naxis,      /* I - number of data axes in array            */
           long  *naxes,     /* I - size of each FITS axis                  */
           long  *fpixel,    /* I - 1st pixel in each axis to write (1=1st) */
           long  *lpixel,    /* I - last pixel in each axis to write        */
           long *array,      /* I - array to be written                     */
           int  *status)     /* IO - error status                           */
/*
  Write a subsection of pixels to the primary array or image.
  A subsection is defined to be any contiguous rectangular
  array of pixels within the n-dimensional FITS data file.
  Data conversion and scaling will be performed if necessary 
  (e.g, if the datatype of the FITS array is not the same as
  the array being written).
*/
{
    long tablerow;
    long fpix[7], irange[7], dimen[7], astart, pstart;
    long off2, off3, off4, off5, off6, off7;
    long st10, st20, st30, st40, st50, st60, st70;
    long st1, st2, st3, st4, st5, st6, st7;
    long ii, i1, i2, i3, i4, i5, i6, i7;

    if (*status > 0)
        return(*status);

    if (naxis < 1 || naxis > 7)
      return(*status = BAD_DIMEN);

    tablerow=maxvalue(1,group);

     /* calculate the size and number of loops to perform in each dimension */
    for (ii = 0; ii < 7; ii++)
    {
      fpix[ii]=1;
      irange[ii]=1;
      dimen[ii]=1;
    }

    for (ii = 0; ii < naxis; ii++)
    {    
      fpix[ii]=fpixel[ii];
      irange[ii]=lpixel[ii]-fpixel[ii]+1;
      dimen[ii]=naxes[ii];
    }

    i1=irange[0];

    /* compute the pixel offset between each dimension */
    off2 =     dimen[0];
    off3 = off2 * dimen[1];
    off4 = off3 * dimen[2];
    off5 = off4 * dimen[3];
    off6 = off5 * dimen[4];
    off7 = off6 * dimen[5];

    st10 = fpix[0];
    st20 = (fpix[1] - 1) * off2;
    st30 = (fpix[2] - 1) * off3;
    st40 = (fpix[3] - 1) * off4;
    st50 = (fpix[4] - 1) * off5;
    st60 = (fpix[5] - 1) * off6;
    st70 = (fpix[6] - 1) * off7;

    /* store the initial offset in each dimension */
    st1 = st10;
    st2 = st20;
    st3 = st30;
    st4 = st40;
    st5 = st50;
    st6 = st60;
    st7 = st70;

    astart = 0;

    for (i7 = 0; i7 < irange[6]; i7++)
    {
     for (i6 = 0; i6 < irange[5]; i6++)
     {
      for (i5 = 0; i5 < irange[4]; i5++)
      {
       for (i4 = 0; i4 < irange[3]; i4++)
       {
        for (i3 = 0; i3 < irange[2]; i3++)
        {
         pstart = st1 + st2 + st3 + st4 + st5 + st6 + st7;
         for (i2 = 0; i2 < irange[1]; i2++)
         {
           if (ffpclj(fptr, 2, tablerow, pstart, i1, &array[astart],
              status) > 0)
              return(*status);

           astart += i1;
           pstart += off2;
         }
         st2 = st20;
         st3 = st3+off3;    
        }
        st3 = st30;
        st4 = st4+off4;
       }
       st4 = st40;
       st5 = st5+off5;
      }
      st5 = st50;
      st6 = st6+off6;
     }
     st6 = st60;
     st7 = st7+off7;
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpsse(fitsfile *fptr,   /* I - FITS file pointer                       */
           long  group,      /* I - group to write(1 = 1st group)           */
           long  naxis,      /* I - number of data axes in array            */
           long  *naxes,     /* I - size of each FITS axis                  */
           long  *fpixel,    /* I - 1st pixel in each axis to write (1=1st) */
           long  *lpixel,    /* I - last pixel in each axis to write        */
           float *array,     /* I - array to be written                     */
           int  *status)     /* IO - error status                           */
/*
  Write a subsection of pixels to the primary array or image.
  A subsection is defined to be any contiguous rectangular
  array of pixels within the n-dimensional FITS data file.
  Data conversion and scaling will be performed if necessary 
  (e.g, if the datatype of the FITS array is not the same as
  the array being written).
*/
{
    long tablerow;
    long fpix[7], irange[7], dimen[7], astart, pstart;
    long off2, off3, off4, off5, off6, off7;
    long st10, st20, st30, st40, st50, st60, st70;
    long st1, st2, st3, st4, st5, st6, st7;
    long ii, i1, i2, i3, i4, i5, i6, i7;

    if (*status > 0)
        return(*status);

    if (naxis < 1 || naxis > 7)
      return(*status = BAD_DIMEN);

    tablerow=maxvalue(1,group);

     /* calculate the size and number of loops to perform in each dimension */
    for (ii = 0; ii < 7; ii++)
    {
      fpix[ii]=1;
      irange[ii]=1;
      dimen[ii]=1;
    }

    for (ii = 0; ii < naxis; ii++)
    {    
      fpix[ii]=fpixel[ii];
      irange[ii]=lpixel[ii]-fpixel[ii]+1;
      dimen[ii]=naxes[ii];
    }

    i1=irange[0];

    /* compute the pixel offset between each dimension */
    off2 =     dimen[0];
    off3 = off2 * dimen[1];
    off4 = off3 * dimen[2];
    off5 = off4 * dimen[3];
    off6 = off5 * dimen[4];
    off7 = off6 * dimen[5];

    st10 = fpix[0];
    st20 = (fpix[1] - 1) * off2;
    st30 = (fpix[2] - 1) * off3;
    st40 = (fpix[3] - 1) * off4;
    st50 = (fpix[4] - 1) * off5;
    st60 = (fpix[5] - 1) * off6;
    st70 = (fpix[6] - 1) * off7;

    /* store the initial offset in each dimension */
    st1 = st10;
    st2 = st20;
    st3 = st30;
    st4 = st40;
    st5 = st50;
    st6 = st60;
    st7 = st70;

    astart = 0;

    for (i7 = 0; i7 < irange[6]; i7++)
    {
     for (i6 = 0; i6 < irange[5]; i6++)
     {
      for (i5 = 0; i5 < irange[4]; i5++)
      {
       for (i4 = 0; i4 < irange[3]; i4++)
       {
        for (i3 = 0; i3 < irange[2]; i3++)
        {
         pstart = st1 + st2 + st3 + st4 + st5 + st6 + st7;
         for (i2 = 0; i2 < irange[1]; i2++)
         {
           if (ffpcle(fptr, 2, tablerow, pstart, i1, &array[astart],
              status) > 0)
              return(*status);

           astart += i1;
           pstart += off2;
         }
         st2 = st20;
         st3 = st3+off3;    
        }
        st3 = st30;
        st4 = st4+off4;
       }
       st4 = st40;
       st5 = st5+off5;
      }
      st5 = st50;
      st6 = st6+off6;
     }
     st6 = st60;
     st7 = st7+off7;
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpssd(fitsfile *fptr,   /* I - FITS file pointer                       */
           long  group,      /* I - group to write(1 = 1st group)           */
           long  naxis,      /* I - number of data axes in array            */
           long  *naxes,     /* I - size of each FITS axis                  */
           long  *fpixel,    /* I - 1st pixel in each axis to write (1=1st) */
           long  *lpixel,    /* I - last pixel in each axis to write        */
           double *array,    /* I - array to be written                     */
           int  *status)     /* IO - error status                           */
/*
  Write a subsection of pixels to the primary array or image.
  A subsection is defined to be any contiguous rectangular
  array of pixels within the n-dimensional FITS data file.
  Data conversion and scaling will be performed if necessary 
  (e.g, if the datatype of the FITS array is not the same as
  the array being written).
*/
{
    long tablerow;
    long fpix[7], irange[7], dimen[7], astart, pstart;
    long off2, off3, off4, off5, off6, off7;
    long st10, st20, st30, st40, st50, st60, st70;
    long st1, st2, st3, st4, st5, st6, st7;
    long ii, i1, i2, i3, i4, i5, i6, i7;

    if (*status > 0)
        return(*status);

    if (naxis < 1 || naxis > 7)
      return(*status = BAD_DIMEN);

    tablerow=maxvalue(1,group);

     /* calculate the size and number of loops to perform in each dimension */
    for (ii = 0; ii < 7; ii++)
    {
      fpix[ii]=1;
      irange[ii]=1;
      dimen[ii]=1;
    }

    for (ii = 0; ii < naxis; ii++)
    {    
      fpix[ii]=fpixel[ii];
      irange[ii]=lpixel[ii]-fpixel[ii]+1;
      dimen[ii]=naxes[ii];
    }

    i1=irange[0];

    /* compute the pixel offset between each dimension */
    off2 =     dimen[0];
    off3 = off2 * dimen[1];
    off4 = off3 * dimen[2];
    off5 = off4 * dimen[3];
    off6 = off5 * dimen[4];
    off7 = off6 * dimen[5];

    st10 = fpix[0];
    st20 = (fpix[1] - 1) * off2;
    st30 = (fpix[2] - 1) * off3;
    st40 = (fpix[3] - 1) * off4;
    st50 = (fpix[4] - 1) * off5;
    st60 = (fpix[5] - 1) * off6;
    st70 = (fpix[6] - 1) * off7;

    /* store the initial offset in each dimension */
    st1 = st10;
    st2 = st20;
    st3 = st30;
    st4 = st40;
    st5 = st50;
    st6 = st60;
    st7 = st70;

    astart = 0;

    for (i7 = 0; i7 < irange[6]; i7++)
    {
     for (i6 = 0; i6 < irange[5]; i6++)
     {
      for (i5 = 0; i5 < irange[4]; i5++)
      {
       for (i4 = 0; i4 < irange[3]; i4++)
       {
        for (i3 = 0; i3 < irange[2]; i3++)
        {
         pstart = st1 + st2 + st3 + st4 + st5 + st6 + st7;
         for (i2 = 0; i2 < irange[1]; i2++)
         {
           if (ffpcld(fptr, 2, tablerow, pstart, i1, &array[astart],
              status) > 0)
              return(*status);

           astart += i1;
           pstart += off2;
         }
         st2 = st20;
         st3 = st3+off3;    
        }
        st3 = st30;
        st4 = st4+off4;
       }
       st4 = st40;
       st5 = st5+off5;
      }
      st5 = st50;
      st6 = st6+off6;
     }
     st6 = st60;
     st7 = st7+off7;
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpgpb( fitsfile *fptr,   /* I - FITS file pointer                      */
            long  group,      /* I - group to write(1 = 1st group)          */
            long  firstelem,  /* I - first vector element to write(1 = 1st) */
            long  nelem,      /* I - number of values to write              */
            unsigned char *array, /* I - array of values that are written   */
            int  *status)     /* IO - error status                          */
/*
  Write an array of group parameters to the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of
  the FITS array is not the same as the array being written).
*/
{
    long row;

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */

    row=maxvalue(1,group);

    ffpclb(fptr, 1L, row, firstelem, nelem, array, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpgpi( fitsfile *fptr,   /* I - FITS file pointer                      */
            long  group,      /* I - group to write(1 = 1st group)          */
            long  firstelem,  /* I - first vector element to write(1 = 1st) */
            long  nelem,      /* I - number of values to write              */
            short *array,     /* I - array of values that are written       */
            int  *status)     /* IO - error status                          */
/*
  Write an array of group parameters to the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of
  the FITS array is not the same as the array being written).
*/
{
    long row;

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */

    row=maxvalue(1,group);

    ffpcli(fptr, 1L, row, firstelem, nelem, array, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpgpj( fitsfile *fptr,   /* I - FITS file pointer                      */
            long  group,      /* I - group to write(1 = 1st group)          */
            long  firstelem,  /* I - first vector element to write(1 = 1st) */
            long  nelem,      /* I - number of values to write              */
            long  *array,     /* I - array of values that are written       */
            int  *status)     /* IO - error status                          */
/*
  Write an array of group parameters to the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of
  the FITS array is not the same as the array being written).
*/
{
    long row;

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */

    row=maxvalue(1,group);

    ffpclj(fptr, 1L, row, firstelem, nelem, array, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpgpe( fitsfile *fptr,   /* I - FITS file pointer                      */
            long  group,      /* I - group to write(1 = 1st group)          */
            long  firstelem,  /* I - first vector element to write(1 = 1st) */
            long  nelem,      /* I - number of values to write              */
            float *array,     /* I - array of values that are written       */
            int  *status)     /* IO - error status                          */
/*
  Write an array of group parameters to the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of
  the FITS array is not the same as the array being written).
*/
{
    long row;

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */

    row=maxvalue(1,group);

    ffpcle(fptr, 1L, row, firstelem, nelem, array, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpgpd( fitsfile *fptr,   /* I - FITS file pointer                      */
            long  group,      /* I - group to write(1 = 1st group)          */
            long  firstelem,  /* I - first vector element to write(1 = 1st) */
            long  nelem,      /* I - number of values to write              */
            double *array,    /* I - array of values that are written       */
            int  *status)     /* IO - error status                          */
/*
  Write an array of group parameters to the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of
  the FITS array is not the same as the array being written).
*/
{
    long row;

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */

    row=maxvalue(1,group);

    ffpcld(fptr, 1L, row, firstelem, nelem, array, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffppru( fitsfile *fptr,  /* I - FITS file pointer                       */
            long  group,      /* I - group to write(1 = 1st group)          */
            long  firstelem,  /* I - first vector element to write(1 = 1st) */
            long  nelem,      /* I - number of values to write              */
            int  *status)     /* IO - error status                          */
/*
  Write null values to the primary array.
*/
{
    long row;

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */

    row=maxvalue(1,group);

    ffpclu(fptr, 2, row, firstelem, nelem, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpcl(  fitsfile *fptr,  /* I - FITS file pointer                       */
            int  datatype,   /* I - datatype of the value                   */
            int  colnum,     /* I - number of column to write (1 = 1st col) */
            long  firstrow,  /* I - first row to write (1 = 1st row)        */
            long  firstelem, /* I - first vector element to write (1 = 1st) */
            long  nelem,     /* I - number of elements to write             */
            void  *array,    /* I - array of values that are written        */
            int  *status)    /* IO - error status                           */
/*
  Write an array of values to a table column.  The datatype of the
  input array is defined by the 2nd argument. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of
  the FITS column is not the same as the array being written).
*/
{
    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    if (datatype == TBYTE)
    {
      ffpclb(fptr, colnum, firstrow, firstelem, nelem, (unsigned char *) array,
             status);
    }
    else if (datatype == TSHORT)
    {
      ffpcli(fptr, colnum, firstrow, firstelem, nelem, (short *) array,
             status);
    }
    else if (datatype == TINT)
    {
      if (sizeof(int) == sizeof(short) )
        ffpcli(fptr, colnum, firstrow, firstelem, nelem, (short *) array,
               status);
      else if (sizeof(int) == sizeof(long) )
        ffpclj(fptr, colnum, firstrow, firstelem, nelem, (long *) array,
               status);
      else
        *status = BAD_DATATYPE;

    }
    else if (datatype == TLONG)
    {
      ffpclj(fptr, colnum, firstrow, firstelem, nelem, (long *) array,
             status);
    }
    else if (datatype == TFLOAT)
    {
      ffpcle(fptr, colnum, firstrow, firstelem, nelem, (float *) array,
             status);
    }
    else if (datatype == TDOUBLE)
    {
      ffpcld(fptr, colnum, firstrow, firstelem, nelem, (double *) array,
             status);
    }
    else if (datatype == TSTRING)
    {
      ffpcls(fptr, colnum, firstrow, firstelem, nelem, (char **) array,
             status);
    }
    else
      *status = BAD_DATATYPE;

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpcls( fitsfile *fptr,  /* I - FITS file pointer                       */
            int  colnum,     /* I - number of column to write (1 = 1st col) */
            long  firstrow,  /* I - first row to write (1 = 1st row)        */
            long  firstelem, /* I - first vector element to write (1 = 1st) */
            long  nelem,     /* I - number of strings to write              */
            char  **array,   /* I - array of pointers to strings            */
            int  *status)    /* IO - error status                           */
/*
  Write an array of string values to a column in the current FITS HDU.
*/
{
    int tcode, maxelem, hdutype, ii, nchar;
    long twidth, incre, repeat, rowlen, rownum, elemnum, remain, next;
    long tnull, startpos, wrtptr;
    double scale, zero;
    char tform[20], *blanks;
    char message[FLEN_ERRMSG];
    char snull[20];   /*  the FITS null value  */
    tcolumn *colptr;

    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    if (fptr->datastart == DATA_UNDEFINED)
        if ( ffrdef(fptr, status) > 0)               /* rescan header */
            return(*status);

    /*---------------------------------------------------*/
    /*  Check input and get parameters about the column: */
    /*---------------------------------------------------*/
    if (colnum < 1 || colnum > fptr->tfield)
    {
        sprintf(message, "Specified column number is out of range: %d",
                colnum);
        ffpmsg(message);
        return(*status = BAD_COL_NUM);
    }

    colptr  = fptr->tableptr;   /* point to first column */
    colptr += (colnum - 1);     /* offset to correct column structure */
    tcode = colptr->tdatatype;

    if (tcode == -TSTRING) /* variable length column in a binary table? */
    {
      /* only write a single string; ignore value of firstelem */
      nchar = strlen(array[0]); 

      if (ffgcpr( fptr, colnum, firstrow, 1, nchar, 1, &scale, &zero,
        tform, &twidth, &tcode, &maxelem, &startpos,  &elemnum, &incre,
        &repeat, &rowlen, &hdutype, &tnull, snull, status) > 0)
        return(*status);

      remain = 1;
      twidth = nchar;  
    }
    else if (tcode == TSTRING)
    {
      if (ffgcpr( fptr, colnum, firstrow, firstelem, nelem, 1, &scale, &zero,
        tform, &twidth, &tcode, &maxelem, &startpos,  &elemnum, &incre,
        &repeat, &rowlen, &hdutype, &tnull, snull, status) > 0)
        return(*status);

      blanks = (char *) malloc(twidth); /* string for blank fill values */
      for (ii = 0; ii < twidth; ii++)
          blanks[ii] = ' ';          /* fill string with blanks */

      remain = nelem;           /* remaining number of values to write  */
    }
    else 
      return(*status = NOT_ASCII_COL);
 
    /*---------------------------------------------------------------------*/
    /*  Now write the strings one at a time to the FITS column.            */
    /*---------------------------------------------------------------------*/

    next = 0;                 /* next element in array to be written  */
    rownum = 0;               /* row number, relative to firstrow     */

    while (remain)
    {
      wrtptr = startpos + (rownum * rowlen) + (elemnum * incre);
      nchar = strlen(array[next]); 
      nchar = minvalue(nchar, twidth);  /* only write what fits in field */

      ffmbyt(fptr, wrtptr, IGNORE_EOF, status);  /* move to write position */

      ffpbyt(fptr, nchar, array[next], status);

      if (nchar < twidth)
         ffpbyt(fptr, twidth - nchar, blanks, status);  /* fill with blanks */

      if (*status > 0)  /* test for error during previous write operation */
      {
         sprintf(message,
           "Error writing element %ld of input array of strings (ffpcls).",
            next+1);
         ffpmsg(message);
         return(*status);
      }

      /*--------------------------------------------*/
      /*  increment the counters for the next loop  */
      /*--------------------------------------------*/
      remain--;
      if (remain)
      {
        next++;
        elemnum++;
        if (elemnum == repeat)  /* completed a row; start on next row */
        {
          elemnum = 0;
          rownum++;
        }
      }
    }  /*  End of main while Loop  */

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpcll( fitsfile *fptr,  /* I - FITS file pointer                       */
            int  colnum,     /* I - number of column to write (1 = 1st col) */
            long  firstrow,  /* I - first row to write (1 = 1st row)        */
            long  firstelem, /* I - first vector element to write (1 = 1st) */
            long  nelem,     /* I - number of values to write               */
            char *array,     /* I - array of values to write                */
            int  *status)    /* IO - error status                           */
/*
  Write an array of logical values to a column in the current FITS HDU.
*/
{
    int tcode, maxelem, hdutype;
    long twidth, incre, repeat, rowlen, rownum, elemnum, remain, next;
    long tnull, startpos, wrtptr;
    double scale, zero;
    char tform[20], ctrue = 'T', cfalse = 'F';
    char message[FLEN_ERRMSG];
    char snull[20];   /*  the FITS null value  */

    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    /*---------------------------------------------------*/
    /*  Check input and get parameters about the column: */
    /*---------------------------------------------------*/
    if (ffgcpr( fptr, colnum, firstrow, firstelem, nelem, 1, &scale, &zero,
        tform, &twidth, &tcode, &maxelem, &startpos,  &elemnum, &incre,
        &repeat, &rowlen, &hdutype, &tnull, snull, status) > 0)
        return(*status);

    if (tcode != TLOGICAL)   
        return(*status = NOT_LOGICAL_COL);

    /*---------------------------------------------------------------------*/
    /*  Now write the logical values one at a time to the FITS column.     */
    /*---------------------------------------------------------------------*/
    remain = nelem;           /* remaining number of values to write  */
    next = 0;                 /* next element in array to be written  */
    rownum = 0;               /* row number, relative to firstrow     */

    while (remain)
    {
      wrtptr = startpos + (rownum * rowlen) + (elemnum * incre);

      ffmbyt(fptr, wrtptr, IGNORE_EOF, status);  /* move to write position */

      if (array[next])
         ffpbyt(fptr, 1, &ctrue, status);
      else
         ffpbyt(fptr, 1, &cfalse, status);

      if (*status > 0)  /* test for error during previous write operation */
      {
        sprintf(message,
           "Error writing element %ld of input array of logicals (ffpcll).",
            next+1);
        ffpmsg(message);
        return(*status);
      }

      /*--------------------------------------------*/
      /*  increment the counters for the next loop  */
      /*--------------------------------------------*/
      remain--;
      if (remain)
      {
        next++;
        elemnum++;
        if (elemnum == repeat)  /* completed a row; start on next row */
        {
           elemnum = 0;
           rownum++;
        }
      }

    }  /*  End of main while Loop  */

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpclb( fitsfile *fptr,  /* I - FITS file pointer                       */
            int  colnum,     /* I - number of column to write (1 = 1st col) */
            long  firstrow,  /* I - first row to write (1 = 1st row)        */
            long  firstelem, /* I - first vector element to write (1 = 1st) */
            long  nelem,     /* I - number of values to write               */
            unsigned char *array, /* I - array of values to write           */
            int  *status)    /* IO - error status                           */
/*
  Write an array of values to a column in the current FITS HDU.
  The column number may refer to a real column in an ASCII or binary table, 
  or it may refer to a virtual column in a 1 or more grouped FITS primary
  array.  FITSIO treats a primary array as a binary table with
  2 vector columns: the first column contains the group parameters (often
  with length = 0) and the second column contains the array of image pixels.
  Each row of the table represents a group in the case of multigroup FITS
  images.

  The input array of values will be converted to the datatype of the column 
  and will be inverse-scaled by the FITS TSCALn and TZEROn values if necessary.
*/
{
    int tcode, maxelem, hdutype, writeraw;
    long twidth, incre, repeat, rowlen, rownum, elemnum, remain, next, ntodo;
    long tnull, startpos, wrtptr;
    double scale, zero, dvalue;
    char tform[20], cform[20], cstring[50];
    char message[FLEN_ERRMSG];

    char snull[20];   /*  the FITS null value  */

    char cbuff[DBUFFSIZE];
    void *buffer;

    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    buffer = cbuff;

    /*---------------------------------------------------*/
    /*  Check input and get parameters about the column: */
    /*---------------------------------------------------*/
    if (ffgcpr( fptr, colnum, firstrow, firstelem, nelem, 1, &scale, &zero,
        tform, &twidth, &tcode, &maxelem, &startpos,  &elemnum, &incre,
        &repeat, &rowlen, &hdutype, &tnull, snull, status) > 0)
        return(*status);

    if (tcode == TSTRING)   
         ffcfmt(tform, cform);     /* derive C format for writing strings */

    /*
      if there is no scaling and the native machine format is not byteswapped,
      then we can simply write the raw data bytes into the FITS file if the
      datatype of the FITS column is the same as the input values.  Otherwise,
      we must convert the raw values into the scaled and/or machine dependent
      format in a temporary buffer that has been allocated for this purpose.
    */
    if (scale == 1. && zero == 0. && 
       MACHINE == NATIVE && tcode == TBYTE)
    {
        writeraw = 1;
        maxelem = nelem;  /* we can write the entire array at one time */
    }
    else
        writeraw = 0;

    /*---------------------------------------------------------------------*/
    /*  Now write the pixels to the FITS column.                           */
    /*  First call the ffXXfYY routine to  (1) convert the datatype        */
    /*  if necessary, and (2) scale the values by the FITS TSCALn and      */
    /*  TZEROn linear scaling parameters into a temporary buffer.          */
    /*---------------------------------------------------------------------*/
    remain = nelem;           /* remaining number of values to write  */
    next = 0;                 /* next element in array to be written  */
    rownum = 0;               /* row number, relative to firstrow     */
    ntodo = remain;           /* number of elements to write at one time */

    while (ntodo)
    {
        /* limit the number of pixels to process a one time to the number that
           will fit in the buffer space or to the number of pixels that remain
           in the current vector, which ever is smaller.
        */
        ntodo = minvalue(ntodo, maxelem);      
        ntodo = minvalue(ntodo, (repeat - elemnum));

        wrtptr = startpos + (rownum * rowlen) + (elemnum * incre);

        ffmbyt(fptr, wrtptr, IGNORE_EOF, status); /* move to write position */

        switch (tcode) 
        {
            case (TBYTE):
              if (writeraw)
              {
                /* write raw input bytes without conversion */
                ffpi1b(fptr, ntodo, incre, &array[next], status);
              }
              else
              {
                /* convert the raw data before writing to FITS file */
                ffi1fi1(&array[next], ntodo, scale, zero,
                        (unsigned char *) buffer, status);
                ffpi1b(fptr, ntodo, incre, (unsigned char *) buffer, status);
              }

              break;

            case (TSHORT):
 
                ffi1fi2(&array[next], ntodo, scale, zero,
                        (short *) buffer, status);
                ffpi2b(fptr, ntodo, incre, (short *) buffer, status);
                break;

            case (TLONG):

                ffi1fi4(&array[next], ntodo, scale, zero,
                        (long *) buffer, status);
                ffpi4b(fptr, ntodo, incre, (long *) buffer, status);
                break;

            case (TFLOAT):

                ffi1fr4(&array[next], ntodo, scale, zero,
                        (float *)  buffer, status);
                ffpr4b(fptr, ntodo, incre, (float *) buffer, status);
                break;

            case (TDOUBLE):
                ffi1fr8(&array[next], ntodo, scale, zero,
                        (double *) buffer, status);
                ffpr8b(fptr, ntodo, incre, (double *) buffer, status);
                break;

            case (TSTRING):  /* numerical column in an ASCII table */

                if (cform[1] != 's')  /*  "%s" format is a string */
                {
                  dvalue = (array[next] - zero) / scale; /* inverse scale */
                  sprintf(cstring, cform, dvalue);
                  ffpbyt(fptr, twidth, cstring, status);
                  break;
                }
                /* can't write to string column, so fall thru to default: */

            default:  /*  error trap  */
                sprintf(message, 
                       "Cannot write numbers to column %d which has format %s",
                        colnum,tform);
                ffpmsg(message);
                if (hdutype == ASCII_TBL)
                    return(*status = BAD_ATABLE_FORMAT);
                else
                    return(*status = BAD_BTABLE_FORMAT);

        } /* End of switch block */

        /*-------------------------*/
        /*  Check for fatal error  */
        /*-------------------------*/
        if (*status > 0)  /* test for error during previous write operation */
        {
          sprintf(message,
          "Error writing elements %ld thru %ld of input data array (ffpclb).",
              next+1, next+ntodo);
          ffpmsg(message);
          return(*status);
        }

        /*--------------------------------------------*/
        /*  increment the counters for the next loop  */
        /*--------------------------------------------*/
        remain -= ntodo;
        if (remain)
        {
            next += ntodo;
            elemnum += ntodo;
            if (elemnum == repeat)  /* completed a row; start on next row */
            {
                elemnum = 0;
                rownum++;
            }
        }
        ntodo = remain;  /* this is the maximum number to do in next loop */

    }  /*  End of main while Loop  */


    /*--------------------------------*/
    /*  check for numerical overflow  */
    /*--------------------------------*/
    if (*status == OVERFLOW_ERR)
    {
      ffpmsg(
      "Numerical overflow during type conversion while writing FITS data.");
      *status = NUM_OVERFLOW;
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpcli( fitsfile *fptr,  /* I - FITS file pointer                       */
            int  colnum,     /* I - number of column to write (1 = 1st col) */
            long  firstrow,  /* I - first row to write (1 = 1st row)        */
            long  firstelem, /* I - first vector element to write (1 = 1st) */
            long  nelem,     /* I - number of values to write               */
            short *array,    /* I - array of values to write                */
            int  *status)    /* IO - error status                           */
/*
  Write an array of values to a column in the current FITS HDU.
  The column number may refer to a real column in an ASCII or binary table, 
  or it may refer to a virtual column in a 1 or more grouped FITS primary
  array.  FITSIO treats a primary array as a binary table with
  2 vector columns: the first column contains the group parameters (often
  with length = 0) and the second column contains the array of image pixels.
  Each row of the table represents a group in the case of multigroup FITS
  images.

  The input array of values will be converted to the datatype of the column 
  and will be inverse-scaled by the FITS TSCALn and TZEROn values if necessary.
*/
{
    int tcode, maxelem, hdutype, writeraw;
    long twidth, incre, repeat, rowlen, rownum, elemnum, remain, next, ntodo;
    long tnull, startpos, wrtptr;
    double scale, zero, dvalue;
    char tform[20], cform[20], cstring[50];
    char message[FLEN_ERRMSG];

    char snull[20];   /*  the FITS null value  */

    char cbuff[DBUFFSIZE];
    void *buffer;

    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    buffer = cbuff;

    /*---------------------------------------------------*/
    /*  Check input and get parameters about the column: */
    /*---------------------------------------------------*/
    if (ffgcpr( fptr, colnum, firstrow, firstelem, nelem, 1, &scale, &zero,
        tform, &twidth, &tcode, &maxelem, &startpos,  &elemnum, &incre,
        &repeat, &rowlen, &hdutype, &tnull, snull, status) > 0)
        return(*status);

    if (tcode == TSTRING)   
         ffcfmt(tform, cform);     /* derive C format for writing strings */

    /*
      if there is no scaling and the native machine format is not byteswapped,
      then we can simply write the raw data bytes into the FITS file if the
      datatype of the FITS column is the same as the input values.  Otherwise,
      we must convert the raw values into the scaled and/or machine dependent
      format in a temporary buffer that has been allocated for this purpose.
    */
    if (scale == 1. && zero == 0. && 
       MACHINE == NATIVE && tcode == TSHORT)
    {
        writeraw = 1;
        maxelem = nelem;  /* we can write the entire array at one time */
    }
    else
        writeraw = 0;

    /*---------------------------------------------------------------------*/
    /*  Now write the pixels to the FITS column.                           */
    /*  First call the ffXXfYY routine to  (1) convert the datatype        */
    /*  if necessary, and (2) scale the values by the FITS TSCALn and      */
    /*  TZEROn linear scaling parameters into a temporary buffer.          */
    /*---------------------------------------------------------------------*/
    remain = nelem;           /* remaining number of values to write  */
    next = 0;                 /* next element in array to be written  */
    rownum = 0;               /* row number, relative to firstrow     */
    ntodo = remain;           /* number of elements to write at one time */

    while (ntodo)
    {
        /* limit the number of pixels to process a one time to the number that
           will fit in the buffer space or to the number of pixels that remain
           in the current vector, which ever is smaller.
        */
        ntodo = minvalue(ntodo, maxelem);      
        ntodo = minvalue(ntodo, (repeat - elemnum));

        wrtptr = startpos + (rownum * rowlen) + (elemnum * incre);

        ffmbyt(fptr, wrtptr, IGNORE_EOF, status); /* move to write position */

        switch (tcode) 
        {
            case (TSHORT):
              if (writeraw)
              {
                /* write raw input bytes without conversion */
                ffpi2b(fptr, ntodo, incre, &array[next], status);
              }
              else
              {
                /* convert the raw data before writing to FITS file */
                ffi2fi2(&array[next], ntodo, scale, zero,
                        (short *) buffer, status);
                ffpi2b(fptr, ntodo, incre, (short *) buffer, status);
              }

              break;

            case (TBYTE):
 
                ffi2fi1(&array[next], ntodo, scale, zero,
                        (unsigned char *) buffer, status);
                ffpi1b(fptr, ntodo, incre, (unsigned char *) buffer, status);
                break;

            case (TLONG):

                ffi2fi4(&array[next], ntodo, scale, zero,
                        (long *) buffer, status);
                ffpi4b(fptr, ntodo, incre, (long *) buffer, status);
                break;

            case (TFLOAT):

                ffi2fr4(&array[next], ntodo, scale, zero,
                        (float *) buffer, status);
                ffpr4b(fptr, ntodo, incre, (float *) buffer, status);
                break;

            case (TDOUBLE):
                ffi2fr8(&array[next], ntodo, scale, zero,
                        (double *) buffer, status);
                ffpr8b(fptr, ntodo, incre, (double *) buffer, status);
                break;

            case (TSTRING):  /* numerical column in an ASCII table */

                if (cform[1] != 's')  /*  "%s" format is a string */
                {
                  dvalue = (array[next] - zero) / scale; /* inverse scale */
                  sprintf(cstring, cform, dvalue);
                  ffpbyt(fptr, twidth, cstring, status);
                  break;
                }
                /* can't write to string column, so fall thru to default: */

            default:  /*  error trap  */
                sprintf(message, 
                    "Cannot write numbers to column %d which has format %s",
                      colnum,tform);
                ffpmsg(message);
                if (hdutype == ASCII_TBL)
                    return(*status = BAD_ATABLE_FORMAT);
                else
                    return(*status = BAD_BTABLE_FORMAT);

        } /* End of switch block */

        /*-------------------------*/
        /*  Check for fatal error  */
        /*-------------------------*/
        if (*status > 0)  /* test for error during previous write operation */
        {
         sprintf(message,
          "Error writing elements %ld thru %ld of input data array (ffpcli).",
             next+1, next+ntodo);
         ffpmsg(message);
         return(*status);
        }

        /*--------------------------------------------*/
        /*  increment the counters for the next loop  */
        /*--------------------------------------------*/
        remain -= ntodo;
        if (remain)
        {
            next += ntodo;
            elemnum += ntodo;
            if (elemnum == repeat)  /* completed a row; start on next row */
            {
                elemnum = 0;
                rownum++;
            }
        }
        ntodo = remain;  /* this is the maximum number to do in next loop */

    }  /*  End of main while Loop  */


    /*--------------------------------*/
    /*  check for numerical overflow  */
    /*--------------------------------*/
    if (*status == OVERFLOW_ERR)
    {
       ffpmsg(
       "Numerical overflow during type conversion while writing FITS data.");
       *status = NUM_OVERFLOW;
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpclj( fitsfile *fptr,  /* I - FITS file pointer                       */
            int  colnum,     /* I - number of column to write (1 = 1st col) */
            long  firstrow,  /* I - first row to write (1 = 1st row)        */
            long  firstelem, /* I - first vector element to write (1 = 1st) */
            long  nelem,     /* I - number of values to write               */
            long  *array,    /* I - array of values to write                */
            int  *status)    /* IO - error status                           */
/*
  Write an array of values to a column in the current FITS HDU.
  The column number may refer to a real column in an ASCII or binary table, 
  or it may refer to a virtual column in a 1 or more grouped FITS primary
  array.  FITSIO treats a primary array as a binary table
  with 2 vector columns: the first column contains the group parameters (often
  with length = 0) and the second column contains the array of image pixels.
  Each row of the table represents a group in the case of multigroup FITS
  images.

  The input array of values will be converted to the datatype of the column 
  and will be inverse-scaled by the FITS TSCALn and TZEROn values if necessary.
*/
{
    int tcode, maxelem, hdutype, writeraw;
    long twidth, incre, repeat, rowlen, rownum, elemnum, remain, next, ntodo;
    long tnull, startpos, wrtptr;
    double scale, zero, dvalue;
    char tform[20], cform[20], cstring[50];
    char message[FLEN_ERRMSG];

    char snull[20];   /*  the FITS null value  */

    char cbuff[DBUFFSIZE];
    void *buffer;

    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    buffer = cbuff;

    /*---------------------------------------------------*/
    /*  Check input and get parameters about the column: */
    /*---------------------------------------------------*/
    if (ffgcpr( fptr, colnum, firstrow, firstelem, nelem, 1, &scale, &zero,
        tform, &twidth, &tcode, &maxelem, &startpos,  &elemnum, &incre,
        &repeat, &rowlen, &hdutype, &tnull, snull, status) > 0)
        return(*status);


    if (tcode == TSTRING)   
         ffcfmt(tform, cform);     /* derive C format for writing strings */

    /*
       if there is no scaling and the native machine format is not byteswapped
       then we can simply write the raw data bytes into the FITS file if the
       datatype of the FITS column is the same as the input values.  Otherwise
       we must convert the raw values into the scaled and/or machine dependent
       format in a temporary buffer that has been allocated for this purpose.
    */
    if (scale == 1. && zero == 0. && 
       MACHINE == NATIVE && tcode == TLONG)
    {
        writeraw = 1;
        maxelem = nelem;  /* we can write the entire array at one time */
    }
    else
        writeraw = 0;

    /*---------------------------------------------------------------------*/
    /*  Now write the pixels to the FITS column.                           */
    /*  First call the ffXXfYY routine to  (1) convert the datatype        */
    /*  if necessary, and (2) scale the values by the FITS TSCALn and      */
    /*  TZEROn linear scaling parameters into a temporary buffer.          */
    /*---------------------------------------------------------------------*/
    remain = nelem;           /* remaining number of values to write  */
    next = 0;                 /* next element in array to be written  */
    rownum = 0;               /* row number, relative to firstrow     */
    ntodo = remain;           /* number of elements to write at one time */

    while (ntodo)
    {
        /* limit the number of pixels to process a one time to the number that
           will fit in the buffer space or to the number of pixels that remain
           in the current vector, which ever is smaller.
        */
        ntodo = minvalue(ntodo, maxelem);      
        ntodo = minvalue(ntodo, (repeat - elemnum));

        wrtptr = startpos + (rownum * rowlen) + (elemnum * incre);

        ffmbyt(fptr, wrtptr, IGNORE_EOF, status); /* move to write position */

        switch (tcode) 
        {
            case (TLONG):
              if (writeraw)
              {
                /* write raw input bytes without conversion */
                ffpi4b(fptr, ntodo, incre, &array[next], status);
              }
              else
              {
                /* convert the raw data before writing to FITS file */
                ffi4fi4(&array[next], ntodo, scale, zero,
                        (long *) buffer, status);
                ffpi4b(fptr, ntodo, incre, (long *) buffer, status);
              }

              break;

            case (TBYTE):
 
                ffi4fi1(&array[next], ntodo, scale, zero,
                        (unsigned char *) buffer, status);
                ffpi1b(fptr, ntodo, incre, (unsigned char *) buffer, status);
                break;

            case (TSHORT):

                ffi4fi2(&array[next], ntodo, scale, zero,
                        (short *) buffer, status);
                ffpi2b(fptr, ntodo, incre, (short *) buffer, status);
                break;

            case (TFLOAT):

                ffi4fr4(&array[next], ntodo, scale, zero,
                        (float *) buffer, status);
                ffpr4b(fptr, ntodo, incre, (float *) buffer, status);
                break;

            case (TDOUBLE):
                ffi4fr8(&array[next], ntodo, scale, zero,
                       (double *) buffer, status);
                ffpr8b(fptr, ntodo, incre, (double *) buffer, status);
                break;

            case (TSTRING):  /* numerical column in an ASCII table */

                if (cform[1] != 's')  /*  "%s" format is a string */
                {
                  dvalue = (array[next] - zero) / scale; /* inverse scale */
                  sprintf(cstring, cform, dvalue);
                  ffpbyt(fptr, twidth, cstring, status);
                  break;
                }
                /* can't write to string column, so fall thru to default: */

            default:  /*  error trap  */
                sprintf(message, 
                     "Cannot write numbers to column %d which has format %s",
                      colnum,tform);
                ffpmsg(message);
                if (hdutype == ASCII_TBL)
                    return(*status = BAD_ATABLE_FORMAT);
                else
                    return(*status = BAD_BTABLE_FORMAT);

        } /* End of switch block */

        /*-------------------------*/
        /*  Check for fatal error  */
        /*-------------------------*/
        if (*status > 0)  /* test for error during previous write operation */
        {
          sprintf(message,
          "Error writing elements %ld thru %ld of input data array (ffpclj).",
              next+1, next+ntodo);
          ffpmsg(message);
          return(*status);
        }

        /*--------------------------------------------*/
        /*  increment the counters for the next loop  */
        /*--------------------------------------------*/
        remain -= ntodo;
        if (remain)
        {
            next += ntodo;
            elemnum += ntodo;
            if (elemnum == repeat)  /* completed a row; start on next row */
            {
                elemnum = 0;
                rownum++;
            }
        }
        ntodo = remain;  /* this is the maximum number to do in next loop */

    }  /*  End of main while Loop  */


    /*--------------------------------*/
    /*  check for numerical overflow  */
    /*--------------------------------*/
    if (*status == OVERFLOW_ERR)
    {
        ffpmsg(
        "Numerical overflow during type conversion while writing FITS data.");
        *status = NUM_OVERFLOW;
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpcle( fitsfile *fptr,  /* I - FITS file pointer                       */
            int  colnum,     /* I - number of column to write (1 = 1st col) */
            long  firstrow,  /* I - first row to write (1 = 1st row)        */
            long  firstelem, /* I - first vector element to write (1 = 1st) */
            long  nelem,     /* I - number of values to write               */
            float *array,    /* I - array of values to write                */
            int  *status)    /* IO - error status                           */
/*
  Write an array of values to a column in the current FITS HDU.
  The column number may refer to a real column in an ASCII or binary table, 
  or it may refer to a virtual column in a 1 or more grouped FITS primary
  array.  FITSIO treats a primary array as a binary table
  with 2 vector columns: the first column contains the group parameters (often
  with length = 0) and the second column contains the array of image pixels.
  Each row of the table represents a group in the case of multigroup FITS
  images.

  The input array of values will be converted to the datatype of the column 
  and will be inverse-scaled by the FITS TSCALn and TZEROn values if necessary.
*/
{
    int tcode, maxelem, hdutype, writeraw;
    long twidth, incre, repeat, rowlen, rownum, elemnum, remain, next, ntodo;
    long tnull, startpos, wrtptr;
    double scale, zero, dvalue;
    char tform[20], cform[20], cstring[50];
    char message[FLEN_ERRMSG];

    char snull[20];   /*  the FITS null value  */

    char cbuff[DBUFFSIZE];
    void *buffer;

    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    buffer = cbuff;

    /*---------------------------------------------------*/
    /*  Check input and get parameters about the column: */
    /*---------------------------------------------------*/
    if (ffgcpr( fptr, colnum, firstrow, firstelem, nelem, 1, &scale, &zero,
        tform, &twidth, &tcode, &maxelem, &startpos,  &elemnum, &incre,
        &repeat, &rowlen, &hdutype, &tnull, snull, status) > 0)
        return(*status);


    if (tcode == TSTRING)   
         ffcfmt(tform, cform);     /* derive C format for writing strings */

    /*
       if there is no scaling and the native machine format is not byteswapped
       then we can simply write the raw data bytes into the FITS file if the
       datatype of the FITS column is the same as the input values.  Otherwise,
       we must convert the raw values into the scaled and/or machine dependent
       format in a temporary buffer that has been allocated for this purpose.
    */
    if (scale == 1. && zero == 0. && 
       MACHINE == NATIVE && tcode == TFLOAT)
    {
        writeraw = 1;
        maxelem = nelem;  /* we can write the entire array at one time */
    }
    else
        writeraw = 0;

    /*---------------------------------------------------------------------*/
    /*  Now write the pixels to the FITS column.                           */
    /*  First call the ffXXfYY routine to  (1) convert the datatype        */
    /*  if necessary, and (2) scale the values by the FITS TSCALn and      */
    /*  TZEROn linear scaling parameters into a temporary buffer.          */
    /*---------------------------------------------------------------------*/
    remain = nelem;           /* remaining number of values to write  */
    next = 0;                 /* next element in array to be written  */
    rownum = 0;               /* row number, relative to firstrow     */
    ntodo = remain;           /* number of elements to write at one time */

    while (ntodo)
    {
        /* limit the number of pixels to process a one time to the number that
           will fit in the buffer space or to the number of pixels that remain
           in the current vector, which ever is smaller.
        */
        ntodo = minvalue(ntodo, maxelem);      
        ntodo = minvalue(ntodo, (repeat - elemnum));

        wrtptr = startpos + (rownum * rowlen) + (elemnum * incre);

        ffmbyt(fptr, wrtptr, IGNORE_EOF, status); /* move to write position */

        switch (tcode) 
        {
            case (TFLOAT):
              if (writeraw)
              {
                /* write raw input bytes without conversion */
                ffpr4b(fptr, ntodo, incre, &array[next], status);
              }
              else
              {
                /* convert the raw data before writing to FITS file */
                ffr4fr4(&array[next], ntodo, scale, zero,
                        (float *) buffer, status);
                ffpr4b(fptr, ntodo, incre, (float *) buffer, status);
              }

              break;

            case (TBYTE):
 
                ffr4fi1(&array[next], ntodo, scale, zero, 
                        (unsigned char *) buffer, status);
                ffpi1b(fptr, ntodo, incre, (unsigned char *) buffer, status);
                break;

            case (TSHORT):

                ffr4fi2(&array[next], ntodo, scale, zero,
                        (short *) buffer, status);
                ffpi2b(fptr, ntodo, incre, (short *) buffer, status);
                break;

            case (TLONG):

                ffr4fi4(&array[next], ntodo, scale, zero,
                        (long *) buffer, status);
                ffpi4b(fptr, ntodo, incre, (long *) buffer, status);
                break;

            case (TDOUBLE):
                ffr4fr8(&array[next], ntodo, scale, zero,
                       (double *) buffer, status);
                ffpr8b(fptr, ntodo, incre, (double *) buffer, status);
                break;

            case (TSTRING):  /* numerical column in an ASCII table */

                if (cform[1] != 's')  /*  "%s" format is a string */
                {
                  dvalue = (array[next] - zero) / scale; /* inverse scale */
                  sprintf(cstring, cform, dvalue);
                  ffpbyt(fptr, twidth, cstring, status);
                  break;
                }
                /* can't write to string column, so fall thru to default: */

            default:  /*  error trap  */
                sprintf(message, 
                       "Cannot write numbers to column %d which has format %s",
                        colnum,tform);
                ffpmsg(message);
                if (hdutype == ASCII_TBL)
                    return(*status = BAD_ATABLE_FORMAT);
                else
                    return(*status = BAD_BTABLE_FORMAT);

        } /* End of switch block */

        /*-------------------------*/
        /*  Check for fatal error  */
        /*-------------------------*/
        if (*status > 0)  /* test for error during previous write operation */
        {
         sprintf(message,
          "Error writing elements %ld thru %ld of input data array (ffpcle).",
             next+1, next+ntodo);
         ffpmsg(message);
         return(*status);
        }

        /*--------------------------------------------*/
        /*  increment the counters for the next loop  */
        /*--------------------------------------------*/
        remain -= ntodo;
        if (remain)
        {
            next += ntodo;
            elemnum += ntodo;
            if (elemnum == repeat)  /* completed a row; start on next row */
            {
                elemnum = 0;
                rownum++;
            }
        }
        ntodo = remain;  /* this is the maximum number to do in next loop */

    }  /*  End of main while Loop  */


    /*--------------------------------*/
    /*  check for numerical overflow  */
    /*--------------------------------*/
    if (*status == OVERFLOW_ERR)
    {
        ffpmsg(
        "Numerical overflow during type conversion while writing FITS data.");
        *status = NUM_OVERFLOW;
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpcld( fitsfile *fptr,  /* I - FITS file pointer                       */
            int  colnum,     /* I - number of column to write (1 = 1st col) */
            long  firstrow,  /* I - first row to write (1 = 1st row)        */
            long  firstelem, /* I - first vector element to write (1 = 1st) */
            long  nelem,     /* I - number of values to write               */
            double *array,   /* I - array of values to write                */
            int  *status)    /* IO - error status                           */
/*
  Write an array of values to a column in the current FITS HDU.
  The column number may refer to a real column in an ASCII or binary table, 
  or it may refer to a virtual column in a 1 or more grouped FITS primary
  array.  FITSIO treats a primary array as a binary table
  with 2 vector columns: the first column contains the group parameters (often
  with length = 0) and the second column contains the array of image pixels.
  Each row of the table represents a group in the case of multigroup FITS
  images.

  The input array of values will be converted to the datatype of the column 
  and will be inverse-scaled by the FITS TSCALn and TZEROn values if necessary.
*/
{
    int tcode, maxelem, hdutype, writeraw;
    long twidth, incre, repeat, rowlen, rownum, elemnum, remain, next, ntodo;
    long tnull, startpos, wrtptr;
    double scale, zero, dvalue;
    char tform[20], cform[20], cstring[50];
    char message[FLEN_ERRMSG];

    char snull[20];   /*  the FITS null value  */

    char cbuff[DBUFFSIZE];
    void *buffer;

    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    buffer = cbuff;

    /*---------------------------------------------------*/
    /*  Check input and get parameters about the column: */
    /*---------------------------------------------------*/
    if (ffgcpr( fptr, colnum, firstrow, firstelem, nelem, 1, &scale, &zero,
        tform, &twidth, &tcode, &maxelem, &startpos,  &elemnum, &incre,
        &repeat, &rowlen, &hdutype, &tnull, snull, status) > 0)
        return(*status);

    if (tcode == TSTRING)   
         ffcfmt(tform, cform);     /* derive C format for writing strings */

    /*
      if there is no scaling and the native machine format is not byteswapped,
      then we can simply write the raw data bytes into the FITS file if the
      datatype of the FITS column is the same as the input values.  Otherwise,
      we must convert the raw values into the scaled and/or machine dependent
      format in a temporary buffer that has been allocated for this purpose.
    */
    if (scale == 1. && zero == 0. && 
       MACHINE == NATIVE && tcode == TDOUBLE)
    {
        writeraw = 1;
        maxelem = nelem;  /* we can write the entire array at one time */
    }
    else
        writeraw = 0;

    /*---------------------------------------------------------------------*/
    /*  Now write the pixels to the FITS column.                           */
    /*  First call the ffXXfYY routine to  (1) convert the datatype        */
    /*  if necessary, and (2) scale the values by the FITS TSCALn and      */
    /*  TZEROn linear scaling parameters into a temporary buffer.          */
    /*---------------------------------------------------------------------*/
    remain = nelem;           /* remaining number of values to write  */
    next = 0;                 /* next element in array to be written  */
    rownum = 0;               /* row number, relative to firstrow     */
    ntodo = remain;           /* number of elements to write at one time */

    while (ntodo)
    {
        /* limit the number of pixels to process a one time to the number that
           will fit in the buffer space or to the number of pixels that remain
           in the current vector, which ever is smaller.
        */
        ntodo = minvalue(ntodo, maxelem);      
        ntodo = minvalue(ntodo, (repeat - elemnum));

        wrtptr = startpos + (rownum * rowlen) + (elemnum * incre);

        ffmbyt(fptr, wrtptr, IGNORE_EOF, status); /* move to write position */

        switch (tcode) 
        {
            case (TDOUBLE):
              if (writeraw)
              {
                /* write raw input bytes without conversion */
                ffpr8b(fptr, ntodo, incre, &array[next], status);
              }
              else
              {
                /* convert the raw data before writing to FITS file */
                ffr8fr8(&array[next], ntodo, scale, zero,
                        (double *) buffer, status);
                ffpr8b(fptr, ntodo, incre, (double *) buffer, status);
              }

              break;

            case (TBYTE):
 
                ffr8fi1(&array[next], ntodo, scale, zero, 
                        (unsigned char *) buffer, status);
                ffpi1b(fptr, ntodo, incre, (unsigned char *) buffer, status);
                break;

            case (TSHORT):

                ffr8fi2(&array[next], ntodo, scale, zero, 
                       (short *) buffer, status);
                ffpi2b(fptr, ntodo, incre, (short *) buffer, status);
                break;

            case (TLONG):

                ffr8fi4(&array[next], ntodo, scale, zero,
                        (long *) buffer, status);
                ffpi4b(fptr, ntodo, incre, (long *) buffer, status);
                break;

            case (TFLOAT):
                ffr8fr4(&array[next], ntodo, scale, zero,
                        (float *) buffer, status);
                ffpr4b(fptr, ntodo, incre, (float *) buffer, status);
                break;

            case (TSTRING):  /* numerical column in an ASCII table */

                if (cform[1] != 's')  /*  "%s" format is a string */
                {
                  dvalue = (array[next] - zero) / scale; /* inverse scale */
                  sprintf(cstring, cform, dvalue);
                  ffpbyt(fptr, twidth, cstring, status);
                  break;
                }
                /* can't write to string column, so fall thru to default: */

            default:  /*  error trap  */
                sprintf(message, 
                      "Cannot write numbers to column %d which has format %s",
                       colnum,tform);
                ffpmsg(message);
                if (hdutype == ASCII_TBL)
                    return(*status = BAD_ATABLE_FORMAT);
                else
                    return(*status = BAD_BTABLE_FORMAT);

        } /* End of switch block */

        /*-------------------------*/
        /*  Check for fatal error  */
        /*-------------------------*/
        if (*status > 0)  /* test for error during previous write operation */
        {
         sprintf(message,
          "Error writing elements %ld thru %ld of input data array (ffpcld).",
              next+1, next+ntodo);
         ffpmsg(message);
         return(*status);
        }

        /*--------------------------------------------*/
        /*  increment the counters for the next loop  */
        /*--------------------------------------------*/
        remain -= ntodo;
        if (remain)
        {
            next += ntodo;
            elemnum += ntodo;
            if (elemnum == repeat)  /* completed a row; start on next row */
            {
                elemnum = 0;
                rownum++;
            }
        }
        ntodo = remain;  /* this is the maximum number to do in next loop */

    }  /*  End of main while Loop  */


    /*--------------------------------*/
    /*  check for numerical overflow  */
    /*--------------------------------*/
    if (*status == OVERFLOW_ERR)
    {
        ffpmsg(
        "Numerical overflow during type conversion while writing FITS data.");
        *status = NUM_OVERFLOW;
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpclu( fitsfile *fptr,  /* I - FITS file pointer                       */
            int  colnum,     /* I - number of column to write (1 = 1st col) */
            long  firstrow,  /* I - first row to write (1 = 1st row)        */
            long  firstelem, /* I - first vector element to write (1 = 1st) */
            long  nelem,     /* I - number of values to write               */
            int  *status)    /* IO - error status                           */
/*
  Set elements of a table column to the appropriate null value for the column
  The column number may refer to a real column in an ASCII or binary table, 
  or it may refer to a virtual column in a 1 or more grouped FITS primary
  array.  FITSIO treats a primary array as a binary table
  with 2 vector columns: the first column contains the group parameters (often
  with length = 0) and the second column contains the array of image pixels.
  Each row of the table represents a group in the case of multigroup FITS
  images.
*/
{
    int tcode, maxelem, hdutype, lennull, nwrite;
    short i2null;
    long twidth, incre, repeat, rowlen, rownum, elemnum, remain, next, ntodo;
    long tnull, startpos, wrtptr, ii;
    double scale, zero;
    unsigned char i1null, lognul = 0;
    char tform[20], cstring[50];
    char message[FLEN_ERRMSG];
    char snull[20];   /*  the FITS null value  */
    long   jbuff[2] = { -1, -1};  /* all bits set is equivalent to a NaN */

    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    /*---------------------------------------------------*/
    /*  Check input and get parameters about the column: */
    /*---------------------------------------------------*/

    /* note that the 5th parameter = 1, not nelem, so that the returned */
    /* repeat and incre values will be the actual values for this column */

    /* note that 6th parameter = 0 because if writing nulls to a variable   */
    /* length column then dummy data values must have already been written  */
    /* to the heap.  We now just have to overwrite the previous values with */
    /* null values.                                                         */
    if (ffgcpr( fptr, colnum, firstrow, firstelem, 1, 0, &scale, &zero,
        tform, &twidth, &tcode, &maxelem, &startpos,  &elemnum, &incre,
        &repeat, &rowlen, &hdutype, &tnull, snull, status) > 0)
        return(*status);

    if (tcode == TSTRING)
    {
      if (snull[0] == ASCII_NULL_UNDEFINED)
      {
        ffpmsg(
        "Null value string for ASCII table column is not defined (FTPCLU).");
        return(*status = NO_NULL);
      }

      strcpy(cstring, snull);          /* copy null string to temp buffer */
      lennull = strlen(cstring);

      if (hdutype == BINARY_TBL)
      {  /* write up to and including null terminator into BINTABLE column */
         nwrite = minvalue(twidth, lennull + 1);
      }
      else
      {  /* write up to 20 chars to ASCII table column; pad with blanks */
         nwrite = minvalue(twidth, 20);

         for (ii = lennull; ii < nwrite; ii++)
            cstring[ii] = ' ';  
      }
    }
    else if ( tcode == TBYTE ||
              tcode == TSHORT ||
              tcode == TLONG ) 
    {
      if (tnull == NULL_UNDEFINED)
      {
        ffpmsg(
        "Null value for integer table column is not defined (FTPCLU).");
        return(*status = NO_NULL);
      }

      if (tcode == TBYTE)
         i1null = tnull;
      else if (tcode == TSHORT)
      {
         i2null = tnull;
#if BYTESWAPPED == TRUE
         ffswap2(&i2null, 1); /* reverse order of bytes */
#endif
      }
      else
      {
#if BYTESWAPPED == TRUE
         ffswaplong(&tnull, 1); /* reverse order of bytes */
#endif
      }
    }

    /*---------------------------------------------------------------------*/
    /*  Now write the pixels to the FITS column.                           */
    /*---------------------------------------------------------------------*/
    remain = nelem;           /* remaining number of values to write  */
    next = 0;                 /* next element in array to be written  */
    rownum = 0;               /* row number, relative to firstrow     */
    ntodo = remain;           /* number of elements to write at one time */

    while (ntodo)
    {
        /* limit the number of pixels to process a one time to the number that
           will fit in the buffer space or to the number of pixels that remain
           in the current vector, which ever is smaller.
        */
        ntodo = minvalue(ntodo, (repeat - elemnum));
        wrtptr = startpos + (rownum * rowlen) + (elemnum * incre);

        ffmbyt(fptr, wrtptr, IGNORE_EOF, status); /* move to write position */

        switch (tcode) 
        {
            case (TBYTE):
 
                for (ii = 0; ii < ntodo; ii++)
                  ffpbyt(fptr, 1,  &i1null, status);
                break;

            case (TSHORT):

                for (ii = 0; ii < ntodo; ii++)
                  ffpbyt(fptr, 2, &i2null, status);
                break;

            case (TLONG):

                for (ii = 0; ii < ntodo; ii++)
                  ffpbyt(fptr, 4, &tnull, status);
                break;

            case (TFLOAT):

                for (ii = 0; ii < ntodo; ii++)
                  ffpbyt(fptr, 4, jbuff, status);
                break;

            case (TDOUBLE):

                for (ii = 0; ii < ntodo; ii++)
                  ffpbyt(fptr, 8, jbuff, status);
                break;

            case (TLOGICAL):
 
                for (ii = 0; ii < ntodo; ii++)
                  ffpbyt(fptr, 1, &lognul, status);
                break;

            case (TSTRING):  /* an ASCII table column */

                ffpbyt(fptr, nwrite, cstring, status);
                break;

            default:  /*  error trap  */
                sprintf(message, 
                   "Cannot write null value to column %d which has format %s",
                     colnum,tform);
                ffpmsg(message);
                return(*status);

        } /* End of switch block */

        /*-------------------------*/
        /*  Check for fatal error  */
        /*-------------------------*/
        if (*status > 0)  /* test for error during previous write operation */
        {
           sprintf(message,
             "Error writing %ld thru %ld of null values (ffpclu).",
              next+1, next+ntodo);
           ffpmsg(message);
           return(*status);
        }

        /*--------------------------------------------*/
        /*  increment the counters for the next loop  */
        /*--------------------------------------------*/
        remain -= ntodo;
        if (remain)
        {
            next += ntodo;
            elemnum += ntodo;
            if (elemnum == repeat)  /* completed a row; start on next row */
            {
                elemnum = 0;
                rownum++;
            }
        }
        ntodo = remain;  /* this is the maximum number to do in next loop */

    }  /*  End of main while Loop  */

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpclx( fitsfile *fptr,  /* I - FITS file pointer                       */
            int  colnum,     /* I - number of column to write (1 = 1st col) */
            long  frow,      /* I - first row to write (1 = 1st row)        */
            long  fbit,      /* I - first bit to write (1 = 1st)            */
            long  nbit,      /* I - number of bits to write                 */
            char *larray,    /* I - array of logicals corresponding to bits */
            int  *status)    /* IO - error status                           */
/*
  write an array of logical values to a specified bit or byte
  column of the binary table.   If larray is TRUE, then the corresponding
  bit is set to 1, otherwise it is set to 0.
  The binary table column being written to must have datatype 'B' or 'X'. 
*/
{
    long bstart, offset, fbyte, bitloc, ndone;
    long ii, repeat, rstart, estart;
    int tcode, descrp;
    unsigned char cbuff;
    static unsigned char onbit[8] = {128,  64,  32,  16,   8,   4,   2,   1};
    static unsigned char offbit[8] = {127, 191, 223, 239, 247, 251, 253, 254};
    tcolumn *colptr;

    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    /*  check input parameters */
    if (nbit < 1)
        return(*status);
    else if (frow < 1)
        return(*status = BAD_ROW_NUM);
    else if (fbit < 1)
        return(*status = BAD_ELEM_NUM);

    fbyte = (fbit + 7) / 8;
    bitloc = fbit - 1 - ((fbit - 1) / 8 * 8);
    ndone = 0;
    rstart = frow - 1;
    estart = fbyte - 1;

    colptr  = fptr->tableptr;   /* point to first column */
    colptr += (colnum - 1);     /* offset to correct column structure */

    tcode = colptr->tdatatype;

    if (abs(tcode) > TBYTE)
        return(*status = NOT_LOGICAL_COL); /* not correct datatype column */

    if (tcode > 0)
    {
        descrp = FALSE;  /* not a variable length descriptor column */
        repeat = colptr->trepeat;

        if (tcode == TBIT)
            repeat = (repeat + 7) / 8; /* convert from bits to bytes */

        if (fbyte > repeat)
            return(*status = BAD_ELEM_NUM);

        /* calc the i/o pointer location to start of sequence of pixels */
        bstart = fptr->datastart + (rstart * fptr->rowlength) +
               colptr->tbcol + estart;
    }
    else
    {
        descrp = TRUE;  /* a variable length descriptor column */
        /* only bit arrays (tform = 'X') are supported for variable */
        /* length arrays.  REPEAT is the number of BITS in the array. */

        repeat = fbit + nbit -1;
        offset = fptr->heapsize;
        /* write the number of elements and the starting offset */
        ffpdes(fptr, colnum, frow, repeat, offset, status);
        /* calc the i/o pointer location to start of sequence of pixels */
        bstart = fptr->datastart + offset + fptr->heapstart + estart;
        /* increment the empty heap starting address (in bytes) */
        repeat = (repeat + 7) / 8;  /* convert from bits to bytes */
        fptr->heapsize += repeat;
    }

    /* move the i/o pointer to the start of the pixel sequence */
    ffmbyt(fptr, bstart, IGNORE_EOF, status);

    /* read the next byte (we may only be modifying some of the bits) */
    while (1)
    {
      if (ffgbyt(fptr, 1, &cbuff, status) == READ_ERROR)
      {
        /* hit end of file trying to read the byte, so just set byte = 0 */
        *status = 0;
        cbuff = 0;
      }

      /* move back, to be able to overwrite the byte */
      ffmbyt(fptr, bstart, REPORT_EOF, status);
 
      for (ii = bitloc; (ii < 8) && (ndone < nbit); ii++, ndone++)
      {
        if(larray[ndone])
          cbuff = cbuff | onbit[ii];
        else
          cbuff = cbuff & offbit[ii];
      }

      ffpbyt(fptr, 1, &cbuff, status); /* write the modified byte */

      if (ndone == nbit)  /* finished all the bits */
        return(*status);

      /* not done, so get the next byte */
      bstart++;
      if (!descrp)
      {
        estart++;
        if (estart == repeat)
        {
          /* move the i/o pointer to the next row of pixels */
          estart = 0;
          rstart = rstart + 1;
          bstart = fptr->datastart + (rstart * fptr->rowlength) +
               colptr->tbcol;

          ffmbyt(fptr, bstart, IGNORE_EOF, status);
        }
      }
      bitloc = 0;
    }
}
/*--------------------------------------------------------------------------*/
int ffpclc( fitsfile *fptr,  /* I - FITS file pointer                       */
            int  colnum,     /* I - number of column to write (1 = 1st col) */
            long  firstrow,  /* I - first row to write (1 = 1st row)        */
            long  firstelem, /* I - first vector element to write (1 = 1st) */
            long  nelem,     /* I - number of values to write               */
            float *array,    /* I - array of values to write                */
            int  *status)    /* IO - error status                           */
/*
  Write an array of complex values to a column in the current FITS HDU.
  Each complex number if interpreted as a pair of float values.
  The column number may refer to a real column in an ASCII or binary table, 
  or it may refer to a virtual column in a 1 or more grouped FITS primary
  array.  FITSIO treats a primary array as a binary table
  with 2 vector columns: the first column contains the group parameters (often
  with length = 0) and the second column contains the array of image pixels.
  Each row of the table represents a group in the case of multigroup FITS
  images.

  The input array of values will be converted to the datatype of the column
  if necessary, but normally complex values should only be written to a binary
  table with TFORMn = 'rC' where r is an optional repeat count. The TSCALn and
  TZERO keywords should not be used with complex numbers because mathmatically
  the scaling should only be applied to the real (first) component of the
  complex value.
*/
{
    /* simply multiply the number of elements by 2, and call ffpcle */

    ffpcle(fptr, colnum, firstrow, firstelem, nelem * 2, array, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpclm( fitsfile *fptr,   /* I - FITS file pointer                       */
            int  colnum,      /* I - number of column to write (1 = 1st col) */
            long  firstrow,   /* I - first row to write (1 = 1st row)        */
            long  firstelem,  /* I - first vector element to write (1 = 1st) */
            long  nelem,      /* I - number of values to write               */
            double *array,    /* I - array of values to write                */
            int  *status)     /* IO - error status                           */
/*
  Write an array of double complex values to a column in the current FITS HDU.
  Each complex number if interpreted as a pair of float values.
  The column number may refer to a real column in an ASCII or binary table, 
  or it may refer to a virtual column in a 1 or more grouped FITS primary
  array.  FITSIO treats a primary array as a binary table
  with 2 vector columns: the first column contains the group parameters (often
  with length = 0) and the second column contains the array of image pixels.
  Each row of the table represents a group in the case of multigroup FITS
  images.

  The input array of values will be converted to the datatype of the column
  if necessary, but normally complex values should only be written to a binary
  table with TFORMn = 'rM' where r is an optional repeat count. The TSCALn and
  TZERO keywords should not be used with complex numbers because mathmatically
  the scaling should only be applied to the real (first) component of the
  complex value.
*/
{
    /* simply multiply the number of elements by 2, and call ffpcld */

    ffpcld(fptr, colnum, firstrow, firstelem, nelem * 2, array, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpcnb( fitsfile *fptr,  /* I - FITS file pointer                       */
            int  colnum,     /* I - number of column to write (1 = 1st col) */
            long  firstrow,  /* I - first row to write (1 = 1st row)        */
            long  firstelem, /* I - first vector element to write (1 = 1st) */
            long  nelem,     /* I - number of values to write               */
            unsigned char *array,   /* I - array of values to write         */
            unsigned char nulvalue, /* I - flag for undefined pixels        */
            int  *status)    /* IO - error status                           */
/*
  Write an array of elements to the specified column of a table.  Any input
  pixels equal to the value of nulvalue will be replaced by the appropriate
  null value in the output FITS file. 

  The input array of values will be converted to the datatype of the column 
  and will be inverse-scaled by the FITS TSCALn and TZEROn values if necessary
*/
{
    tcolumn *colptr;
    long repeat, first, ngood = 0, nbad = 0, ii, fstelm, fstrow;

    if (*status > 0)
        return(*status);

    if (fptr->datastart == DATA_UNDEFINED)
        if ( ffrdef(fptr, status) > 0)               /* rescan header */
            return(*status);

    colptr  = fptr->tableptr;   /* point to first column */
    colptr += (colnum - 1);     /* offset to correct column structure */

    repeat = colptr->trepeat;  /* repeat count for this column */

    /* absolute element number in the column */
    first = (firstrow - 1) * repeat + firstelem;

    for (ii = 0; ii < nelem; ii++)
    {
      if (array[ii] != nulvalue)  /* is this a good pixel? */
      {
         if (nbad)  /* write previous string of bad pixels */
         {
            fstelm = ii - nbad + first;  /* absolute element number */
            fstrow = (fstelm - 1) / repeat + 1;  /* starting row number */
            fstelm = fstelm - (fstrow - 1) * repeat;  /* relative number */
            if (ffpclu(fptr, colnum, fstrow, fstelm, nbad, status) > 0)
                return(*status);

            nbad=0;
         }

         ngood = ngood + 1;  /* the consecutive number of good pixels */
      }
      else
      {
         if (ngood)  /* write previous string of good pixels */
         {
            fstelm = ii - ngood + first;  /* absolute element number */
            fstrow = (fstelm - 1) / repeat + 1;  /* starting row number */
            fstelm = fstelm - (fstrow - 1) * repeat;  /* relative number */

            if (ffpclb(fptr, colnum, fstrow, fstelm, ngood, &array[ii-ngood],
                status) > 0)
                return(*status);

            ngood=0;
         }

         nbad = nbad + 1;  /* the consecutive number of bad pixels */
      }
    }

    /* finished loop;  now just write the last set of pixels */

    if (ngood)  /* write last string of good pixels */
    {
      fstelm = ii - ngood + first;  /* absolute element number */
      fstrow = (fstelm - 1) / repeat + 1;  /* starting row number */
      fstelm = fstelm - (fstrow - 1) * repeat;  /* relative number */

      ffpclb(fptr, colnum, fstrow, fstelm, ngood, &array[ii-ngood], status);
    }
    else  /* write last string of bad pixels */
    {
      fstelm = ii - nbad + first;  /* absolute element number */
      fstrow = (fstelm - 1) / repeat + 1;  /* starting row number */
      fstelm = fstelm - (fstrow - 1) * repeat;  /* relative number */

      ffpclu(fptr, colnum, fstrow, fstelm, nbad, status);
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpcni( fitsfile *fptr,  /* I - FITS file pointer                       */
            int  colnum,     /* I - number of column to write (1 = 1st col) */
            long  firstrow,  /* I - first row to write (1 = 1st row)        */
            long  firstelem, /* I - first vector element to write (1 = 1st) */
            long  nelem,     /* I - number of values to write               */
            short *array,    /* I - array of values to write                */
            short  nulvalue, /* I - value used to flag undefined pixels     */
            int  *status)    /* IO - error status                           */
/*
  Write an array of elements to the specified column of a table.  Any input
  pixels equal to the value of nulvalue will be replaced by the appropriate
  null value in the output FITS file. 

  The input array of values will be converted to the datatype of the column 
  and will be inverse-scaled by the FITS TSCALn and TZEROn values if necessary
*/
{
    tcolumn *colptr;
    long repeat, first, ngood = 0, nbad = 0, ii, fstelm, fstrow;

    if (*status > 0)
        return(*status);

    if (fptr->datastart == DATA_UNDEFINED)
        if ( ffrdef(fptr, status) > 0)               /* rescan header */
            return(*status);

    colptr  = fptr->tableptr;   /* point to first column */
    colptr += (colnum - 1);     /* offset to correct column structure */

    repeat = colptr->trepeat;  /* repeat count for this column */

    /* absolute element number in the column */
    first = (firstrow - 1) * repeat + firstelem;

    for (ii = 0; ii < nelem; ii++)
    {
      if (array[ii] != nulvalue)  /* is this a good pixel? */
      {
         if (nbad)  /* write previous string of bad pixels */
         {
            fstelm = ii - nbad + first;  /* absolute element number */
            fstrow = (fstelm - 1) / repeat + 1;  /* starting row number */
            fstelm = fstelm - (fstrow - 1) * repeat;  /* relative number */

            if (ffpclu(fptr, colnum, fstrow, fstelm, nbad, status) > 0)
                return(*status);

            nbad=0;
         }

         ngood = ngood +1;  /* the consecutive number of good pixels */
      }
      else
      {
         if (ngood)  /* write previous string of good pixels */
         {
            fstelm = ii - ngood + first;  /* absolute element number */
            fstrow = (fstelm - 1) / repeat + 1;  /* starting row number */
            fstelm = fstelm - (fstrow - 1) * repeat;  /* relative number */

            if (ffpcli(fptr, colnum, fstrow, fstelm, ngood, &array[ii-ngood],
                status) > 0)
                return(*status);

            ngood=0;
         }

         nbad = nbad +1;  /* the consecutive number of bad pixels */
      }
    }

    /* finished loop;  now just write the last set of pixels */

    if (ngood)  /* write last string of good pixels */
    {
      fstelm = ii - ngood + first;  /* absolute element number */
      fstrow = (fstelm - 1) / repeat + 1;  /* starting row number */
      fstelm = fstelm - (fstrow - 1) * repeat;  /* relative number */

      ffpcli(fptr, colnum, fstrow, fstelm, ngood, &array[ii-ngood], status);
    }
    else  /* write last string of bad pixels */
    {
      fstelm = ii - nbad + first;  /* absolute element number */
      fstrow = (fstelm - 1) / repeat + 1;  /* starting row number */
      fstelm = fstelm - (fstrow - 1) * repeat;  /* relative number */

      ffpclu(fptr, colnum, fstrow, fstelm, nbad, status);
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpcnj( fitsfile *fptr,  /* I - FITS file pointer                       */
            int  colnum,     /* I - number of column to write (1 = 1st col) */
            long  firstrow,  /* I - first row to write (1 = 1st row)        */
            long  firstelem, /* I - first vector element to write (1 = 1st) */
            long  nelem,     /* I - number of values to write               */
            long  *array,    /* I - array of values to write                */
            long   nulvalue, /* I - value used to flag undefined pixels     */
            int  *status)    /* IO - error status                           */
/*
  Write an array of elements to the specified column of a table.  Any input
  pixels equal to the value of nulvalue will be replaced by the appropriate
  null value in the output FITS file. 

  The input array of values will be converted to the datatype of the column 
  and will be inverse-scaled by the FITS TSCALn and TZEROn values if necessary
*/
{
    tcolumn *colptr;
    long repeat, first, ngood = 0, nbad = 0, ii, fstelm, fstrow;

    if (*status > 0)
        return(*status);

    if (fptr->datastart == DATA_UNDEFINED)
        if ( ffrdef(fptr, status) > 0)               /* rescan header */
            return(*status);

    colptr  = fptr->tableptr;   /* point to first column */
    colptr += (colnum - 1);     /* offset to correct column structure */

    repeat = colptr->trepeat;  /* repeat count for this column */

    /* absolute element number in the column */
    first = (firstrow - 1) * repeat + firstelem;

    for (ii = 0; ii < nelem; ii++)
    {
      if (array[ii] != nulvalue)  /* is this a good pixel? */
      {
         if (nbad)  /* write previous string of bad pixels */
         {
            fstelm = ii - nbad + first;  /* absolute element number */
            fstrow = (fstelm - 1) / repeat + 1;  /* starting row number */
            fstelm = fstelm - (fstrow - 1) * repeat;  /* relative number */

            if (ffpclu(fptr, colnum, fstrow, fstelm, nbad, status) > 0)
                return(*status);

            nbad=0;
         }

         ngood = ngood +1;  /* the consecutive number of good pixels */
      }
      else
      {
         if (ngood)  /* write previous string of good pixels */
         {
            fstelm = ii - ngood + first;  /* absolute element number */
            fstrow = (fstelm - 1) / repeat + 1;  /* starting row number */
            fstelm = fstelm - (fstrow - 1) * repeat;  /* relative number */

            if (ffpclj(fptr, colnum, fstrow, fstelm, ngood, &array[ii-ngood],
                status) > 0)
                return(*status);

            ngood=0;
         }

         nbad = nbad +1;  /* the consecutive number of bad pixels */
      }
    }

    /* finished loop;  now just write the last set of pixels */

    if (ngood)  /* write last string of good pixels */
    {
      fstelm = ii - ngood + first;  /* absolute element number */
      fstrow = (fstelm - 1) / repeat + 1;  /* starting row number */
      fstelm = fstelm - (fstrow - 1) * repeat;  /* relative number */

      ffpclj(fptr, colnum, fstrow, fstelm, ngood, &array[ii-ngood], status);
    }
    else  /* write last string of bad pixels */
    {
      fstelm = ii - nbad + first;  /* absolute element number */
      fstrow = (fstelm - 1) / repeat + 1;  /* starting row number */
      fstelm = fstelm - (fstrow - 1) * repeat;  /* relative number */

      ffpclu(fptr, colnum, fstrow, fstelm, nbad, status);
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpcne( fitsfile *fptr,  /* I - FITS file pointer                       */
            int  colnum,     /* I - number of column to write (1 = 1st col) */
            long  firstrow,  /* I - first row to write (1 = 1st row)        */
            long  firstelem, /* I - first vector element to write (1 = 1st) */
            long  nelem,     /* I - number of values to write               */
            float *array,    /* I - array of values to write                */
            float  nulvalue, /* I - value used to flag undefined pixels     */
            int  *status)    /* IO - error status                           */
/*
  Write an array of elements to the specified column of a table.  Any input
  pixels equal to the value of nulvalue will be replaced by the appropriate
  null value in the output FITS file. 

  The input array of values will be converted to the datatype of the column 
  and will be inverse-scaled by the FITS TSCALn and TZEROn values if necessary
*/
{
    tcolumn *colptr;
    long repeat, first, ngood = 0, nbad = 0, ii, fstelm, fstrow;

    if (*status > 0)
        return(*status);

    if (fptr->datastart == DATA_UNDEFINED)
        if ( ffrdef(fptr, status) > 0)               /* rescan header */
            return(*status);

    colptr  = fptr->tableptr;   /* point to first column */
    colptr += (colnum - 1);     /* offset to correct column structure */

    repeat = colptr->trepeat;  /* repeat count for this column */

    /* absolute element number in the column */
    first = (firstrow - 1) * repeat + firstelem;

    for (ii = 0; ii < nelem; ii++)
    {
      if (array[ii] != nulvalue)  /* is this a good pixel? */
      {
         if (nbad)  /* write previous string of bad pixels */
         {
            fstelm = ii - nbad + first;  /* absolute element number */
            fstrow = (fstelm - 1) / repeat + 1;  /* starting row number */
            fstelm = fstelm - (fstrow - 1) * repeat;  /* relative number */

            if (ffpclu(fptr, colnum, fstrow, fstelm, nbad, status) > 0)
                return(*status);

            nbad=0;
         }

         ngood = ngood +1;  /* the consecutive number of good pixels */
      }
      else
      {
         if (ngood)  /* write previous string of good pixels */
         {
            fstelm = ii - ngood + first;  /* absolute element number */
            fstrow = (fstelm - 1) / repeat + 1;  /* starting row number */
            fstelm = fstelm - (fstrow - 1) * repeat;  /* relative number */

            if (ffpcle(fptr, colnum, fstrow, fstelm, ngood, &array[ii-ngood],
                status) > 0)
                return(*status);

            ngood=0;
         }

         nbad = nbad +1;  /* the consecutive number of bad pixels */
      }
    }

    /* finished loop;  now just write the last set of pixels */

    if (ngood)  /* write last string of good pixels */
    {
      fstelm = ii - ngood + first;  /* absolute element number */
      fstrow = (fstelm - 1) / repeat + 1;  /* starting row number */
      fstelm = fstelm - (fstrow - 1) * repeat;  /* relative number */

      ffpcle(fptr, colnum, fstrow, fstelm, ngood, &array[ii-ngood], status);
    }
    else  /* write last string of bad pixels */
    {
      fstelm = ii - nbad + first;  /* absolute element number */
      fstrow = (fstelm - 1) / repeat + 1;  /* starting row number */
      fstelm = fstelm - (fstrow - 1) * repeat;  /* relative number */

      ffpclu(fptr, colnum, fstrow, fstelm, nbad, status);
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpcnd( fitsfile *fptr,  /* I - FITS file pointer                       */
            int  colnum,     /* I - number of column to write (1 = 1st col) */
            long  firstrow,  /* I - first row to write (1 = 1st row)        */
            long  firstelem, /* I - first vector element to write (1 = 1st) */
            long  nelem,     /* I - number of values to write               */
            double *array,   /* I - array of values to write                */
            double nulvalue, /* I - value used to flag undefined pixels     */
            int  *status)    /* IO - error status                           */
/*
  Write an array of elements to the specified column of a table.  Any input
  pixels equal to the value of nulvalue will be replaced by the appropriate
  null value in the output FITS file. 

  The input array of values will be converted to the datatype of the column 
  and will be inverse-scaled by the FITS TSCALn and TZEROn values if necessary
*/
{
    tcolumn *colptr;
    long repeat, first, ngood = 0, nbad = 0, ii, fstelm, fstrow;

    if (*status > 0)
        return(*status);

    if (fptr->datastart == DATA_UNDEFINED)
        if ( ffrdef(fptr, status) > 0)               /* rescan header */
            return(*status);

    colptr  = fptr->tableptr;   /* point to first column */
    colptr += (colnum - 1);     /* offset to correct column structure */

    repeat = colptr->trepeat;  /* repeat count for this column */

    /* absolute element number in the column */
    first = (firstrow - 1) * repeat + firstelem;

    for (ii = 0; ii < nelem; ii++)
    {
      if (array[ii] != nulvalue)  /* is this a good pixel? */
      {
         if (nbad)  /* write previous string of bad pixels */
         {
            fstelm = ii - nbad + first;  /* absolute element number */
            fstrow = (fstelm - 1) / repeat + 1;  /* starting row number */
            fstelm = fstelm - (fstrow - 1) * repeat;  /* relative number */

            if (ffpclu(fptr, colnum, fstrow, fstelm, nbad, status) > 0)
                return(*status);

            nbad=0;
         }

         ngood = ngood +1;  /* the consecutive number of good pixels */
      }
      else
      {
         if (ngood)  /* write previous string of good pixels */
         {
            fstelm = ii - ngood + first;  /* absolute element number */
            fstrow = (fstelm - 1) / repeat + 1;  /* starting row number */
            fstelm = fstelm - (fstrow - 1) * repeat;  /* relative number */

            if (ffpcld(fptr, colnum, fstrow, fstelm, ngood, &array[ii-ngood],
                status) > 0)
                return(*status);

            ngood=0;
         }

         nbad = nbad +1;  /* the consecutive number of bad pixels */
      }
    }

    /* finished loop;  now just write the last set of pixels */

    if (ngood)  /* write last string of good pixels */
    {
      fstelm = ii - ngood + first;  /* absolute element number */
      fstrow = (fstelm - 1) / repeat + 1;  /* starting row number */
      fstelm = fstelm - (fstrow - 1) * repeat;  /* relative number */

      ffpcld(fptr, colnum, fstrow, fstelm, ngood, &array[ii-ngood], status);
    }
    else  /* write last string of bad pixels */
    {
      fstelm = ii - nbad + first;  /* absolute element number */
      fstrow = (fstelm - 1) / repeat + 1;  /* starting row number */
      fstelm = fstelm - (fstrow - 1) * repeat;  /* relative number */

      ffpclu(fptr, colnum, fstrow, fstelm, nbad, status);
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpdes(fitsfile *fptr, /* I - FITS file pointer                         */
           int colnum,     /* I - column number (1 = 1st column of table)   */
           long rownum,    /* I - row number (1 = 1st row of table)         */
           long length,    /* I - number of elements in the row             */
           long heapaddr,  /* I - heap pointer to the data                  */
           int *status)    /* IO - error status                             */
/*
  put (write) the variable length vector descriptor to the table.
*/
{
    long bytepos, descript[2];
    tcolumn *colptr;

    if (*status > 0)
        return(*status);

    if (fptr->datastart == DATA_UNDEFINED)
        if ( ffrdef(fptr, status) > 0)               /* rescan header */
            return(*status);

    colptr = fptr->tableptr;  /* point to first column structure */
    colptr += (colnum - 1);   /* offset to the correct column */

    if (colptr->tdatatype >= 0)
        *status = NOT_VARI_LEN;

    else
    {
        bytepos = fptr->datastart + (rownum - 1) * fptr->rowlength +
                  colptr->tbcol;

        ffmbyt(fptr, bytepos, IGNORE_EOF, status); /* move to element */

        descript[0] = length;   /* 1st word of descriptor is the length  */
        descript[1] = heapaddr; /* 2nd word of descriptor is the address */
 
        ffpi4b(fptr, 2, 4, descript, status); /* write the descriptor */
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffptbb(fitsfile *fptr,        /* I - FITS file pointer                 */
           long firstrow,         /* I - starting row (1 = first row)      */
           long firstchar,        /* I - starting byte in row (1=first)    */
           long nchars,           /* I - number of bytes to write          */
           unsigned char *values, /* I - array of bytes to write           */
           int *status)           /* IO - error status                     */
/*
  write a consecutive string of bytes to an ascii or binary table.
  This will span multiple rows of the table if nchars + firstchar is
  greater than the length of a row.
*/
{
    long bytepos;

    if (*status > 0 || nchars < 0)
        return(*status);

    else if (firstrow < 1)
        return(*status=BAD_ROW_NUM);

    else if (firstchar < 1)
        return(*status=BAD_ELEM_NUM);

    if (fptr->datastart < 0)    /* rescan header if data pointer undefined */
        ffrdef(fptr, status);

    /* move the i/o pointer to the start of the sequence of characters */
    bytepos = fptr->datastart + ( (firstrow - 1) * fptr->rowlength )
            + firstchar - 1;

    ffmbyt(fptr, bytepos, IGNORE_EOF, status);
    ffpbyt(fptr, nchars, values, status);  /* write the bytes */

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpi1b(fitsfile *fptr, /* I - FITS file pointer                         */
           long nvals,     /* I - number of pixels in the values array      */
           long incre,     /* I - byte increment between pixels             */
           unsigned char *values, /* I - array of values to write           */
           int *status)    /* IO - error status                             */
/*
  put (write) the array of values to the FITS file, doing machine dependent
  format conversion (e.g. byte-swapping) if necessary.
*/
{
    long ii, offset;

    if (incre == 1)      /* write all the values at once (contiguous bytes) */

        ffpbyt(fptr, nvals, values, status);

    else         /* have to write each value individually (not contiguous ) */
    {
        offset = incre - 1;   /* the number of bytes to move between values */

        ffpbyt(fptr, 1, values, status);    /* write first value */

        for (ii = 1; ii < nvals; ii++)
        {
            ffmoff(fptr, offset, IGNORE_EOF, status); /* offset to next pos */
            ffpbyt(fptr, 1, &values[ii], status);   /* write the next value */
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpi2b(fitsfile *fptr, /* I - FITS file pointer                         */
           long nvals,     /* I - number of pixels in the values array      */
           long incre,     /* I - byte increment between pixels             */
           short *values,  /* I - array of values to write                  */
           int *status)    /* IO - error status                             */
/*
  put (write) the array of values to the FITS file, doing machine dependent
  format conversion (e.g. byte-swapping) if necessary.
*/
{
    long ii, offset;

#if BYTESWAPPED == TRUE

    ffswap2(values, nvals); /* reverse order of bytes in each value */

#endif

    if (incre == 2)      /* write all the values at once (contiguous bytes) */

        ffpbyt(fptr, nvals * 2, values, status);

    else         /* have to write each value individually (not contiguous ) */
    {
        offset = incre - 2;   /* the number of bytes to move between values */

        ffpbyt(fptr, 2, values, status);    /* write first value */

        for (ii = 1; ii < nvals; ii++)
        {
            ffmoff(fptr, offset, IGNORE_EOF, status); /* offset to next pos */
            ffpbyt(fptr, 2, &values[ii], status);   /* write the next value */
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpi4b(fitsfile *fptr, /* I - FITS file pointer                         */
           long nvals,     /* I - number of pixels in the values array      */
           long incre,     /* I - byte increment between pixels             */
           long *values,   /* I - array of values to write                  */
           int *status)    /* IO - error status                             */
/*
  put (write) the array of values to the FITS file, doing machine dependent
  format conversion (e.g. byte-swapping) if necessary.
*/
{
    long ii, offset;
    char *ptr;

#if BYTESWAPPED == TRUE

    ffswaplong(values, nvals); /* reverse order of bytes in each value */

#endif

    if (incre == 4)      /* write all the values at once (contiguous bytes) */

        ffpbyt(fptr, nvals * 4, values, status);

    else         /* have to write each value individually (not contiguous ) */
    {
        offset = incre - 4; /* the number of bytes to move between each value */

        ptr = (char *) values;

        ffpbyt(fptr, 4, ptr, status);    /* write first value */

        for (ii = 1; ii < nvals; ii++)
        {
            ffmoff(fptr, offset, IGNORE_EOF, status); /* offset to next pos */
            ptr += 4;
            ffpbyt(fptr, 4, ptr, status);   /* write the next value */
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpr4b(fitsfile *fptr, /* I - FITS file pointer                         */
           long nvals,     /* I - number of pixels in the values array      */
           long incre,     /* I - byte increment between pixels             */
           float *values,  /* I - array of values to write                  */
           int *status)    /* IO - error status                             */
/*
  put (write) the array of values to the FITS file, doing machine dependent
  format conversion (e.g. byte-swapping) if necessary.
*/
{
    long ii, offset;

#if MACHINE == VAXVMS

    ii = nvals;                      /* call VAX macro routine to convert */
    ieevpr(values, values, &ii);     /* from F float -> IEEE float        */

#elif MACHINE == ALPHAVMS

    if (*(short *) &testfloat == GFLOAT)
    {
        /* convert from VMS FFLOAT float format to IEEE float format */
        for (ii = 0; ii < nvals; ii++)
            values[ii] *= 0.25;

        ffswap2( (short *) values, nvals * 2);  /* swap pairs of bytes */
    }
    else
        ffswapfloat(values, nvals); /* reverse order of bytes in each value */

#elif BYTESWAPPED == TRUE

    ffswapfloat(values, nvals); /* reverse order of bytes in each value */

#endif

    if (incre == 4)      /* write all the values at once (contiguous bytes) */

        ffpbyt(fptr, nvals * 4, values, status);

    else         /* have to write each value individually (not contiguous ) */
    {
        offset = incre - 4;   /* the number of bytes to move between values */

        ffpbyt(fptr, 4, values, status);    /* write first value */

        for (ii = 1; ii < nvals; ii++)
        {
            ffmoff(fptr, offset, IGNORE_EOF, status); /* offset to next pos */
            ffpbyt(fptr, 4, &values[ii], status);   /* write the next value */
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpr8b(fitsfile *fptr, /* I - FITS file pointer                         */
           long nvals,     /* I - number of pixels in the values array      */
           long incre,     /* I - byte increment between pixels             */
           double *values, /* I - array of values to write                  */
           int *status)    /* IO - error status                             */
/*
  put (write) the array of values to the FITS file, doing machine dependent
  format conversion (e.g. byte-swapping) if necessary.
*/
{
    long ii, offset;

#if MACHINE == VAXVMS

    ii = nvals;                      /* call VAX macro routine to convert */
    ieevpd(values, values, &ii);  /* from D float -> IEEE float        */

#elif MACHINE == ALPHAVMS

    if (*(short *) &testfloat ==  GFLOAT)
    {
        /* convert from VMS GFLOAT float format to IEEE float format */
        for (ii = 0; ii < nvals; ii++)
            values[ii] *= 0.25;

        ffswap2( (short *) values, nvals * 4);  /* swap pairs of bytes */
    }
    else
        ffswap8(values, nvals); /* reverse order of bytes in each value */

#elif BYTESWAPPED == TRUE

    ffswap8(values, nvals); /* reverse order of bytes in each value */

#endif

    if (incre == 8)      /* write all the values at once (contiguous bytes) */

        ffpbyt(fptr, nvals * 8, values, status);

    else         /* have to write each value individually (not contiguous ) */
    {
        offset = incre - 8;   /* the number of bytes to move between values */

        ffpbyt(fptr, 8, values, status);    /* write first value */

        for (ii = 1; ii < nvals; ii++)
        {
            ffmoff(fptr, offset, IGNORE_EOF, status); /* offset to next pos */
            ffpbyt(fptr, 8, &values[ii], status);   /* write the next value */
        }
    }
    return(*status);
}

