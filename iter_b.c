#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "fitsio.h"

main()
{
    extern flux_rate(); /* external work function is passed to the iterator */
    fitsfile *fptr;
    iteratorCol cols[3];
    int n_cols;
    long rows_per_loop, offset;

    int status = 0, nkeys, keypos, hdutype, ii, jj;
    char filename[]  = "iterfile.fit";     /* name of rate FITS file */

    n_cols  = 3;   /* number of columns */

    /* open the file */
    fits_open_file(&fptr, filename, READWRITE, &status);

    /* move to the HDU containing the rate table */
    fits_movnam_hdu(fptr, "RATE", 0, &status);

    /* define input column structure members for the iterator function */

    /* all the columns are in the same FITS file */
    cols[0].fptr  = fptr;
    cols[1].fptr  = fptr;
    cols[2].fptr  = fptr;

    /* define the desired columns by name */
    strcpy(cols[0].colname, "COUNTS");    
    strcpy(cols[1].colname, "TIME");    
    strcpy(cols[2].colname, "RATE");    

    /* leave column numbers undefined */
    cols[0].colnum = 0;    
    cols[1].colnum = 0;    
    cols[2].colnum = 0;    

    /* define the desired datatype for each column */
    /* convert all to doubles */
    cols[0].datatype = TDOUBLE;
    cols[1].datatype = TDOUBLE;
    cols[2].datatype = TDOUBLE;

    /* define whether columns are input, input/output, or output only */
    cols[0].iotype = InputCol;    
    cols[1].iotype = InputCol;    
    cols[2].iotype = OutputCol;

    rows_per_loop = 0;  /* use default optimum number of rows */
    offset = 0;         /* process all the rows */

    /* apply the rate function to each row of the table */
    printf("Calling iterator function...%d\n", status);

    fits_iterate_data(n_cols, cols, offset, rows_per_loop,
                      flux_rate, 0L, &status);

    fits_close_file(fptr, &status);      /* all done */

    if (status)
        fits_report_error(stderr, status);  /* print out error messages */

    return(status);
}
/*--------------------------------------------------------------------------*/
int flux_rate(long totalrows, long offset, long firstrow, long nrows,
             int ncols, iteratorCol *cols, void *user_strct ) 

/*
   Sample interator function that calculates the output flux 'rate' column
   by dividing the input 'counts' by the 'time' column.
   It also applies a constant deadtime correction factor if the 'deadtime'
   keyword exists.  Finally, this creates or updates the 'LIVETIME'
   keyword with the sum of all the individual integration times.
*/
{
    int ii, status = 0;

    /* declare variables static to preserve their values between calls */
    static double *counts;
    static double *interval;
    static double *rate;
    static double deadtime, livetime; /* must preserve values between calls */

    /*--------------------------------------------------------*/
    /*  Initialization procedures: execute on the first call  */
    /*--------------------------------------------------------*/
    if (firstrow == 1)
    {
       if (ncols != 3)
           return(-1);  /* number of columns incorrect */

       if (cols[0].datatype != TDOUBLE ||
           cols[1].datatype != TDOUBLE ||
           cols[2].datatype != TDOUBLE )
           return(-2);  /* bad data type */

       /* assign the input pointers to the appropriate arrays and null ptrs*/
       counts       = (double *) cols[0].array;
       interval     = (double *) cols[1].array;
       rate         = (double *) cols[2].array;

       livetime = 0;  /* initialize the total integration time */

       /* try to get the deadtime keyword value */
       fits_read_key_dbl(cols[0].fptr, "DEADTIME", &deadtime, '\0', &status);
       if (status)
       {
           deadtime = 1.0;  /* default deadtime if keyword doesn't exist */
       }
       else if (deadtime < 0. || deadtime > 1.0)
       {
           return(-1);    /* bad deadtime value */
       }

       printf("deadtime = %f\n", deadtime);
    }

    /*--------------------------------------------*/
    /*  Main loop: process all the rows of data */
    /*--------------------------------------------*/

    /*  NOTE: 1st element of array is the null pixel value!  */
    /*  Loop from 1 to nrows, not 0 to nrows - 1.  */

    /* this version ignores null values */
    rate[0] = 0.;  /* set the output null value to zero to ignore nulls */

    for (ii = 1; ii <= nrows; ii++)
    {
       if (interval[ii] > 0.)
       {
           rate[ii] = counts[ii] / interval[ii] / deadtime;
           livetime += interval[ii];  /* accumulate total integration time */
       }
       else
           return(-2);  /* bad integration time */
    }

    /*-------------------------------------------------------*/
    /*  Clean up procedures:  after processing all the rows  */
    /*-------------------------------------------------------*/

    if (firstrow + nrows - 1 == totalrows)
    {
        /*  update the LIVETIME keyword value */

        fits_update_key_fixdbl(cols[0].fptr, "LIVETIME", livetime, 3, 
                 "total integration time", &status);
        printf("livetime = %f\n", livetime);
   }
    return(0);  /* return successful status */
}
