/*  This file, putcol.c, contains routines that write data elements to     */
/*  a FITS image or table. These are the generic routines.                 */

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
      ffpclk(fptr, 2, row, firstelem, nelem, (int *) array, status);
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

    if (datatype == TBIT)
    {
      ffpclx(fptr, colnum, firstrow, firstelem, nelem, (char *) array, 
             status);
    }
    else if (datatype == TBYTE)
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
      ffpclk(fptr, colnum, firstrow, firstelem, nelem, (int *) array,
               status);
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
    else if (datatype == TCOMPLEX)
    {
      ffpcle(fptr, colnum, firstrow, (firstelem - 1) * 2 + 1, nelem * 2,
             (float *) array, status);
    }
    else if (datatype == TDBLCOMPLEX)
    {
      ffpcld(fptr, colnum, firstrow, (firstelem - 1) * 2 + 1, nelem * 2,
             (double *) array, status);
    }
    else if (datatype == TLOGICAL)
    {
      ffpcll(fptr, colnum, firstrow, firstelem, nelem, (char *) array,
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

