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
    else if (datatype == TUSHORT)
    {
      ffpclui(fptr, 2, row, firstelem, nelem, (unsigned short *) array,
              status);
    }
    else if (datatype == TSHORT)
    {
      ffpcli(fptr, 2, row, firstelem, nelem, (short *) array, status);
    }
    else if (datatype == TINT)
    {
      ffpclk(fptr, 2, row, firstelem, nelem, (int *) array, status);
    }
    else if (datatype == TULONG)
    {
      ffpcluj(fptr, 2, row, firstelem, nelem, (unsigned long *) array, status);
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
    else if (datatype == TUSHORT)
    {
      ffpclui(fptr, colnum, firstrow, firstelem, nelem, 
             (unsigned short *) array, status);
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
    else if (datatype == TULONG)
    {
      ffpcluj(fptr, colnum, firstrow, firstelem, nelem, (unsigned long *) array,
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

/*--------------------------------------------------------------------------*/
int ffiter( fitsfile *fptr,
            int n_in_cols,    int *in_colnum, 
            int n_inout_cols, int *inout_colnum,
            int n_out_cols,   int *out_colnum,
            int work_fn( fitsfile *fptr, long fstrow, long nrows,
                        void **col_ptrs), int *status)
/*
   The iterator function.  This function will pass the selected
   columns from a FITS table to the user-supplied function.
   Depending on the size of the table, only a subset of the rows
   may be passed to the function on each call, in which case
   the function will be called multiple times until all the
   rows in the table have been processed.
*/
{
    void *col_ptrs[MAX_ITER_COLS];
    int intype[MAX_ITER_COLS], inouttype[MAX_ITER_COLS];
    int  outtype[MAX_ITER_COLS];
    long inrepeat[MAX_ITER_COLS], inoutrepeat[MAX_ITER_COLS];
    long  outrepeat[MAX_ITER_COLS];
    long nrows, frow, optim_rows, ntodo;
    long repeat, width;
    int ii, jj, total_cols, typecode, anynul;

    if (*status > 0)
        return(*status);

    /* make sure this is a table extension */
    ffghdt(fptr, &typecode, status);
    if (typecode == IMAGE_HDU)
        return(*status = NOT_TABLE);

    if (n_in_cols  < 0 || n_inout_cols < 0 || n_out_cols < 0)
    {
        return(*status = BAD_COL_NUM);
    }   

    total_cols = n_in_cols + n_inout_cols + n_out_cols;
    if (total_cols > MAX_ITER_COLS)
    {
        return(*status = ARRAY_TOO_BIG);
    }   

    ffgkyj(fptr, "NAXIS2", &nrows, 0, status);  /* total # of rows */
    ffgrsz(fptr, &optim_rows, status);   /* optimum number of rows */

    /* initialize null pointers */
    for (ii = 0; ii < total_cols; ii++)
        col_ptrs[ii] = 0;

    /* allocate work arrays for each 'in' column */
    for (ii = 0, jj = 0; ii < n_in_cols; ii++, jj++)
    {
        /* get column datatype and vector length */
        ffgtcl(fptr, in_colnum[ii], &typecode, &repeat, &width, status);
        if (*status > 0)
            goto cleanup;

        intype[ii] = typecode;
        inrepeat[ii] = repeat;

        ntodo = optim_rows * repeat;

        switch (typecode) {

        case TBYTE:
          col_ptrs[jj] = malloc(ntodo * sizeof(char));
          break;

        case TSHORT:
          col_ptrs[jj] = malloc(ntodo * sizeof(short));
          break;

        case TLONG:
          col_ptrs[jj] = malloc(ntodo * sizeof(long));
          break;

        case TFLOAT:
          col_ptrs[jj] = malloc(ntodo * sizeof(float));
          break;

        case TDOUBLE:
          col_ptrs[jj] = malloc(ntodo * sizeof(double));
          break;

        default:
          *status = BAD_TFORM;
          goto cleanup;
        }
    }

    /* allocate work arrays for each 'inout' column */
    for (ii = 0; ii < n_inout_cols; ii++, jj++)
    {
        /* get column datatype and vector length */
        ffgtcl(fptr, inout_colnum[ii], &typecode, &repeat, &width, status);
        if (*status > 0)
            goto cleanup;

        inouttype[ii] = typecode;
        inoutrepeat[ii] = repeat;

        ntodo = optim_rows * repeat;

        switch (typecode) {

        case TBYTE:
          col_ptrs[jj] = malloc(ntodo * sizeof(char));
          break;

        case TSHORT:
          col_ptrs[jj] = malloc(ntodo * sizeof(short));
          break;

        case TLONG:
          col_ptrs[jj] = malloc(ntodo * sizeof(long));
          break;

        case TFLOAT:
          col_ptrs[jj] = malloc(ntodo * sizeof(float));
          break;

        case TDOUBLE:
          col_ptrs[jj] = malloc(ntodo * sizeof(double));
          break;

        default:
          *status = BAD_TFORM;
          goto cleanup;
        }
    }

    /* allocate work arrays for each 'out' column */
    for (ii = 0; ii < n_out_cols; ii++, jj++)
    {
        /* get column datatype and vector length */
        ffgtcl(fptr, out_colnum[ii], &typecode, &repeat, &width, status);
        if (*status > 0)
            goto cleanup;

        outtype[ii] = typecode;
        outrepeat[ii] = repeat;

        ntodo = optim_rows * repeat;

        switch (typecode) {

        case TBYTE:
          col_ptrs[jj] = malloc(ntodo * sizeof(char));
          break;

        case TSHORT:
          col_ptrs[jj] = malloc(ntodo * sizeof(short));
          break;

        case TLONG:
          col_ptrs[jj] = malloc(ntodo * sizeof(long));
          break;

        case TFLOAT:
          col_ptrs[jj] = malloc(ntodo * sizeof(float));
          break;

        case TDOUBLE:
          col_ptrs[jj] = malloc(ntodo * sizeof(double));
          break;

        default:
          *status = BAD_TFORM;
          goto cleanup;
        }
    }

    /* check that all the memory allocations were successful */
    for (jj = 0; jj < total_cols; jj++)
    {
        if (!col_ptrs[jj])
        {
            *status = MEMORY_ALLOCATION;
            goto cleanup;
        }
    }

    /* loop while there are rows left to process */
    frow = 1;

    while (nrows)
    {
      ntodo = minvalue(nrows, optim_rows); /* no. of rows for this loop */

      /* read set of rows into the 'in' columns */
      for (jj= 0, ii = 0; ii < n_in_cols; ii++, jj++)
      {
        ffgcv(fptr, intype[ii], in_colnum[ii], frow, 1L,
              inrepeat[ii] * ntodo, 0L, col_ptrs[jj], &anynul, status);
      }

      /* read set of rows into the 'inout' columns */
      for (ii = 0; ii < n_inout_cols; ii++, jj++)
      {
        ffgcv(fptr, inouttype[ii], inout_colnum[ii], frow, 1L,
              inoutrepeat[ii] * ntodo, 0L, col_ptrs[jj], &anynul, status);
      }

      if (*status)
         break;

      /* call iterator function */
      if (*status = work_fn(fptr, frow, ntodo, col_ptrs))
      {
         /* returned an error status */
         break;
      }
     
      /* write 'inout' columns to FITS file */
      for (jj = n_in_cols, ii = 0; ii < n_inout_cols; ii++, jj++)
      {
        ffpcl(fptr, inouttype[ii], inout_colnum[ii], frow, 1L,
              inoutrepeat[ii] * ntodo, col_ptrs[jj], status);
      }

      /* write 'out' columns to FITS file */
      for (ii = 0; ii < n_out_cols; ii++, jj++)
      {
        ffpcl(fptr, outtype[ii], out_colnum[ii], frow, 1L,
              outrepeat[ii] * ntodo, col_ptrs[jj], status);
      }

      if (*status)
         break;

      nrows -= ntodo;
      frow += ntodo;
    }

cleanup:

    /* free work arrays for the columns */
    for (jj = 0; jj < total_cols; jj++)
    {
        free(col_ptrs[jj]);
    }

    return(*status);
}

