#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "fitsio.h"

main()
{
    extern str_iter(); /* external work function is passed to the iterator */
    fitsfile *fptr;
    iteratorCol cols[2];
    int n_cols;
    long rows_per_loop, offset;
    int status = 0;
    char filename[]  = "iterstr.fit";     /* name of rate FITS file */

    /* open the file and move to the correct extension */
    fits_open_file(&fptr, filename, READWRITE, &status);
    fits_movnam_hdu(fptr, "iter_test", 0, &status);

    /* define input column structure members for the iterator function */
    n_cols  = 2;   /* number of columns */

    /* both columns are in the same FITS file */
    cols[0].fptr  = fptr;
    cols[1].fptr  = fptr;

    /* define the desired columns by name */
    strcpy(cols[0].colname, "Avalue");    
    strcpy(cols[1].colname, "Lvalue");    

    /* leave column numbers undefined */
    cols[0].colnum = 0;    
    cols[1].colnum = 0;    

    /* define the desired datatype for each column */
    cols[0].datatype = TSTRING;
    cols[1].datatype = TLOGICAL;

    /* define whether columns are input, input/output, or output only */
    cols[0].iotype = InputOutputCol;    
    cols[1].iotype = InputOutputCol;
 
    rows_per_loop = 0;  /* use default optimum number of rows */
    offset = 0;         /* process all the rows */

    /* apply the  function to each row of the table */
    printf("Calling iterator function...%d\n", status);

    fits_iterate_data(n_cols, cols, offset, rows_per_loop,
                      str_iter, 0L, &status);

    fits_close_file(fptr, &status);      /* all done */

    if (status)
       fits_report_error(stderr, status); /* print out error messages */

    return(status);
}
/*--------------------------------------------------------------------------*/
int str_iter(long totalrows, long offset, long firstrow, long nrows,
             int ncols, iteratorCol *cols, void *user_strct )

/*
   Sample interator function.
*/
{
    int ii;

    /* declare variables static to preserve their values between calls */
    static char **stringvals;
    static char *logicalvals;

    /*--------------------------------------------------------*/
    /*  Initialization procedures: execute on the first call  */
    /*--------------------------------------------------------*/
    if (firstrow == 1)
    {
       if (ncols != 2)
           return(-1);  /* number of columns incorrect */

       if (cols[0].datatype != TSTRING ||
           cols[1].datatype != TLOGICAL )
           return(-2);  /* bad data type */

       /* assign the input pointers to the appropriate arrays */
       stringvals       = (char **) cols[0].array;
       logicalvals      = (char *) cols[1].array;

       printf("Total rows, No. rows = %d %d\n",totalrows, nrows);
    }

    /*------------------------------------------*/
    /*  Main loop: process all the rows of data */
    /*------------------------------------------*/

    /*  NOTE: 1st element of array is the null pixel value!  */
    /*  Loop from 1 to nrows, not 0 to nrows - 1.  */
   
    for (ii = 1; ii <= nrows; ii++)
    {
      printf("%s %d\n", stringvals[ii], logicalvals[ii]);
      if (logicalvals[ii])
      {
         logicalvals[ii] = FALSE;
         strcpy(stringvals[ii], "changed to false");
      }
      else
      {
         logicalvals[ii] = TRUE;
         strcpy(stringvals[ii], "changed to true");
      }
    }

    /*-------------------------------------------------------*/
    /*  Clean up procedures:  after processing all the rows  */
    /*-------------------------------------------------------*/

    if (firstrow + nrows - 1 == totalrows)
    {
      /* no action required in this case */
    }
 
    return(0);
}
