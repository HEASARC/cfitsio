/*  This file, getcol.c, contains routines that read data elements from    */
/*  a FITS image or table.                                                 */

/*  The FITSIO software was written by William Pence at the High Energy    */
/*  Astrophysic Science Archive Research Center (HEASARC) at the NASA      */
/*  Goddard Space Flight Center.  Users shall not, without prior written   */
/*  permission of the U.S. Government,  establish a claim to statutory     */
/*  copyright.  The Government and others acting on its behalf, shall have */
/*  a royalty-free, non-exclusive, irrevocable,  worldwide license for     */
/*  Government purposes to publish, distribute, translate, copy, exhibit,  */
/*  and perform such material.                                             */

#include <stdlib.h>
#include <string.h>
#include "fitsio2.h"

#if MACHINE == ALPHAVMS
  static float testfloat = TESTFLOAT;  /* use to test floating pt format */
#endif

/*--------------------------------------------------------------------------*/
int ffgcno( fitsfile *fptr,  /* I - FITS file pionter                       */
            int  casesen,    /* I - case sensitive string comparison? 0=no  */
            char *templt,    /* I - input name of column (w/wildcards)      */
            int  *colnum,    /* O - number of the named column; 1=first col */
            int  *status)    /* IO - error status                           */
/*
  Determine the column number corresponding to an input column name.
  The first column of the table = column 1;  
  This supports the * and ? wild cards in the input template.
*/
{
    char colname[FLEN_VALUE];  /*  temporary string to hold column name  */

    ffgcnn(fptr, casesen, templt, colname, colnum, status);

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgcnn( fitsfile *fptr,  /* I - FITS file pointer                       */
            int  casesen,    /* I - case sensitive string comparison? 0=no  */
            char *templt,    /* I - input name of column (w/wildcards)      */
            char *colname,   /* O - full column name up to 68 + 1 chars long*/
            int  *colnum,    /* O - number of the named column; 1=first col */
            int  *status)    /* IO - error status                           */
/*
  Return the full column name and column number of the next column whose
  TTYPEn keyword value matches the input template string.
  The template may contain the * and ? wildcards.  Status = 237 is
  returned if the match is not unique.  If so, one may call this routine
  again with input status=237  to get the next match.  A status value of
  219 is returned when there are no more matching columns.
*/
{
    char errmsg[FLEN_ERRMSG];
    static int startcol;
    int tstatus, ii, founde, foundw, match, exact, unique;
    long ivalue;
    tcolumn *colptr;

    if (*status <= 0)
    {
        startcol = 0;   /* start search with first column */
        tstatus = 0;
    }
    else if (*status == COL_NOT_UNIQUE) /* start search from previous spot */
    {
        tstatus = COL_NOT_UNIQUE;
        *status = 0;
    }
    else
        return(*status);  /* bad input status value */

    colname[0] = 0;    /* initialize null return */
    *colnum = 0;

    if (fptr->datastart == DATA_UNDEFINED)
        if ( ffrdef(fptr, status) > 0)   /* rescan header to get col struct */
            return(*status);

    colptr = fptr->tableptr;   /* pointer to first column */
    colptr += (startcol);      /* offset to starting column */

    founde = FALSE;   /* initialize 'found exact match' flag */
    foundw = FALSE;   /* initialize 'found wildcard match' flag */

    for (ii = startcol; ii < fptr->tfield; ii++, colptr++)
    {
        ffcmps(templt, colptr->ttype, casesen, &match, &exact);
        if (match)
        {
            if (founde && exact)
            {
                /* warning: this is the second exact match we've found     */
                /*reset pointer to first match so next search starts there */
               startcol = *colnum;
               return(*status = COL_NOT_UNIQUE);
            }
            else if (founde)   /* a wildcard match */
            {
                /* already found exact match so ignore this non-exact match */
            }
            else if (exact)
            {
                /* this is the first exact match we have found, so save it. */
                strcpy(colname, colptr->ttype);
                *colnum = ii + 1;
                founde = TRUE;
            }
            else if (foundw)
            {
                /* we have already found a wild card match, so not unique */
                /* continue searching for other matches                   */
                unique = FALSE;
            }
            else
            {
               /* this is the first wild card match we've found. save it */
               strcpy(colname, colptr->ttype);
               *colnum = ii + 1;
               startcol = *colnum;
               foundw = TRUE;
               unique = TRUE;
            }
        }
    }

    /* OK, we've checked all the names now see if we got any matches */
    if (founde)
    {
        if (tstatus == COL_NOT_UNIQUE)  /* we did find 1 exact match but */
            *status = COL_NOT_UNIQUE;   /* there was a previous match too */
    }
    else if (foundw)
    {
        /* found one or more wildcard matches; report error if not unique */
       if (!unique || tstatus == COL_NOT_UNIQUE)
           *status = COL_NOT_UNIQUE;
    }
    else
    {
        /* didn't find a match; check if template is a positive integer */
        ffc2ii(templt, &ivalue, &tstatus);
        if (tstatus ==  0 && ivalue <= fptr->tfield && ivalue > 0)
        {
            *colnum = ivalue;

            colptr = fptr->tableptr;   /* pointer to first column */
            colptr += (ivalue - 1);    /* offset to correct column */
            strcpy(colname, colptr->ttype);
        }
        else
        {
            *status = COL_NOT_FOUND;
            if (tstatus != COL_NOT_UNIQUE)
            {
              sprintf(errmsg, "ffgcnn could not find column: %.45s", templt);
              ffpmsg(errmsg);
            }
        }
    }
    
    startcol = *colnum;  /* save pointer for next time */
    return(*status);
}
/*--------------------------------------------------------------------------*/
void ffcmps(char *templt,   /* I - input template (may have wildcards)      */
            char *colname,  /* I - full column name up to 68 + 1 chars long */
            int  casesen,   /* I - case sensitive string comparison? 1=yes  */
            int  *match,    /* O - do template and colname match? 1=yes     */
            int  *exact)    /* O - do strings exactly match, or wildcards   */
/*
  compare the template to the string and test if they match.
  The strings are limited to 68 characters or less (the max. length
  of a FITS string keyword value.  This routine reports whether
  the two strings match and whether the match is exact or
  involves wildcards.

  This algorithm is very similar to the way unix filename wildcards
  work except that this first treats a wild card as a literal character
  when looking for a match.  If there is no literal match, then
  it interpretes it as a wild card.  So the template 'AB*DE'
  is considered to be an exact rather than a wild card match to
  the string 'AB*DE'.
*/
{
    int found, t1, s1;
    char temp[FLEN_VALUE], col[FLEN_VALUE];

    *match = FALSE;
    *exact = TRUE;

    strncpy(temp, templt, FLEN_VALUE); /* copy strings to work area */
    strncpy(col, colname, FLEN_VALUE);
    temp[FLEN_VALUE -1] = '\0';  /* make sure strings are teminated */
    col[FLEN_VALUE -1]  = '\0';

    if (!casesen)
    {             /* convert both strings to uppercase before comparison */
        ffupch(temp);
        ffupch(col);
    }

    if (!strcmp(temp, col) )
    {
        *match = TRUE;     /* strings exactly match */
        return;
    }

    *exact = FALSE;    /* strings don't exactly match */

    t1 = 0;   /* start comparison with 1st char of each string */
    s1 = 0;

    while(1)  /* compare corresponding chars in each string */
    {
      if (temp[t1] == '\0' || col[s1] == '\0')
      { 
         /* completely scanned one or both strings so they match */
         *match = TRUE;
         return;
      }

      if (temp[t1] == col[s1] || temp[t1] == '?')
      {
        s1++;  /* corresponding chars in the 2 strings match */
        t1++;  /* increment both pointers and loop back again */
      }
      else if (temp[t1] == '*')
      {    
        /* get next char from template and look for it in the col name */
        t1++;
        if (temp[t1] == '\0' || temp[t1] == ' ')
        {
          /* reached end of template so strings match */
          *match = TRUE;
          return;
        }

        found = 0;
        while (col[s1] && !found)
        {
          if (temp[t1] == col[s1])
          {
            t1++;  /* found matching characters; incre both pointers */
            s1++;  /* and loop back to compare next chars */
            found = 1;
          }
          else
            s1++;  /* increment the column name pointer and try again */
        }

        if (!found)
          return;  /* hit end of column name and failed to find a match */
      }
      else
        return;   /* strings don't match */
    }
}
/*--------------------------------------------------------------------------*/
int ffgtcl( fitsfile *fptr,  /* I - FITS file pointer                       */
            int  colnum,     /* I - column number                           */
            int *typecode,   /* O - datatype code (21 = short, etc)         */
            long *repeat,    /* O - repeat count of field                   */
            long *width,     /* O - if ASCII, width of field or unit string */
            int  *status)    /* IO - error status                           */
/*
  Get Type of table column. 
  Returns the datatype code of the column, as well as the vector
  repeat count and (if it is an ASCII character column) the
  width of the field or a unit string within the field.  This supports the
  TFORMn = 'rAw' syntax for specifying arrays of substrings, so
  if TFORMn = '60A12' then repeat = 60 and width = 12.
*/
{
    tcolumn *colptr;

    if (*status > 0)
        return(*status);

    if (fptr->datastart == DATA_UNDEFINED)
        if ( ffrdef(fptr, status) > 0)               /* rescan header */
            return(*status);

    if (colnum < 1 || colnum > fptr->tfield)
        return(*status = BAD_COL_NUM);

    colptr = fptr->tableptr;   /* pointer to first column */
    colptr += (colnum - 1);    /* offset to correct column */

    *typecode = colptr->tdatatype;
    *repeat = colptr->trepeat;
    *width = colptr->twidth;

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgacl( fitsfile *fptr,   /* I - FITS file pointer                      */
            int  colnum,      /* I - column number                          */
            char *ttype,      /* O - TTYPEn keyword value                   */
            long *tbcol,      /* O - TBCOLn keyword value                   */
            char *tunit,      /* O - TUNITn keyword value                   */
            char *tform,      /* O - TFORMn keyword value                   */
            double *tscal,    /* O - TSCALn keyword value                   */
            double *tzero,    /* O - TZEROn keyword value                   */
            char *tnull,      /* O - TNULLn keyword value                   */
            char *tdisp,      /* O - TDISPn keyword value                   */
            int  *status)     /* IO - error status                          */
/*
  get ASCII column keyword values
*/
{
    char name[FLEN_KEYWORD], comm[FLEN_COMMENT];
    tcolumn *colptr;
    int tstatus;

    if (*status > 0)
        return(*status);

    if (fptr->datastart == DATA_UNDEFINED)
        if ( ffrdef(fptr, status) > 0)               /* rescan header */
            return(*status);

    if (colnum < 1 || colnum > fptr->tfield)
        return(*status = BAD_COL_NUM);

    /* get what we can from the column structure */

    colptr = fptr->tableptr;   /* pointer to first column */
    colptr += (colnum -1);     /* offset to correct column */

    strcpy(ttype, colptr->ttype);
    *tbcol = colptr->tbcol;
    strcpy(tform, colptr->tform);
    *tscal = colptr->tscale;
    *tzero = colptr->tzero;
    strcpy(tnull, colptr->strnull);

    /* read keywords to get additional parameters */

    ffkeyn("TUNIT", colnum, name, status);
    tstatus = 0;
    *tunit = '\0';
    ffgkys(fptr, name, tunit, comm, &tstatus);

    ffkeyn("TDISP", colnum, name, status);
    tstatus = 0;
    *tdisp = '\0';
    ffgkys(fptr, name, tdisp, comm, &tstatus);

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgbcl( fitsfile *fptr,   /* I - FITS file pointer                      */
            int  colnum,      /* I - column number                          */
            char *ttype,      /* O - TTYPEn keyword value                   */
            char *tunit,      /* O - TUNITn keyword value                   */
            char *dtype,      /* O - datatype char: I, J, E, D, etc.        */
            long *repeat,     /* O - vector column repeat count             */
            double *tscal,    /* O - TSCALn keyword value                   */
            double *tzero,    /* O - TZEROn keyword value                   */
            long *tnull,      /* O - TNULLn keyword value integer cols only */
            char *tdisp,      /* O - TDISPn keyword value                   */
            int  *status)     /* IO - error status                          */
/*
  get BINTABLE column keyword values
*/
{
    char name[FLEN_KEYWORD], comm[FLEN_COMMENT];
    tcolumn *colptr;
    int tstatus;

    if (*status > 0)
        return(*status);

    if (fptr->datastart == DATA_UNDEFINED)
        if ( ffrdef(fptr, status) > 0)               /* rescan header */
            return(*status);

    if (colnum < 1 || colnum > fptr->tfield)
        return(*status = BAD_COL_NUM);

    /* get what we can from the column structure */

    colptr = fptr->tableptr;   /* pointer to first column */
    colptr += (colnum -1);     /* offset to correct column */

    strcpy(ttype, colptr->ttype);
    if (colptr->tdatatype < 0)  /* add the "P" prefix for */
        strcpy(dtype, "P");     /* variable length columns */
    else
        dtype[0] = 0;

    if      (abs(colptr->tdatatype) == TBIT)
        strcat(dtype, "X");
    else if (abs(colptr->tdatatype) == TBYTE)
        strcat(dtype, "B");
    else if (abs(colptr->tdatatype) == TLOGICAL)
        strcat(dtype, "L");
    else if (abs(colptr->tdatatype) == TSTRING)
        strcat(dtype, "A");
    else if (abs(colptr->tdatatype) == TSHORT)
        strcat(dtype, "I");
    else if (abs(colptr->tdatatype) == TLONG)
        strcat(dtype, "J");
    else if (abs(colptr->tdatatype) == TFLOAT)
        strcat(dtype, "E");
    else if (abs(colptr->tdatatype) == TDOUBLE)
        strcat(dtype, "D");
    else if (abs(colptr->tdatatype) == TCOMPLEX)
        strcat(dtype, "C");
    else if (abs(colptr->tdatatype) == TDBLCOMPLEX)
        strcat(dtype, "M");

    *repeat = colptr->trepeat;
    *tscal  = colptr->tscale;
    *tzero  = colptr->tzero;
    *tnull  = colptr->tnull;

    /* read keywords to get additional parameters */

    ffkeyn("TUNIT", colnum, name, status);
    tstatus = 0;
    *tunit = '\0';
    ffgkys(fptr, name, tunit, comm, &tstatus);

    ffkeyn("TDISP", colnum, name, status);
    tstatus = 0;
    *tdisp = '\0';
    ffgkys(fptr, name, tdisp, comm, &tstatus);

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgpv(  fitsfile *fptr,   /* I - FITS file pointer                       */
            int  datatype,    /* I - datatype of the value                   */
            long firstelem,   /* I - first vector element to read (1 = 1st)  */
            long nelem,       /* I - number of values to read                */
            void *nulval,     /* I - value for undefined pixels              */
            void *array,      /* O - array of values that are returned       */
            int  *anynul,     /* O - set to 1 if any values are null; else 0 */
            int  *status)     /* IO - error status                           */
/*
  Read an array of values from the primary array. The datatype of the
  input array is defined by the 2nd argument.  Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of
  the FITS array is not the same as the array being read).
  Undefined elements will be set equal to NULVAL, unless NULVAL=0
  in which case no checking for undefined values will be performed.
  ANYNUL is returned with a value of .true. if any pixels are undefined.
*/
{
    long row = 1;
    char *cdummy;

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
      if (nulval == 0)
        ffgclb(fptr, 2, row, firstelem, nelem, 1, 1, 0,
              (unsigned char *) array, cdummy, anynul, status);
      else
        ffgclb(fptr, 2, row, firstelem, nelem, 1, 1, *(unsigned char *) nulval,
              (unsigned char *) array, cdummy, anynul, status);
    }
    else if (datatype == TSHORT)
    {
      if (nulval == 0)
        ffgcli(fptr, 2, row, firstelem, nelem, 1, 1, 0,
              (short *) array, cdummy, anynul, status);
      else
        ffgcli(fptr, 2, row, firstelem, nelem, 1, 1, *(short *) nulval,
              (short *) array, cdummy, anynul, status);
    }
    else if (datatype == TINT)
    {
      if (nulval == 0)
        ffgclk(fptr, 2, row, firstelem, nelem, 1, 1, 0,
              (int *) array, cdummy, anynul, status);
      else
        ffgclk(fptr, 2, row, firstelem, nelem, 1, 1, *(int *) nulval,
              (int *) array, cdummy, anynul, status);
    }
    else if (datatype == TLONG)
    {
      if (nulval == 0)
        ffgclj(fptr, 2, row, firstelem, nelem, 1, 1, 0,
              (long *) array, cdummy, anynul, status);
      else
        ffgclj(fptr, 2, row, firstelem, nelem, 1, 1, *(long *) nulval,
              (long *) array, cdummy, anynul, status);
    }
    else if (datatype == TFLOAT)
    {
      if (nulval == 0)
        ffgcle(fptr, 2, row, firstelem, nelem, 1, 1, 0.,
              (float *) array, cdummy, anynul, status);
      else
        ffgcle(fptr, 2, row, firstelem, nelem, 1, 1, *(float *) nulval,
              (float *) array, cdummy, anynul, status);
    }
    else if (datatype == TDOUBLE)
    {
      if (nulval == 0)
        ffgcld(fptr, 2, row, firstelem, nelem, 1, 1, 0.,
              (double *) array, cdummy, anynul, status);
      else
        ffgcld(fptr, 2, row, firstelem, nelem, 1, 1, *(double *) nulval,
              (double *) array, cdummy, anynul, status);
    }
    else
      *status = BAD_DATATYPE;

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgpvb( fitsfile *fptr,   /* I - FITS file pointer                       */
            long  group,      /* I - group to read (1 = 1st group)           */
            long  firstelem,  /* I - first vector element to read (1 = 1st)  */
            long  nelem,      /* I - number of values to read                */
            unsigned char nulval, /* I - value for undefined pixels          */
            unsigned char *array, /* O - array of values that are returned   */
            int  *anynul,     /* O - set to 1 if any values are null; else 0 */
            int  *status)     /* IO - error status                           */
/*
  Read an array of values from the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of
  the FITS array is not the same as the array being read).
  Undefined elements will be set equal to NULVAL, unless NULVAL=0
  in which case no checking for undefined values will be performed.
  ANYNUL is returned with a value of .true. if any pixels are undefined.
*/
{
    long row;
    char *cdummy;
    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */

    row=maxvalue(1,group);

    ffgclb(fptr, 2, row, firstelem, nelem, 1, 1, nulval,
               array, cdummy, anynul, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgpvi( fitsfile *fptr,   /* I - FITS file pointer                       */
            long  group,      /* I - group to read (1 = 1st group)           */
            long  firstelem,  /* I - first vector element to read (1 = 1st)  */
            long  nelem,      /* I - number of values to read                */
            short nulval,     /* I - value for undefined pixels              */
            short *array,     /* O - array of values that are returned       */
            int  *anynul,     /* O - set to 1 if any values are null; else 0 */
            int  *status)     /* IO - error status                           */
/*
  Read an array of values from the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of
  the FITS array is not the same as the array being read).
  Undefined elements will be set equal to NULVAL, unless NULVAL=0
  in which case no checking for undefined values will be performed.
  ANYNUL is returned with a value of .true. if any pixels are undefined.
*/
{
    long row;
    char *cdummy;
    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */

    row=maxvalue(1,group);

    ffgcli(fptr, 2, row, firstelem, nelem, 1, 1, nulval,
               array, cdummy, anynul, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgpvj( fitsfile *fptr,   /* I - FITS file pointer                       */
            long  group,      /* I - group to read (1 = 1st group)           */
            long  firstelem,  /* I - first vector element to read (1 = 1st)  */
            long  nelem,      /* I - number of values to read                */
            long  nulval,     /* I - value for undefined pixels              */
            long  *array,     /* O - array of values that are returned       */
            int  *anynul,     /* O - set to 1 if any values are null; else 0 */
            int  *status)     /* IO - error status                           */
/*
  Read an array of values from the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of
  the FITS array is not the same as the array being read).
  Undefined elements will be set equal to NULVAL, unless NULVAL=0
  in which case no checking for undefined values will be performed.
  ANYNUL is returned with a value of .true. if any pixels are undefined.
*/
{
    long row;
    char *cdummy;
    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */

    row=maxvalue(1,group);

    ffgclj(fptr, 2, row, firstelem, nelem, 1, 1, nulval,
               array, cdummy, anynul, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgpvk( fitsfile *fptr,   /* I - FITS file pointer                       */
            long  group,      /* I - group to read (1 = 1st group)           */
            long  firstelem,  /* I - first vector element to read (1 = 1st)  */
            long  nelem,      /* I - number of values to read                */
            int   nulval,     /* I - value for undefined pixels              */
            int   *array,     /* O - array of values that are returned       */
            int  *anynul,     /* O - set to 1 if any values are null; else 0 */
            int  *status)     /* IO - error status                           */
/*
  Read an array of values from the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of
  the FITS array is not the same as the array being read).
  Undefined elements will be set equal to NULVAL, unless NULVAL=0
  in which case no checking for undefined values will be performed.
  ANYNUL is returned with a value of .true. if any pixels are undefined.
*/
{
    long row;
    char *cdummy;
    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */

    row=maxvalue(1,group);

    ffgclk(fptr, 2, row, firstelem, nelem, 1, 1, nulval,
               array, cdummy, anynul, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgpve( fitsfile *fptr,   /* I - FITS file pointer                       */
            long  group,      /* I - group to read (1 = 1st group)           */
            long  firstelem,  /* I - first vector element to read (1 = 1st)  */
            long  nelem,      /* I - number of values to read                */
            float nulval,     /* I - value for undefined pixels              */
            float *array,     /* O - array of values that are returned       */
            int  *anynul,     /* O - set to 1 if any values are null; else 0 */
            int  *status)     /* IO - error status                           */
/*
  Read an array of values from the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of
  the FITS array is not the same as the array being read).
  Undefined elements will be set equal to NULVAL, unless NULVAL=0
  in which case no checking for undefined values will be performed.
  ANYNUL is returned with a value of .true. if any pixels are undefined.
*/
{
    long row;
    char *cdummy;
    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */

    row=maxvalue(1,group);

    ffgcle(fptr, 2, row, firstelem, nelem, 1, 1, nulval,
               array, cdummy, anynul, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgpvd( fitsfile *fptr,   /* I - FITS file pointer                       */
            long  group,      /* I - group to read (1 = 1st group)           */
            long  firstelem,  /* I - first vector element to read (1 = 1st)  */
            long  nelem,      /* I - number of values to read                */
            double nulval,    /* I - value for undefined pixels              */
            double *array,    /* O - array of values that are returned       */
            int  *anynul,     /* O - set to 1 if any values are null; else 0 */
            int  *status)     /* IO - error status                           */
/*
  Read an array of values from the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of
  the FITS array is not the same as the array being read).
  Undefined elements will be set equal to NULVAL, unless NULVAL=0
  in which case no checking for undefined values will be performed.
  ANYNUL is returned with a value of .true. if any pixels are undefined.
*/
{
    long row;
    char *cdummy;
    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */

    row=maxvalue(1,group);

    ffgcld(fptr, 2, row, firstelem, nelem, 1, 1, nulval,
               array, cdummy, anynul, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgpfb( fitsfile *fptr,   /* I - FITS file pointer                       */
            long  group,      /* I - group to read (1 = 1st group)           */
            long  firstelem,  /* I - first vector element to read (1 = 1st)  */
            long  nelem,      /* I - number of values to read                */
            unsigned char *array, /* O - array of values that are returned   */
            char *nularray,   /* O - array of null pixel flags               */
            int  *anynul,     /* O - set to 1 if any values are null; else 0 */
            int  *status)     /* IO - error status                           */
/*
  Read an array of values from the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of
  the FITS array is not the same as the array being read).
  Any undefined pixels in the returned array will be set = 0 and the 
  corresponding nularray value will be set = 1.
  ANYNUL is returned with a value of .true. if any pixels are undefined.
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

    ffgclb(fptr, 2, row, firstelem, nelem, 1, 2, 0,
               array, nularray, anynul, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgpfi( fitsfile *fptr,   /* I - FITS file pointer                       */
            long  group,      /* I - group to read (1 = 1st group)           */
            long  firstelem,  /* I - first vector element to read (1 = 1st)  */
            long  nelem,      /* I - number of values to read                */
            short *array,     /* O - array of values that are returned       */
            char *nularray,   /* O - array of null pixel flags               */
            int  *anynul,     /* O - set to 1 if any values are null; else 0 */
            int  *status)     /* IO - error status                           */
/*
  Read an array of values from the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of
  the FITS array is not the same as the array being read).
  Any undefined pixels in the returned array will be set = 0 and the 
  corresponding nularray value will be set = 1.
  ANYNUL is returned with a value of .true. if any pixels are undefined.
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

    ffgcli(fptr, 2, row, firstelem, nelem, 1, 2, 0,
               array, nularray, anynul, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgpfj( fitsfile *fptr,   /* I - FITS file pointer                       */
            long  group,      /* I - group to read (1 = 1st group)           */
            long  firstelem,  /* I - first vector element to read (1 = 1st)  */
            long  nelem,      /* I - number of values to read                */
            long  *array,     /* O - array of values that are returned       */
            char *nularray,   /* O - array of null pixel flags               */
            int  *anynul,     /* O - set to 1 if any values are null; else 0 */
            int  *status)     /* IO - error status                           */
/*
  Read an array of values from the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of
  the FITS array is not the same as the array being read).
  Any undefined pixels in the returned array will be set = 0 and the 
  corresponding nularray value will be set = 1.
  ANYNUL is returned with a value of .true. if any pixels are undefined.
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

    ffgclj(fptr, 2, row, firstelem, nelem, 1, 2, 0L,
               array, nularray, anynul, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgpfk( fitsfile *fptr,   /* I - FITS file pointer                       */
            long  group,      /* I - group to read (1 = 1st group)           */
            long  firstelem,  /* I - first vector element to read (1 = 1st)  */
            long  nelem,      /* I - number of values to read                */
            int   *array,     /* O - array of values that are returned       */
            char *nularray,   /* O - array of null pixel flags               */
            int  *anynul,     /* O - set to 1 if any values are null; else 0 */
            int  *status)     /* IO - error status                           */
/*
  Read an array of values from the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of
  the FITS array is not the same as the array being read).
  Any undefined pixels in the returned array will be set = 0 and the 
  corresponding nularray value will be set = 1.
  ANYNUL is returned with a value of .true. if any pixels are undefined.
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

    ffgclk(fptr, 2, row, firstelem, nelem, 1, 2, 0L,
               array, nularray, anynul, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgpfe( fitsfile *fptr,   /* I - FITS file pointer                       */
            long  group,      /* I - group to read (1 = 1st group)           */
            long  firstelem,  /* I - first vector element to read (1 = 1st)  */
            long  nelem,      /* I - number of values to read                */
            float *array,     /* O - array of values that are returned       */
            char *nularray,   /* O - array of null pixel flags               */
            int  *anynul,     /* O - set to 1 if any values are null; else 0 */
            int  *status)     /* IO - error status                           */
/*
  Read an array of values from the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of
  the FITS array is not the same as the array being read).
  Any undefined pixels in the returned array will be set = 0 and the 
  corresponding nularray value will be set = 1.
  ANYNUL is returned with a value of .true. if any pixels are undefined.
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

    ffgcle(fptr, 2, row, firstelem, nelem, 1, 2, 0.F,
               array, nularray, anynul, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgpfd( fitsfile *fptr,   /* I - FITS file pointer                       */
            long  group,      /* I - group to read (1 = 1st group)           */
            long  firstelem,  /* I - first vector element to read (1 = 1st)  */
            long  nelem,      /* I - number of values to read                */
            double *array,    /* O - array of values that are returned       */
            char *nularray,   /* O - array of null pixel flags               */
            int  *anynul,     /* O - set to 1 if any values are null; else 0 */
            int  *status)     /* IO - error status                           */
/*
  Read an array of values from the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of
  the FITS array is not the same as the array being read).
  Any undefined pixels in the returned array will be set = 0 and the 
  corresponding nularray value will be set = 1.
  ANYNUL is returned with a value of .true. if any pixels are undefined.
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

    ffgcld(fptr, 2, row, firstelem, nelem, 1, 2, 0.,
               array, nularray, anynul, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffg2db(fitsfile *fptr,  /* I - FITS file pointer                       */
           long  group,     /* I - group to read (1 = 1st group)           */
           unsigned char nulval, /* set undefined pixels equal to this     */
           long  ncols,     /* I - number of pixels in each row of array   */
           long  naxis1,    /* I - FITS image NAXIS1 value                 */
           long  naxis2,    /* I - FITS image NAXIS2 value                 */
           unsigned char *array, /* O - array to be filled and returned    */
           int  *anynul,    /* O - set to 1 if any values are null; else 0 */
           int  *status)    /* IO - error status                           */
/*
  Read an entire 2-D array of values to the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of the
  FITS array is not the same as the array being read).  Any null
  values in the array will be set equal to the value of nulval, unless
  nulval = 0 in which case no null checking will be performed.
*/
{
    long tablerow, nfits, narray, ii;
    char *cdummy;

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */
    tablerow=maxvalue(1,group);

    if (ncols == naxis1)  /* arrays have same row length? */
    {
       /* all the image pixels are contiguous, so read all at once */
       ffgclb(fptr, 2, tablerow, 1, naxis1 * naxis2, 1, 1, nulval,
               array, cdummy, anynul, status);
       return(*status);
    }

    /* loop over the naxis2 rows in the FITS image, */
    /* reading naxis1 pixels to each row            */

    nfits = 1;   /* next pixel in FITS image to read */
    narray = 0;  /* next pixel in output array to be filled */

    for (ii = 0; ii < naxis2; ii++)
    {
       if (ffgclb(fptr, 2, tablerow, nfits, naxis1, 1, 1, nulval,
          &array[narray], cdummy, anynul, status) > 0)
          return(*status);

       nfits += naxis1;
       narray += ncols;
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffg2di(fitsfile *fptr,  /* I - FITS file pointer                       */
           long  group,     /* I - group to read (1 = 1st group)           */
           short nulval,    /* set undefined pixels equal to this          */
           long  ncols,     /* I - number of pixels in each row of array   */
           long  naxis1,    /* I - FITS image NAXIS1 value                 */
           long  naxis2,    /* I - FITS image NAXIS2 value                 */
           short *array,    /* O - array to be filled and returned         */
           int  *anynul,    /* O - set to 1 if any values are null; else 0 */
           int  *status)    /* IO - error status                           */
/*
  Read an entire 2-D array of values to the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of the
  FITS array is not the same as the array being read).  Any null
  values in the array will be set equal to the value of nulval, unless
  nulval = 0 in which case no null checking will be performed.
*/
{
    long tablerow, nfits, narray, ii;
    char *cdummy;

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */
    tablerow=maxvalue(1,group);

    if (ncols == naxis1)  /* arrays have same row length? */
    {
       /* all the image pixels are contiguous, so read all at once */
       ffgcli(fptr, 2, tablerow, 1, naxis1 * naxis2, 1, 1, nulval,
               array, cdummy, anynul, status);
       return(*status);
    }

    /* loop over the naxis2 rows in the FITS image, */
    /* reading naxis1 pixels to each row            */

    nfits = 1;   /* next pixel in FITS image to read */
    narray = 0;  /* next pixel in output array to be filled */

    for (ii = 0; ii < naxis2; ii++)
    {
       if (ffgcli(fptr, 2, tablerow, nfits, naxis1, 1, 1, nulval,
          &array[narray], cdummy, anynul, status) > 0)
          return(*status);

       nfits += naxis1;
       narray += ncols;
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffg2dj(fitsfile *fptr,  /* I - FITS file pointer                       */
           long  group,     /* I - group to read (1 = 1st group)           */
           long  nulval,    /* set undefined pixels equal to this          */
           long  ncols,     /* I - number of pixels in each row of array   */
           long  naxis1,    /* I - FITS image NAXIS1 value                 */
           long  naxis2,    /* I - FITS image NAXIS2 value                 */
           long  *array,    /* O - array to be filled and returned         */
           int  *anynul,    /* O - set to 1 if any values are null; else 0 */
           int  *status)    /* IO - error status                           */
/*
  Read an entire 2-D array of values to the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of the
  FITS array is not the same as the array being read).  Any null
  values in the array will be set equal to the value of nulval, unless
  nulval = 0 in which case no null checking will be performed.
*/
{
    long tablerow, nfits, narray, ii;
    char *cdummy;

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */
    tablerow=maxvalue(1,group);

    if (ncols == naxis1)  /* arrays have same row length? */
    {
       /* all the image pixels are contiguous, so read all at once */
       ffgclj(fptr, 2, tablerow, 1, naxis1 * naxis2, 1, 1, nulval,
               array, cdummy, anynul, status);
       return(*status);
    }

    /* loop over the naxis2 rows in the FITS image, */
    /* reading naxis1 pixels to each row            */

    nfits = 1;   /* next pixel in FITS image to read */
    narray = 0;  /* next pixel in output array to be filled */

    for (ii = 0; ii < naxis2; ii++)
    {
       if (ffgclj(fptr, 2, tablerow, nfits, naxis1, 1, 1, nulval,
          &array[narray], cdummy, anynul, status) > 0)
          return(*status);

       nfits += naxis1;
       narray += ncols;
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffg2dk(fitsfile *fptr,  /* I - FITS file pointer                       */
           long  group,     /* I - group to read (1 = 1st group)           */
           int  nulval,    /* set undefined pixels equal to this          */
           long  ncols,     /* I - number of pixels in each row of array   */
           long  naxis1,    /* I - FITS image NAXIS1 value                 */
           long  naxis2,    /* I - FITS image NAXIS2 value                 */
           int  *array,    /* O - array to be filled and returned         */
           int  *anynul,    /* O - set to 1 if any values are null; else 0 */
           int  *status)    /* IO - error status                           */
/*
  Read an entire 2-D array of values to the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of the
  FITS array is not the same as the array being read).  Any null
  values in the array will be set equal to the value of nulval, unless
  nulval = 0 in which case no null checking will be performed.
*/
{
    long tablerow, nfits, narray, ii;
    char *cdummy;

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */
    tablerow=maxvalue(1,group);

    if (ncols == naxis1)  /* arrays have same row length? */
    {
       /* all the image pixels are contiguous, so read all at once */
       ffgclk(fptr, 2, tablerow, 1, naxis1 * naxis2, 1, 1, nulval,
               array, cdummy, anynul, status);
       return(*status);
    }

    /* loop over the naxis2 rows in the FITS image, */
    /* reading naxis1 pixels to each row            */

    nfits = 1;   /* next pixel in FITS image to read */
    narray = 0;  /* next pixel in output array to be filled */

    for (ii = 0; ii < naxis2; ii++)
    {
       if (ffgclk(fptr, 2, tablerow, nfits, naxis1, 1, 1, nulval,
          &array[narray], cdummy, anynul, status) > 0)
          return(*status);

       nfits += naxis1;
       narray += ncols;
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffg2de(fitsfile *fptr,  /* I - FITS file pointer                       */
           long  group,     /* I - group to read (1 = 1st group)           */
           float nulval,    /* set undefined pixels equal to this          */
           long  ncols,     /* I - number of pixels in each row of array   */
           long  naxis1,    /* I - FITS image NAXIS1 value                 */
           long  naxis2,    /* I - FITS image NAXIS2 value                 */
           float *array,    /* O - array to be filled and returned         */
           int  *anynul,    /* O - set to 1 if any values are null; else 0 */
           int  *status)    /* IO - error status                           */
/*
  Read an entire 2-D array of values to the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of the
  FITS array is not the same as the array being read).  Any null
  values in the array will be set equal to the value of nulval, unless
  nulval = 0 in which case no null checking will be performed.
*/
{
    long tablerow, nfits, narray, ii;
    char *cdummy;

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */
    tablerow=maxvalue(1,group);

    if (ncols == naxis1)  /* arrays have same row length? */
    {
       /* all the image pixels are contiguous, so read all at once */
       ffgcle(fptr, 2, tablerow, 1, naxis1 * naxis2, 1, 1, nulval,
               array, cdummy, anynul, status);
       return(*status);
    }

    /* loop over the naxis2 rows in the FITS image, */
    /* reading naxis1 pixels to each row            */

    nfits = 1;   /* next pixel in FITS image to read */
    narray = 0;  /* next pixel in output array to be filled */

    for (ii = 0; ii < naxis2; ii++)
    {
       if (ffgcle(fptr, 2, tablerow, nfits, naxis1, 1, 1, nulval,
          &array[narray], cdummy, anynul, status) > 0)
          return(*status);

       nfits += naxis1;
       narray += ncols;
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffg2dd(fitsfile *fptr,  /* I - FITS file pointer                       */
           long  group,     /* I - group to read (1 = 1st group)           */
           double nulval,   /* set undefined pixels equal to this          */
           long  ncols,     /* I - number of pixels in each row of array   */
           long  naxis1,    /* I - FITS image NAXIS1 value                 */
           long  naxis2,    /* I - FITS image NAXIS2 value                 */
           double *array,   /* O - array to be filled and returned         */
           int  *anynul,    /* O - set to 1 if any values are null; else 0 */
           int  *status)    /* IO - error status                           */
/*
  Read an entire 2-D array of values to the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of the
  FITS array is not the same as the array being read).  Any null
  values in the array will be set equal to the value of nulval, unless
  nulval = 0 in which case no null checking will be performed.
*/
{
    long tablerow, nfits, narray, ii;
    char *cdummy;

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */
    tablerow=maxvalue(1,group);

    if (ncols == naxis1)  /* arrays have same row length? */
    {
       /* all the image pixels are contiguous, so read all at once */
       ffgcld(fptr, 2, tablerow, 1, naxis1 * naxis2, 1, 1, nulval,
               array, cdummy, anynul, status);
       return(*status);
    }

    /* loop over the naxis2 rows in the FITS image, */
    /* reading naxis1 pixels to each row            */

    nfits = 1;   /* next pixel in FITS image to read */
    narray = 0;  /* next pixel in output array to be filled */

    for (ii = 0; ii < naxis2; ii++)
    {
       if (ffgcld(fptr, 2, tablerow, nfits, naxis1, 1, 1, nulval,
          &array[narray], cdummy, anynul, status) > 0)
          return(*status);

       nfits += naxis1;
       narray += ncols;
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffg3db(fitsfile *fptr,  /* I - FITS file pointer                       */
           long  group,     /* I - group to read (1 = 1st group)           */
           unsigned char nulval, /* set undefined pixels equal to this     */
           long  ncols,     /* I - number of pixels in each row of array   */
           long  nrows,     /* I - number of rows in each plane of array   */
           long  naxis1,    /* I - FITS image NAXIS1 value                 */
           long  naxis2,    /* I - FITS image NAXIS2 value                 */
           long  naxis3,    /* I - FITS image NAXIS3 value                 */
           unsigned char *array, /* O - array to be filled and returned    */
           int  *anynul,    /* O - set to 1 if any values are null; else 0 */
           int  *status)    /* IO - error status                           */
/*
  Read an entire 3-D array of values to the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of the
  FITS array is not the same as the array being read).  Any null
  values in the array will be set equal to the value of nulval, unless
  nulval = 0 in which case no null checking will be performed.
*/
{
    long tablerow, nfits, narray, ii, jj;
    char *cdummy;

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */
    tablerow=maxvalue(1,group);

    if (ncols == naxis1 && nrows == naxis2)  /* arrays have same size? */
    {
       /* all the image pixels are contiguous, so read all at once */
       ffgclb(fptr, 2, tablerow, 1, naxis1 * naxis2 * naxis3, 1, 1, nulval,
               array, cdummy, anynul, status);
       return(*status);
    }

    nfits = 1;   /* next pixel in FITS image to read */
    narray = 0;  /* next pixel in output array to be filled */

    /* loop over naxis3 planes in the data cube */
    for (jj = 0; jj < naxis3; jj++)
    {
      /* loop over the naxis2 rows in the FITS image, */
      /* reading naxis1 pixels to each row            */

      for (ii = 0; ii < naxis2; ii++)
      {
       if (ffgclb(fptr, 2, tablerow, nfits, naxis1, 1, 1, nulval,
          &array[narray], cdummy, anynul, status) > 0)
          return(*status);

       nfits += naxis1;
       narray += ncols;
      }
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffg3di(fitsfile *fptr,  /* I - FITS file pointer                       */
           long  group,     /* I - group to read (1 = 1st group)           */
           short nulval,    /* set undefined pixels equal to this          */
           long  ncols,     /* I - number of pixels in each row of array   */
           long  nrows,     /* I - number of rows in each plane of array   */
           long  naxis1,    /* I - FITS image NAXIS1 value                 */
           long  naxis2,    /* I - FITS image NAXIS2 value                 */
           long  naxis3,    /* I - FITS image NAXIS3 value                 */
           short *array,    /* O - array to be filled and returned         */
           int  *anynul,    /* O - set to 1 if any values are null; else 0 */
           int  *status)    /* IO - error status                           */
/*
  Read an entire 3-D array of values to the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of the
  FITS array is not the same as the array being read).  Any null
  values in the array will be set equal to the value of nulval, unless
  nulval = 0 in which case no null checking will be performed.
*/
{
    long tablerow, nfits, narray, ii, jj;
    char *cdummy;

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */
    tablerow=maxvalue(1,group);

    if (ncols == naxis1 && nrows == naxis2)  /* arrays have same size? */
    {
       /* all the image pixels are contiguous, so read all at once */
       ffgcli(fptr, 2, tablerow, 1, naxis1 * naxis2 * naxis3, 1, 1, nulval,
               array, cdummy, anynul, status);
       return(*status);
    }

    nfits = 1;   /* next pixel in FITS image to read */
    narray = 0;  /* next pixel in output array to be filled */

    /* loop over naxis3 planes in the data cube */
    for (jj = 0; jj < naxis3; jj++)
    {
      /* loop over the naxis2 rows in the FITS image, */
      /* reading naxis1 pixels to each row            */

      for (ii = 0; ii < naxis2; ii++)
      {
       if (ffgcli(fptr, 2, tablerow, nfits, naxis1, 1, 1, nulval,
          &array[narray], cdummy, anynul, status) > 0)
          return(*status);

       nfits += naxis1;
       narray += ncols;
      }
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffg3dj(fitsfile *fptr,  /* I - FITS file pointer                       */
           long  group,     /* I - group to read (1 = 1st group)           */
           long  nulval,    /* set undefined pixels equal to this          */
           long  ncols,     /* I - number of pixels in each row of array   */
           long  nrows,     /* I - number of rows in each plane of array   */
           long  naxis1,    /* I - FITS image NAXIS1 value                 */
           long  naxis2,    /* I - FITS image NAXIS2 value                 */
           long  naxis3,    /* I - FITS image NAXIS3 value                 */
           long  *array,    /* O - array to be filled and returned         */
           int  *anynul,    /* O - set to 1 if any values are null; else 0 */
           int  *status)    /* IO - error status                           */
/*
  Read an entire 3-D array of values to the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of the
  FITS array is not the same as the array being read).  Any null
  values in the array will be set equal to the value of nulval, unless
  nulval = 0 in which case no null checking will be performed.
*/
{
    long tablerow, nfits, narray, ii, jj;
    char *cdummy;

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */
    tablerow=maxvalue(1,group);

    if (ncols == naxis1 && nrows == naxis2)  /* arrays have same size? */
    {
       /* all the image pixels are contiguous, so read all at once */
       ffgclj(fptr, 2, tablerow, 1, naxis1 * naxis2 * naxis3, 1, 1, nulval,
               array, cdummy, anynul, status);
       return(*status);
    }

    nfits = 1;   /* next pixel in FITS image to read */
    narray = 0;  /* next pixel in output array to be filled */

    /* loop over naxis3 planes in the data cube */
    for (jj = 0; jj < naxis3; jj++)
    {
      /* loop over the naxis2 rows in the FITS image, */
      /* reading naxis1 pixels to each row            */

      for (ii = 0; ii < naxis2; ii++)
      {
       if (ffgclj(fptr, 2, tablerow, nfits, naxis1, 1, 1, nulval,
          &array[narray], cdummy, anynul, status) > 0)
          return(*status);

       nfits += naxis1;
       narray += ncols;
      }
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffg3dk(fitsfile *fptr,  /* I - FITS file pointer                       */
           long  group,     /* I - group to read (1 = 1st group)           */
           int   nulval,    /* set undefined pixels equal to this          */
           long  ncols,     /* I - number of pixels in each row of array   */
           long  nrows,     /* I - number of rows in each plane of array   */
           long  naxis1,    /* I - FITS image NAXIS1 value                 */
           long  naxis2,    /* I - FITS image NAXIS2 value                 */
           long  naxis3,    /* I - FITS image NAXIS3 value                 */
           int   *array,    /* O - array to be filled and returned         */
           int  *anynul,    /* O - set to 1 if any values are null; else 0 */
           int  *status)    /* IO - error status                           */
/*
  Read an entire 3-D array of values to the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of the
  FITS array is not the same as the array being read).  Any null
  values in the array will be set equal to the value of nulval, unless
  nulval = 0 in which case no null checking will be performed.
*/
{
    long tablerow, nfits, narray, ii, jj;
    char *cdummy;

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */
    tablerow=maxvalue(1,group);

    if (ncols == naxis1 && nrows == naxis2)  /* arrays have same size? */
    {
       /* all the image pixels are contiguous, so read all at once */
       ffgclk(fptr, 2, tablerow, 1, naxis1 * naxis2 * naxis3, 1, 1, nulval,
               array, cdummy, anynul, status);
       return(*status);
    }

    nfits = 1;   /* next pixel in FITS image to read */
    narray = 0;  /* next pixel in output array to be filled */

    /* loop over naxis3 planes in the data cube */
    for (jj = 0; jj < naxis3; jj++)
    {
      /* loop over the naxis2 rows in the FITS image, */
      /* reading naxis1 pixels to each row            */

      for (ii = 0; ii < naxis2; ii++)
      {
       if (ffgclk(fptr, 2, tablerow, nfits, naxis1, 1, 1, nulval,
          &array[narray], cdummy, anynul, status) > 0)
          return(*status);

       nfits += naxis1;
       narray += ncols;
      }
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffg3de(fitsfile *fptr,  /* I - FITS file pointer                       */
           long  group,     /* I - group to read (1 = 1st group)           */
           float nulval,    /* set undefined pixels equal to this          */
           long  ncols,     /* I - number of pixels in each row of array   */
           long  nrows,     /* I - number of rows in each plane of array   */
           long  naxis1,    /* I - FITS image NAXIS1 value                 */
           long  naxis2,    /* I - FITS image NAXIS2 value                 */
           long  naxis3,    /* I - FITS image NAXIS3 value                 */
           float *array,    /* O - array to be filled and returned         */
           int  *anynul,    /* O - set to 1 if any values are null; else 0 */
           int  *status)    /* IO - error status                           */
/*
  Read an entire 3-D array of values to the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of the
  FITS array is not the same as the array being read).  Any null
  values in the array will be set equal to the value of nulval, unless
  nulval = 0 in which case no null checking will be performed.
*/
{
    long tablerow, nfits, narray, ii, jj;
    char *cdummy;

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */
    tablerow=maxvalue(1,group);

    if (ncols == naxis1 && nrows == naxis2)  /* arrays have same size? */
    {
       /* all the image pixels are contiguous, so read all at once */
       ffgcle(fptr, 2, tablerow, 1, naxis1 * naxis2 * naxis3, 1, 1, nulval,
               array, cdummy, anynul, status);
       return(*status);
    }

    nfits = 1;   /* next pixel in FITS image to read */
    narray = 0;  /* next pixel in output array to be filled */

    /* loop over naxis3 planes in the data cube */
    for (jj = 0; jj < naxis3; jj++)
    {
      /* loop over the naxis2 rows in the FITS image, */
      /* reading naxis1 pixels to each row            */

      for (ii = 0; ii < naxis2; ii++)
      {
       if (ffgcle(fptr, 2, tablerow, nfits, naxis1, 1, 1, nulval,
          &array[narray], cdummy, anynul, status) > 0)
          return(*status);

       nfits += naxis1;
       narray += ncols;
      }
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffg3dd(fitsfile *fptr,  /* I - FITS file pointer                       */
           long  group,     /* I - group to read (1 = 1st group)           */
           double nulval,   /* set undefined pixels equal to this          */
           long  ncols,     /* I - number of pixels in each row of array   */
           long  nrows,     /* I - number of rows in each plane of array   */
           long  naxis1,    /* I - FITS image NAXIS1 value                 */
           long  naxis2,    /* I - FITS image NAXIS2 value                 */
           long  naxis3,    /* I - FITS image NAXIS3 value                 */
           double *array,   /* O - array to be filled and returned         */
           int  *anynul,    /* O - set to 1 if any values are null; else 0 */
           int  *status)    /* IO - error status                           */
/*
  Read an entire 3-D array of values to the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of the
  FITS array is not the same as the array being read).  Any null
  values in the array will be set equal to the value of nulval, unless
  nulval = 0 in which case no null checking will be performed.
*/
{
    long tablerow, nfits, narray, ii, jj;
    char *cdummy;

    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */
    tablerow=maxvalue(1,group);

    if (ncols == naxis1 && nrows == naxis2)  /* arrays have same size? */
    {
       /* all the image pixels are contiguous, so read all at once */
       ffgcld(fptr, 2, tablerow, 1, naxis1 * naxis2 * naxis3, 1, 1, nulval,
               array, cdummy, anynul, status);
       return(*status);
    }

    nfits = 1;   /* next pixel in FITS image to read */
    narray = 0;  /* next pixel in output array to be filled */

    /* loop over naxis3 planes in the data cube */
    for (jj = 0; jj < naxis3; jj++)
    {
      /* loop over the naxis2 rows in the FITS image, */
      /* reading naxis1 pixels to each row            */

      for (ii = 0; ii < naxis2; ii++)
      {
       if (ffgcld(fptr, 2, tablerow, nfits, naxis1, 1, 1, nulval,
          &array[narray], cdummy, anynul, status) > 0)
          return(*status);

       nfits += naxis1;
       narray += ncols;
      }
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgsvb(fitsfile *fptr, /* I - FITS file pointer                         */
           int  colnum,    /* I - number of the column to read (1 = 1st)    */
           int naxis,      /* I - number of dimensions in the FITS array    */
           long  *naxes,   /* I - size of each dimension                    */
           long  *blc,     /* I - 'bottom left corner' of the subsection    */
           long  *trc,     /* I - 'top right corner' of the subsection      */
           long  *inc,     /* I - increment to be applied in each dimension */
           unsigned char nulval, /* I - value to set undefined pixels       */
           unsigned char *array, /* O - array to be filled and returned     */
           int  *anynul,   /* O - set to 1 if any values are null; else 0   */
           int  *status)   /* IO - error status                             */
/*
  Read a subsection of data values from an image or a table column.
  This routine is set up to handle a maximum of nine dimensions.
*/
{
    long ii, i0, i1, i2, i3, i4, i5, i6, i7, i8, row, rstr, rstp, rinc;
    long str[9], stp[9], incr[9], dsize[10];
    long felem, nelem, nultyp, ninc, numcol;
    int anyf;
    char ldummy, msg[FLEN_ERRMSG];

    if (naxis < 1 || naxis > 9)
    {
        sprintf(msg, "NAXIS = %d in call to ffgsvb is out of range", naxis);
        ffpmsg(msg);
        return(*status = BAD_DIMEN);
    }

/*
    if this is a primary array, then the input COLNUM parameter should
    be interpreted as the row number, and we will alway read the image
    data from column 2 (any group parameters are in column 1).
*/
    if (fptr->hdutype == 0)
    {
        /* this is a primary array, or image extension */
        if (colnum == 0)
        {
            rstr = 1;
            rstp = 1;
        }
        else
        {
            rstr = colnum - 1;
            rstp = colnum - 1;
        }
        rinc = 1;
        numcol = 2;
    }
    else
    {
        /* this is a table, so the row info is in the (naxis+1) elements */
        rstr = blc[naxis];
        rstp = trc[naxis];
        rinc = inc[naxis];
        numcol = colnum;
    }

    nultyp = 1;
    *anynul = FALSE;
    i0 = 0;
    for (ii = 0; ii < 9; ii++)
    {
        str[ii] = 1;
        stp[ii] = 1;
        incr[ii] = 1;
        dsize[ii] = 1;
    }

    for (ii = 0; ii < naxis; ii++)
    {
      if (trc[ii] < blc[ii])
      {
        sprintf(msg, "ffgsvb: illegal range specified for axis %d", ii + 1);
        ffpmsg(msg);
        return(*status = BAD_PIX_NUM);
      }

      str[ii] = blc[ii];
      stp[ii] = trc[ii];
      incr[ii] = inc[ii];
      dsize[ii + 1] = dsize[ii] * naxes[ii];
    }

    if (naxis == 1 && naxes[0] == 1)
    {
      /* This is not a vector column, so read all the rows at once */
      nelem = (rstp - rstr) / rinc + 1;
      ninc = rinc;
      rstp = rstr;
    }
    else
    {
      /* have to read each row individually, in all dimensions */
      nelem = (stp[0] - str[0]) / inc[0] + 1;
      ninc = incr[0];
    }

    for (row = rstr; row <= rstp; row += rinc)
    {
     for (i8 = str[8]; i8 <= stp[8]; i8 += incr[8])
     {
      for (i7 = str[7]; i7 <= stp[7]; i7 += incr[7])
      {
       for (i6 = str[6]; i6 <= stp[6]; i6 += incr[6])
       {
        for (i5 = str[5]; i5 <= stp[5]; i5 += incr[5])
        {
         for (i4 = str[4]; i4 <= stp[4]; i4 += incr[4])
         {
          for (i3 = str[3]; i3 <= stp[3]; i3 += incr[3])
          {
           for (i2 = str[2]; i2 <= stp[2]; i2 += incr[2])
           {
            for (i1 = str[1]; i1 <= stp[1]; i1 += incr[1])
            {
              felem=str[0] + (i1 - 1) * dsize[1] + (i2 - 1) * dsize[2] + 
                             (i3 - 1) * dsize[3] + (i4 - 1) * dsize[4] +
                             (i5 - 1) * dsize[5] + (i6 - 1) * dsize[6] +
                             (i7 - 1) * dsize[7] + (i8 - 1) * dsize[8];

              if ( ffgclb(fptr, numcol, row, felem, nelem, ninc, nultyp,
                   nulval, &array[i0], &ldummy, &anyf, status) > 0)
                   return(*status);

              if (anyf)
                  *anynul = TRUE;

              i0 += nelem;
            }
           }
          }
         }
        }
       }
      }
     }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgsvi(fitsfile *fptr, /* I - FITS file pointer                         */
           int  colnum,    /* I - number of the column to read (1 = 1st)    */
           int naxis,      /* I - number of dimensions in the FITS array    */
           long  *naxes,   /* I - size of each dimension                    */
           long  *blc,     /* I - 'bottom left corner' of the subsection    */
           long  *trc,     /* I - 'top right corner' of the subsection      */
           long  *inc,     /* I - increment to be applied in each dimension */
           short nulval,   /* I - value to set undefined pixels             */
           short *array,   /* O - array to be filled and returned           */
           int  *anynul,   /* O - set to 1 if any values are null; else 0   */
           int  *status)   /* IO - error status                             */
/*
  Read a subsection of data values from an image or a table column.
  This routine is set up to handle a maximum of nine dimensions.
*/
{
    long ii,i0, i1,i2,i3,i4,i5,i6,i7,i8,row,rstr,rstp,rinc;
    long str[9],stp[9],incr[9],dsize[10];
    long felem, nelem, nultyp, ninc, numcol;
    int anyf;
    char ldummy, msg[FLEN_ERRMSG];

    if (naxis < 1 || naxis > 9)
    {
        sprintf(msg, "NAXIS = %d in call to ffgsvi is out of range", naxis);
        ffpmsg(msg);
        return(*status = BAD_DIMEN);
    }

/*
    if this is a primary array, then the input COLNUM parameter should
    be interpreted as the row number, and we will alway read the image
    data from column 2 (any group parameters are in column 1).
*/
    if (fptr->hdutype == 0)
    {
        /* this is a primary array, or image extension */
        if (colnum == 0)
        {
            rstr = 1;
            rstp = 1;
        }
        else
        {
            rstr = colnum;
            rstp = colnum;
        }
        rinc = 1;
        numcol = 2;
    }
    else
    {
        /* this is a table, so the row info is in the (naxis+1) elements */
        rstr = blc[naxis];
        rstp = trc[naxis];
        rinc = inc[naxis];
        numcol = colnum;
    }

    nultyp = 1;
    *anynul = FALSE;
    i0 = 0;
    for (ii = 0; ii < 9; ii++)
    {
        str[ii] = 1;
        stp[ii] = 1;
        incr[ii] = 1;
        dsize[ii] = 1;
    }

    for (ii = 0; ii < naxis; ii++)
    {
      if (trc[ii] < blc[ii])
      {
        sprintf(msg, "ffgsvi: illegal range specified for axis %d", ii + 1);
        ffpmsg(msg);
        return(*status = BAD_PIX_NUM);
      }

      str[ii] = blc[ii];
      stp[ii] = trc[ii];
      incr[ii] = inc[ii];
      dsize[ii + 1] = dsize[ii] * naxes[ii];
    }

    if (naxis == 1 && naxes[0] == 1)
    {
      /* This is not a vector column, so read all the rows at once */
      nelem = (rstp - rstr) / rinc + 1;
      ninc = rinc;
      rstp = rstr;
    }
    else
    {
      /* have to read each row individually, in all dimensions */
      nelem = (stp[0] - str[0]) / inc[0] + 1;
      ninc = incr[0];
    }

    for (row = rstr; row <= rstp; row += rinc)
    {
     for (i8 = str[8]; i8 <= stp[8]; i8 += incr[8])
     {
      for (i7 = str[7]; i7 <= stp[7]; i7 += incr[7])
      {
       for (i6 = str[6]; i6 <= stp[6]; i6 += incr[6])
       {
        for (i5 = str[5]; i5 <= stp[5]; i5 += incr[5])
        {
         for (i4 = str[4]; i4 <= stp[4]; i4 += incr[4])
         {
          for (i3 = str[3]; i3 <= stp[3]; i3 += incr[3])
          {
           for (i2 = str[2]; i2 <= stp[2]; i2 += incr[2])
           {
            for (i1 = str[1]; i1 <= stp[1]; i1 += incr[1])
            {
              felem=str[0] + (i1 - 1) * dsize[1] + (i2 - 1) * dsize[2] + 
                             (i3 - 1) * dsize[3] + (i4 - 1) * dsize[4] +
                             (i5 - 1) * dsize[5] + (i6 - 1) * dsize[6] +
                             (i7 - 1) * dsize[7] + (i8 - 1) * dsize[8];
              if ( ffgcli(fptr, numcol, row, felem, nelem, ninc, nultyp,
                   nulval, &array[i0], &ldummy, &anyf, status) > 0)
                   return(*status);

              if (anyf)
                  *anynul = TRUE;

              i0 += nelem;
            }
           }
          }
         }
        }
       }
      }
     }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgsvj(fitsfile *fptr, /* I - FITS file pointer                         */
           int  colnum,    /* I - number of the column to read (1 = 1st)    */
           int naxis,      /* I - number of dimensions in the FITS array    */
           long  *naxes,   /* I - size of each dimension                    */
           long  *blc,     /* I - 'bottom left corner' of the subsection    */
           long  *trc,     /* I - 'top right corner' of the subsection      */
           long  *inc,     /* I - increment to be applied in each dimension */
           long nulval,    /* I - value to set undefined pixels             */
           long *array,    /* O - array to be filled and returned           */
           int  *anynul,   /* O - set to 1 if any values are null; else 0   */
           int  *status)   /* IO - error status                             */
/*
  Read a subsection of data values from an image or a table column.
  This routine is set up to handle a maximum of nine dimensions.
*/
{
    long ii,i0, i1,i2,i3,i4,i5,i6,i7,i8,row,rstr,rstp,rinc;
    long str[9],stp[9],incr[9],dsize[10];
    long felem, nelem, nultyp, ninc, numcol;
    int anyf;
    char ldummy, msg[FLEN_ERRMSG];

    if (naxis < 1 || naxis > 9)
    {
        sprintf(msg, "NAXIS = %d in call to ffgsvj is out of range", naxis);
        ffpmsg(msg);
        return(*status = BAD_DIMEN);
    }

/*
    if this is a primary array, then the input COLNUM parameter should
    be interpreted as the row number, and we will alway read the image
    data from column 2 (any group parameters are in column 1).
*/
    if (fptr->hdutype == 0)
    {
        /* this is a primary array, or image extension */
        if (colnum == 0)
        {
            rstr = 1;
            rstp = 1;
        }
        else
        {
            rstr = colnum - 1;
            rstp = colnum - 1;
        }
        rinc = 1;
        numcol = 2;
    }
    else
    {
        /* this is a table, so the row info is in the (naxis+1) elements */
        rstr = blc[naxis];
        rstp = trc[naxis];
        rinc = inc[naxis];
        numcol = colnum;
    }

    nultyp = 1;
    *anynul = FALSE;
    i0 = 0;
    for (ii = 0; ii < 9; ii++)
    {
        str[ii] = 1;
        stp[ii] = 1;
        incr[ii] = 1;
        dsize[ii] = 1;
    }

    for (ii = 0; ii < naxis; ii++)
    {
      if (trc[ii] < blc[ii])
      {
        sprintf(msg, "ffgsvj: illegal range specified for axis %d", ii + 1);
        ffpmsg(msg);
        return(*status = BAD_PIX_NUM);
      }

      str[ii] = blc[ii];
      stp[ii] = trc[ii];
      incr[ii] = inc[ii];
      dsize[ii + 1] = dsize[ii] * naxes[ii];
    }

    if (naxis == 1 && naxes[0] == 1)
    {
      /* This is not a vector column, so read all the rows at once */
      nelem = (rstp - rstr) / rinc + 1;
      ninc = rinc;
      rstp = rstr;
    }
    else
    {
      /* have to read each row individually, in all dimensions */
      nelem = (stp[0] - str[0]) / inc[0] + 1;
      ninc = incr[0];
    }

    for (row = rstr; row <= rstp; row += rinc)
    {
     for (i8 = str[8]; i8 <= stp[8]; i8 += incr[8])
     {
      for (i7 = str[7]; i7 <= stp[7]; i7 += incr[7])
      {
       for (i6 = str[6]; i6 <= stp[6]; i6 += incr[6])
       {
        for (i5 = str[5]; i5 <= stp[5]; i5 += incr[5])
        {
         for (i4 = str[4]; i4 <= stp[4]; i4 += incr[4])
         {
          for (i3 = str[3]; i3 <= stp[3]; i3 += incr[3])
          {
           for (i2 = str[2]; i2 <= stp[2]; i2 += incr[2])
           {
            for (i1 = str[1]; i1 <= stp[1]; i1 += incr[1])
            {
              felem=str[0] + (i1 - 1) * dsize[1] + (i2 - 1) * dsize[2] + 
                             (i3 - 1) * dsize[3] + (i4 - 1) * dsize[4] +
                             (i5 - 1) * dsize[5] + (i6 - 1) * dsize[6] +
                             (i7 - 1) * dsize[7] + (i8 - 1) * dsize[8];

              if ( ffgclj(fptr, numcol, row, felem, nelem, ninc, nultyp,
                   nulval, &array[i0], &ldummy, &anyf, status) > 0)
                   return(*status);

              if (anyf)
                  *anynul = TRUE;

              i0 += nelem;
            }
           }
          }
         }
        }
       }
      }
     }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgsvk(fitsfile *fptr, /* I - FITS file pointer                         */
           int  colnum,    /* I - number of the column to read (1 = 1st)    */
           int naxis,      /* I - number of dimensions in the FITS array    */
           long  *naxes,   /* I - size of each dimension                    */
           long  *blc,     /* I - 'bottom left corner' of the subsection    */
           long  *trc,     /* I - 'top right corner' of the subsection      */
           long  *inc,     /* I - increment to be applied in each dimension */
           int  nulval,    /* I - value to set undefined pixels             */
           int  *array,    /* O - array to be filled and returned           */
           int  *anynul,   /* O - set to 1 if any values are null; else 0   */
           int  *status)   /* IO - error status                             */
/*
  Read a subsection of data values from an image or a table column.
  This routine is set up to handle a maximum of nine dimensions.
*/
{
    long ii,i0, i1,i2,i3,i4,i5,i6,i7,i8,row,rstr,rstp,rinc;
    long str[9],stp[9],incr[9],dsize[10];
    long felem, nelem, nultyp, ninc, numcol;
    int anyf;
    char ldummy, msg[FLEN_ERRMSG];

    if (naxis < 1 || naxis > 9)
    {
        sprintf(msg, "NAXIS = %d in call to ffgsvj is out of range", naxis);
        ffpmsg(msg);
        return(*status = BAD_DIMEN);
    }

/*
    if this is a primary array, then the input COLNUM parameter should
    be interpreted as the row number, and we will alway read the image
    data from column 2 (any group parameters are in column 1).
*/
    if (fptr->hdutype == 0)
    {
        /* this is a primary array, or image extension */
        if (colnum == 0)
        {
            rstr = 1;
            rstp = 1;
        }
        else
        {
            rstr = colnum - 1;
            rstp = colnum - 1;
        }
        rinc = 1;
        numcol = 2;
    }
    else
    {
        /* this is a table, so the row info is in the (naxis+1) elements */
        rstr = blc[naxis];
        rstp = trc[naxis];
        rinc = inc[naxis];
        numcol = colnum;
    }

    nultyp = 1;
    *anynul = FALSE;
    i0 = 0;
    for (ii = 0; ii < 9; ii++)
    {
        str[ii] = 1;
        stp[ii] = 1;
        incr[ii] = 1;
        dsize[ii] = 1;
    }

    for (ii = 0; ii < naxis; ii++)
    {
      if (trc[ii] < blc[ii])
      {
        sprintf(msg, "ffgsvj: illegal range specified for axis %d", ii + 1);
        ffpmsg(msg);
        return(*status = BAD_PIX_NUM);
      }

      str[ii] = blc[ii];
      stp[ii] = trc[ii];
      incr[ii] = inc[ii];
      dsize[ii + 1] = dsize[ii] * naxes[ii];
    }

    if (naxis == 1 && naxes[0] == 1)
    {
      /* This is not a vector column, so read all the rows at once */
      nelem = (rstp - rstr) / rinc + 1;
      ninc = rinc;
      rstp = rstr;
    }
    else
    {
      /* have to read each row individually, in all dimensions */
      nelem = (stp[0] - str[0]) / inc[0] + 1;
      ninc = incr[0];
    }

    for (row = rstr; row <= rstp; row += rinc)
    {
     for (i8 = str[8]; i8 <= stp[8]; i8 += incr[8])
     {
      for (i7 = str[7]; i7 <= stp[7]; i7 += incr[7])
      {
       for (i6 = str[6]; i6 <= stp[6]; i6 += incr[6])
       {
        for (i5 = str[5]; i5 <= stp[5]; i5 += incr[5])
        {
         for (i4 = str[4]; i4 <= stp[4]; i4 += incr[4])
         {
          for (i3 = str[3]; i3 <= stp[3]; i3 += incr[3])
          {
           for (i2 = str[2]; i2 <= stp[2]; i2 += incr[2])
           {
            for (i1 = str[1]; i1 <= stp[1]; i1 += incr[1])
            {
              felem=str[0] + (i1 - 1) * dsize[1] + (i2 - 1) * dsize[2] + 
                             (i3 - 1) * dsize[3] + (i4 - 1) * dsize[4] +
                             (i5 - 1) * dsize[5] + (i6 - 1) * dsize[6] +
                             (i7 - 1) * dsize[7] + (i8 - 1) * dsize[8];

              if ( ffgclk(fptr, numcol, row, felem, nelem, ninc, nultyp,
                   nulval, &array[i0], &ldummy, &anyf, status) > 0)
                   return(*status);

              if (anyf)
                  *anynul = TRUE;

              i0 += nelem;
            }
           }
          }
         }
        }
       }
      }
     }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgsve(fitsfile *fptr, /* I - FITS file pointer                         */
           int  colnum,    /* I - number of the column to read (1 = 1st)    */
           int naxis,      /* I - number of dimensions in the FITS array    */
           long  *naxes,   /* I - size of each dimension                    */
           long  *blc,     /* I - 'bottom left corner' of the subsection    */
           long  *trc,     /* I - 'top right corner' of the subsection      */
           long  *inc,     /* I - increment to be applied in each dimension */
           float nulval,   /* I - value to set undefined pixels             */
           float *array,   /* O - array to be filled and returned           */
           int  *anynul,   /* O - set to 1 if any values are null; else 0   */
           int  *status)   /* IO - error status                             */
/*
  Read a subsection of data values from an image or a table column.
  This routine is set up to handle a maximum of nine dimensions.
*/
{
    long ii,i0, i1,i2,i3,i4,i5,i6,i7,i8,row,rstr,rstp,rinc;
    long str[9],stp[9],incr[9],dsize[10];
    long felem, nelem, nultyp, ninc, numcol;
    int anyf;
    char ldummy, msg[FLEN_ERRMSG];

    if (naxis < 1 || naxis > 9)
    {
        sprintf(msg, "NAXIS = %d in call to ffgsve is out of range", naxis);
        ffpmsg(msg);
        return(*status = BAD_DIMEN);
    }

/*
    if this is a primary array, then the input COLNUM parameter should
    be interpreted as the row number, and we will alway read the image
    data from column 2 (any group parameters are in column 1).
*/
    if (fptr->hdutype == 0)
    {
        /* this is a primary array, or image extension */
        if (colnum == 0)
        {
            rstr = 1;
            rstp = 1;
        }
        else
        {
            rstr = colnum - 1;
            rstp = colnum - 1;
        }
        rinc = 1;
        numcol = 2;
    }
    else
    {
        /* this is a table, so the row info is in the (naxis+1) elements */
        rstr = blc[naxis];
        rstp = trc[naxis];
        rinc = inc[naxis];
        numcol = colnum;
    }

    nultyp = 1;
    *anynul = FALSE;
    i0 = 0;
    for (ii = 0; ii < 9; ii++)
    {
        str[ii] = 1;
        stp[ii] = 1;
        incr[ii] = 1;
        dsize[ii] = 1;
    }

    for (ii = 0; ii < naxis; ii++)
    {
      if (trc[ii] < blc[ii])
      {
        sprintf(msg, "ffgsve: illegal range specified for axis %d", ii + 1);
        ffpmsg(msg);
        return(*status = BAD_PIX_NUM);
      }

      str[ii] = blc[ii];
      stp[ii] = trc[ii];
      incr[ii] = inc[ii];
      dsize[ii + 1] = dsize[ii] * naxes[ii];
    }

    if (naxis == 1 && naxes[0] == 1)
    {
      /* This is not a vector column, so read all the rows at once */
      nelem = (rstp - rstr) / rinc + 1;
      ninc = rinc;
      rstp = rstr;
    }
    else
    {
      /* have to read each row individually, in all dimensions */
      nelem = (stp[0] - str[0]) / inc[0] + 1;
      ninc = incr[0];
    }

    for (row = rstr; row <= rstp; row += rinc)
    {
     for (i8 = str[8]; i8 <= stp[8]; i8 += incr[8])
     {
      for (i7 = str[7]; i7 <= stp[7]; i7 += incr[7])
      {
       for (i6 = str[6]; i6 <= stp[6]; i6 += incr[6])
       {
        for (i5 = str[5]; i5 <= stp[5]; i5 += incr[5])
        {
         for (i4 = str[4]; i4 <= stp[4]; i4 += incr[4])
         {
          for (i3 = str[3]; i3 <= stp[3]; i3 += incr[3])
          {
           for (i2 = str[2]; i2 <= stp[2]; i2 += incr[2])
           {
            for (i1 = str[1]; i1 <= stp[1]; i1 += incr[1])
            {
              felem=str[0] + (i1 - 1) * dsize[1] + (i2 - 1) * dsize[2] + 
                             (i3 - 1) * dsize[3] + (i4 - 1) * dsize[4] +
                             (i5 - 1) * dsize[5] + (i6 - 1) * dsize[6] +
                             (i7 - 1) * dsize[7] + (i8 - 1) * dsize[8];

              if ( ffgcle(fptr, numcol, row, felem, nelem, ninc, nultyp,
                   nulval, &array[i0], &ldummy, &anyf, status) > 0)
                   return(*status);

              if (anyf)
                  *anynul = TRUE;

              i0 += nelem;
            }
           }
          }
         }
        }
       }
      }
     }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgsvd(fitsfile *fptr, /* I - FITS file pointer                         */
           int  colnum,    /* I - number of the column to read (1 = 1st)    */
           int naxis,      /* I - number of dimensions in the FITS array    */
           long  *naxes,   /* I - size of each dimension                    */
           long  *blc,     /* I - 'bottom left corner' of the subsection    */
           long  *trc,     /* I - 'top right corner' of the subsection      */
           long  *inc,     /* I - increment to be applied in each dimension */
           double nulval,  /* I - value to set undefined pixels             */
           double *array,  /* O - array to be filled and returned           */
           int  *anynul,   /* O - set to 1 if any values are null; else 0   */
           int  *status)   /* IO - error status                             */
/*
  Read a subsection of data values from an image or a table column.
  This routine is set up to handle a maximum of nine dimensions.
*/
{
    long ii,i0, i1,i2,i3,i4,i5,i6,i7,i8,row,rstr,rstp,rinc;
    long str[9],stp[9],incr[9],dsize[10];
    long felem, nelem, nultyp, ninc, numcol;
    int anyf;
    char ldummy, msg[FLEN_ERRMSG];

    if (naxis < 1 || naxis > 9)
    {
        sprintf(msg, "NAXIS = %d in call to ffgsvd is out of range", naxis);
        ffpmsg(msg);
        return(*status = BAD_DIMEN);
    }

/*
    if this is a primary array, then the input COLNUM parameter should
    be interpreted as the row number, and we will alway read the image
    data from column 2 (any group parameters are in column 1).
*/
    if (fptr->hdutype == 0)
    {
        /* this is a primary array, or image extension */
        if (colnum == 0)
        {
            rstr = 1;
            rstp = 1;
        }
        else
        {
            rstr = colnum - 1;
            rstp = colnum - 1;
        }
        rinc = 1;
        numcol = 2;
    }
    else
    {
        /* this is a table, so the row info is in the (naxis+1) elements */
        rstr = blc[naxis];
        rstp = trc[naxis];
        rinc = inc[naxis];
        numcol = colnum;
    }

    nultyp = 1;
    *anynul = FALSE;
    i0 = 0;
    for (ii = 0; ii < 9; ii++)
    {
        str[ii] = 1;
        stp[ii] = 1;
        incr[ii] = 1;
        dsize[ii] = 1;
    }

    for (ii = 0; ii < naxis; ii++)
    {
      if (trc[ii] < blc[ii])
      {
        sprintf(msg, "ffgsvd: illegal range specified for axis %d", ii + 1);
        ffpmsg(msg);
        return(*status = BAD_PIX_NUM);
      }

      str[ii] = blc[ii];
      stp[ii] = trc[ii];
      incr[ii] = inc[ii];
      dsize[ii + 1] = dsize[ii] * naxes[ii];
    }

    if (naxis == 1 && naxes[0] == 1)
    {
      /* This is not a vector column, so read all the rows at once */
      nelem = (rstp - rstr) / rinc + 1;
      ninc = rinc;
      rstp = rstr;
    }
    else
    {
      /* have to read each row individually, in all dimensions */
      nelem = (stp[0] - str[0]) / inc[0] + 1;
      ninc = incr[0];
    }

    for (row = rstr; row <= rstp; row += rinc)
    {
     for (i8 = str[8]; i8 <= stp[8]; i8 += incr[8])
     {
      for (i7 = str[7]; i7 <= stp[7]; i7 += incr[7])
      {
       for (i6 = str[6]; i6 <= stp[6]; i6 += incr[6])
       {
        for (i5 = str[5]; i5 <= stp[5]; i5 += incr[5])
        {
         for (i4 = str[4]; i4 <= stp[4]; i4 += incr[4])
         {
          for (i3 = str[3]; i3 <= stp[3]; i3 += incr[3])
          {
           for (i2 = str[2]; i2 <= stp[2]; i2 += incr[2])
           {
            for (i1 = str[1]; i1 <= stp[1]; i1 += incr[1])
            {
              felem=str[0] + (i1 - 1) * dsize[1] + (i2 - 1) * dsize[2] + 
                             (i3 - 1) * dsize[3] + (i4 - 1) * dsize[4] +
                             (i5 - 1) * dsize[5] + (i6 - 1) * dsize[6] +
                             (i7 - 1) * dsize[7] + (i8 - 1) * dsize[8];

              if ( ffgcld(fptr, numcol, row, felem, nelem, ninc, nultyp,
                   nulval, &array[i0], &ldummy, &anyf, status) > 0)
                   return(*status);

              if (anyf)
                  *anynul = TRUE;

              i0 += nelem;
            }
           }
          }
         }
        }
       }
      }
     }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgsfb(fitsfile *fptr, /* I - FITS file pointer                         */
           int  colnum,    /* I - number of the column to read (1 = 1st)    */
           int naxis,      /* I - number of dimensions in the FITS array    */
           long  *naxes,   /* I - size of each dimension                    */
           long  *blc,     /* I - 'bottom left corner' of the subsection    */
           long  *trc,     /* I - 'top right corner' of the subsection      */
           long  *inc,     /* I - increment to be applied in each dimension */
           unsigned char *array, /* O - array to be filled and returned     */
           char *flagval,  /* O - set to 1 if corresponding value is null   */
           int  *anynul,   /* O - set to 1 if any values are null; else 0   */
           int  *status)   /* IO - error status                             */
/*
  Read a subsection of data values from an image or a table column.
  This routine is set up to handle a maximum of nine dimensions.
*/
{
    long ii,i0, i1,i2,i3,i4,i5,i6,i7,i8,row,rstr,rstp,rinc;
    long str[9],stp[9],incr[9],dsize[10];
    long felem, nelem, nultyp, ninc, numcol;
    int anyf;
    unsigned char nulval;
    char msg[FLEN_ERRMSG];

    if (naxis < 1 || naxis > 9)
    {
        sprintf(msg, "NAXIS = %d in call to ffgsvb is out of range", naxis);
        ffpmsg(msg);
        return(*status = BAD_DIMEN);
    }

/*
    if this is a primary array, then the input COLNUM parameter should
    be interpreted as the row number, and we will alway read the image
    data from column 2 (any group parameters are in column 1).
*/
    if (fptr->hdutype == 0)
    {
        /* this is a primary array, or image extension */
        if (colnum == 0)
        {
            rstr = 1;
            rstp = 1;
        }
        else
        {
            rstr = colnum - 1;
            rstp = colnum - 1;
        }
        rinc = 1;
        numcol = 2;
    }
    else
    {
        /* this is a table, so the row info is in the (naxis+1) elements */
        rstr = blc[naxis];
        rstp = trc[naxis];
        rinc = inc[naxis];
        numcol = colnum;
    }

    nultyp = 2;
    *anynul = FALSE;
    i0 = 0;
    for (ii = 0; ii < 9; ii++)
    {
        str[ii] = 1;
        stp[ii] = 1;
        incr[ii] = 1;
        dsize[ii] = 1;
    }

    for (ii = 0; ii < naxis; ii++)
    {
      if (trc[ii] < blc[ii])
      {
        sprintf(msg, "ffgsvb: illegal range specified for axis %d", ii + 1);
        ffpmsg(msg);
        return(*status = BAD_PIX_NUM);
      }

      str[ii] = blc[ii];
      stp[ii] = trc[ii];
      incr[ii] = inc[ii];
      dsize[ii + 1] = dsize[ii] * naxes[ii];
    }

    if (naxis == 1 && naxes[0] == 1)
    {
      /* This is not a vector column, so read all the rows at once */
      nelem = (rstp - rstr) / rinc + 1;
      ninc = rinc;
      rstp = rstr;
    }
    else
    {
      /* have to read each row individually, in all dimensions */
      nelem = (stp[0] - str[0]) / inc[0] + 1;
      ninc = incr[0];
    }

    for (row = rstr; row <= rstp; row += rinc)
    {
     for (i8 = str[8]; i8 <= stp[8]; i8 += incr[8])
     {
      for (i7 = str[7]; i7 <= stp[7]; i7 += incr[7])
      {
       for (i6 = str[6]; i6 <= stp[6]; i6 += incr[6])
       {
        for (i5 = str[5]; i5 <= stp[5]; i5 += incr[5])
        {
         for (i4 = str[4]; i4 <= stp[4]; i4 += incr[4])
         {
          for (i3 = str[3]; i3 <= stp[3]; i3 += incr[3])
          {
           for (i2 = str[2]; i2 <= stp[2]; i2 += incr[2])
           {
            for (i1 = str[1]; i1 <= stp[1]; i1 += incr[1])
            {
              felem=str[0] + (i1 - 1) * dsize[1] + (i2 - 1) * dsize[2] + 
                             (i3 - 1) * dsize[3] + (i4 - 1) * dsize[4] +
                             (i5 - 1) * dsize[5] + (i6 - 1) * dsize[6] +
                             (i7 - 1) * dsize[7] + (i8 - 1) * dsize[8];

              if ( ffgclb(fptr, numcol, row, felem, nelem, ninc, nultyp,
                   nulval, &array[i0], &flagval[i0], &anyf, status) > 0)
                   return(*status);

              if (anyf)
                  *anynul = TRUE;

              i0 += nelem;
            }
           }
          }
         }
        }
       }
      }
     }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgsfi(fitsfile *fptr, /* I - FITS file pointer                         */
           int  colnum,    /* I - number of the column to read (1 = 1st)    */
           int naxis,      /* I - number of dimensions in the FITS array    */
           long  *naxes,   /* I - size of each dimension                    */
           long  *blc,     /* I - 'bottom left corner' of the subsection    */
           long  *trc,     /* I - 'top right corner' of the subsection      */
           long  *inc,     /* I - increment to be applied in each dimension */
           short *array,   /* O - array to be filled and returned           */
           char *flagval,  /* O - set to 1 if corresponding value is null   */
           int  *anynul,   /* O - set to 1 if any values are null; else 0   */
           int  *status)   /* IO - error status                             */
/*
  Read a subsection of data values from an image or a table column.
  This routine is set up to handle a maximum of nine dimensions.
*/
{
    long ii,i0, i1,i2,i3,i4,i5,i6,i7,i8,row,rstr,rstp,rinc;
    long str[9],stp[9],incr[9],dsize[10];
    long felem, nelem, nultyp, ninc, numcol;
    int anyf;
    short nulval;
    char msg[FLEN_ERRMSG];

    if (naxis < 1 || naxis > 9)
    {
        sprintf(msg, "NAXIS = %d in call to ffgsvi is out of range", naxis);
        ffpmsg(msg);
        return(*status = BAD_DIMEN);
    }

/*
    if this is a primary array, then the input COLNUM parameter should
    be interpreted as the row number, and we will alway read the image
    data from column 2 (any group parameters are in column 1).
*/
    if (fptr->hdutype == 0)
    {
        /* this is a primary array, or image extension */
        if (colnum == 0)
        {
            rstr = 1;
            rstp = 1;
        }
        else
        {
            rstr = colnum - 1;
            rstp = colnum - 1;
        }
        rinc = 1;
        numcol = 2;
    }
    else
    {
        /* this is a table, so the row info is in the (naxis+1) elements */
        rstr = blc[naxis];
        rstp = trc[naxis];
        rinc = inc[naxis];
        numcol = colnum;
    }

    nultyp = 2;
    *anynul = FALSE;
    i0 = 0;
    for (ii = 0; ii < 9; ii++)
    {
        str[ii] = 1;
        stp[ii] = 1;
        incr[ii] = 1;
        dsize[ii] = 1;
    }

    for (ii = 0; ii < naxis; ii++)
    {
      if (trc[ii] < blc[ii])
      {
        sprintf(msg, "ffgsvi: illegal range specified for axis %d", ii + 1);
        ffpmsg(msg);
        return(*status = BAD_PIX_NUM);
      }

      str[ii] = blc[ii];
      stp[ii] = trc[ii];
      incr[ii] = inc[ii];
      dsize[ii + 1] = dsize[ii] * naxes[ii];
    }

    if (naxis == 1 && naxes[0] == 1)
    {
      /* This is not a vector column, so read all the rows at once */
      nelem = (rstp - rstr) / rinc + 1;
      ninc = rinc;
      rstp = rstr;
    }
    else
    {
      /* have to read each row individually, in all dimensions */
      nelem = (stp[0] - str[0]) / inc[0] + 1;
      ninc = incr[0];
    }

    for (row = rstr; row <= rstp; row += rinc)
    {
     for (i8 = str[8]; i8 <= stp[8]; i8 += incr[8])
     {
      for (i7 = str[7]; i7 <= stp[7]; i7 += incr[7])
      {
       for (i6 = str[6]; i6 <= stp[6]; i6 += incr[6])
       {
        for (i5 = str[5]; i5 <= stp[5]; i5 += incr[5])
        {
         for (i4 = str[4]; i4 <= stp[4]; i4 += incr[4])
         {
          for (i3 = str[3]; i3 <= stp[3]; i3 += incr[3])
          {
           for (i2 = str[2]; i2 <= stp[2]; i2 += incr[2])
           {
            for (i1 = str[1]; i1 <= stp[1]; i1 += incr[1])
            {
              felem=str[0] + (i1 - 1) * dsize[1] + (i2 - 1) * dsize[2] + 
                             (i3 - 1) * dsize[3] + (i4 - 1) * dsize[4] +
                             (i5 - 1) * dsize[5] + (i6 - 1) * dsize[6] +
                             (i7 - 1) * dsize[7] + (i8 - 1) * dsize[8];

              if ( ffgcli(fptr, numcol, row, felem, nelem, ninc, nultyp,
                   nulval, &array[i0], &flagval[i0], &anyf, status) > 0)
                   return(*status);

              if (anyf)
                  *anynul = TRUE;

              i0 += nelem;
            }
           }
          }
         }
        }
       }
      }
     }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgsfj(fitsfile *fptr, /* I - FITS file pointer                         */
           int  colnum,    /* I - number of the column to read (1 = 1st)    */
           int naxis,      /* I - number of dimensions in the FITS array    */
           long  *naxes,   /* I - size of each dimension                    */
           long  *blc,     /* I - 'bottom left corner' of the subsection    */
           long  *trc,     /* I - 'top right corner' of the subsection      */
           long  *inc,     /* I - increment to be applied in each dimension */
           long *array,    /* O - array to be filled and returned           */
           char *flagval,  /* O - set to 1 if corresponding value is null   */
           int  *anynul,   /* O - set to 1 if any values are null; else 0   */
           int  *status)   /* IO - error status                             */
/*
  Read a subsection of data values from an image or a table column.
  This routine is set up to handle a maximum of nine dimensions.
*/
{
    long ii,i0, i1,i2,i3,i4,i5,i6,i7,i8,row,rstr,rstp,rinc;
    long str[9],stp[9],incr[9],dsize[10];
    long felem, nelem, nultyp, ninc, numcol;
    long nulval;
    int anyf;
    char msg[FLEN_ERRMSG];

    if (naxis < 1 || naxis > 9)
    {
        sprintf(msg, "NAXIS = %d in call to ffgsvj is out of range", naxis);
        ffpmsg(msg);
        return(*status = BAD_DIMEN);
    }

/*
    if this is a primary array, then the input COLNUM parameter should
    be interpreted as the row number, and we will alway read the image
    data from column 2 (any group parameters are in column 1).
*/
    if (fptr->hdutype == 0)
    {
        /* this is a primary array, or image extension */
        if (colnum == 0)
        {
            rstr = 1;
            rstp = 1;
        }
        else
        {
            rstr = colnum - 1;
            rstp = colnum - 1;
        }
        rinc = 1;
        numcol = 2;
    }
    else
    {
        /* this is a table, so the row info is in the (naxis+1) elements */
        rstr = blc[naxis];
        rstp = trc[naxis];
        rinc = inc[naxis];
        numcol = colnum;
    }

    nultyp = 2;
    *anynul = FALSE;
    i0 = 0;
    for (ii = 0; ii < 9; ii++)
    {
        str[ii] = 1;
        stp[ii] = 1;
        incr[ii] = 1;
        dsize[ii] = 1;
    }

    for (ii = 0; ii < naxis; ii++)
    {
      if (trc[ii] < blc[ii])
      {
        sprintf(msg, "ffgsvj: illegal range specified for axis %d", ii + 1);
        ffpmsg(msg);
        return(*status = BAD_PIX_NUM);
      }

      str[ii] = blc[ii];
      stp[ii] = trc[ii];
      incr[ii] = inc[ii];
      dsize[ii + 1] = dsize[ii] * naxes[ii];
    }

    if (naxis == 1 && naxes[0] == 1)
    {
      /* This is not a vector column, so read all the rows at once */
      nelem = (rstp - rstr) / rinc + 1;
      ninc = rinc;
      rstp = rstr;
    }
    else
    {
      /* have to read each row individually, in all dimensions */
      nelem = (stp[0] - str[0]) / inc[0] + 1;
      ninc = incr[0];
    }

    for (row = rstr; row <= rstp; row += rinc)
    {
     for (i8 = str[8]; i8 <= stp[8]; i8 += incr[8])
     {
      for (i7 = str[7]; i7 <= stp[7]; i7 += incr[7])
      {
       for (i6 = str[6]; i6 <= stp[6]; i6 += incr[6])
       {
        for (i5 = str[5]; i5 <= stp[5]; i5 += incr[5])
        {
         for (i4 = str[4]; i4 <= stp[4]; i4 += incr[4])
         {
          for (i3 = str[3]; i3 <= stp[3]; i3 += incr[3])
          {
           for (i2 = str[2]; i2 <= stp[2]; i2 += incr[2])
           {
            for (i1 = str[1]; i1 <= stp[1]; i1 += incr[1])
            {
              felem=str[0] + (i1 - 1) * dsize[1] + (i2 - 1) * dsize[2] + 
                             (i3 - 1) * dsize[3] + (i4 - 1) * dsize[4] +
                             (i5 - 1) * dsize[5] + (i6 - 1) * dsize[6] +
                             (i7 - 1) * dsize[7] + (i8 - 1) * dsize[8];

              if ( ffgclj(fptr, numcol, row, felem, nelem, ninc, nultyp,
                   nulval, &array[i0], &flagval[i0], &anyf, status) > 0)
                   return(*status);

              if (anyf)
                  *anynul = TRUE;

              i0 += nelem;
            }
           }
          }
         }
        }
       }
      }
     }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgsfk(fitsfile *fptr, /* I - FITS file pointer                         */
           int  colnum,    /* I - number of the column to read (1 = 1st)    */
           int naxis,      /* I - number of dimensions in the FITS array    */
           long  *naxes,   /* I - size of each dimension                    */
           long  *blc,     /* I - 'bottom left corner' of the subsection    */
           long  *trc,     /* I - 'top right corner' of the subsection      */
           long  *inc,     /* I - increment to be applied in each dimension */
           int  *array,    /* O - array to be filled and returned           */
           char *flagval,  /* O - set to 1 if corresponding value is null   */
           int  *anynul,   /* O - set to 1 if any values are null; else 0   */
           int  *status)   /* IO - error status                             */
/*
  Read a subsection of data values from an image or a table column.
  This routine is set up to handle a maximum of nine dimensions.
*/
{
    long ii,i0, i1,i2,i3,i4,i5,i6,i7,i8,row,rstr,rstp,rinc;
    long str[9],stp[9],incr[9],dsize[10];
    long felem, nelem, nultyp, ninc, numcol;
    long nulval;
    int anyf;
    char msg[FLEN_ERRMSG];

    if (naxis < 1 || naxis > 9)
    {
        sprintf(msg, "NAXIS = %d in call to ffgsvj is out of range", naxis);
        ffpmsg(msg);
        return(*status = BAD_DIMEN);
    }

/*
    if this is a primary array, then the input COLNUM parameter should
    be interpreted as the row number, and we will alway read the image
    data from column 2 (any group parameters are in column 1).
*/
    if (fptr->hdutype == 0)
    {
        /* this is a primary array, or image extension */
        if (colnum == 0)
        {
            rstr = 1;
            rstp = 1;
        }
        else
        {
            rstr = colnum - 1;
            rstp = colnum - 1;
        }
        rinc = 1;
        numcol = 2;
    }
    else
    {
        /* this is a table, so the row info is in the (naxis+1) elements */
        rstr = blc[naxis];
        rstp = trc[naxis];
        rinc = inc[naxis];
        numcol = colnum;
    }

    nultyp = 2;
    *anynul = FALSE;
    i0 = 0;
    for (ii = 0; ii < 9; ii++)
    {
        str[ii] = 1;
        stp[ii] = 1;
        incr[ii] = 1;
        dsize[ii] = 1;
    }

    for (ii = 0; ii < naxis; ii++)
    {
      if (trc[ii] < blc[ii])
      {
        sprintf(msg, "ffgsvj: illegal range specified for axis %d", ii + 1);
        ffpmsg(msg);
        return(*status = BAD_PIX_NUM);
      }

      str[ii] = blc[ii];
      stp[ii] = trc[ii];
      incr[ii] = inc[ii];
      dsize[ii + 1] = dsize[ii] * naxes[ii];
    }

    if (naxis == 1 && naxes[0] == 1)
    {
      /* This is not a vector column, so read all the rows at once */
      nelem = (rstp - rstr) / rinc + 1;
      ninc = rinc;
      rstp = rstr;
    }
    else
    {
      /* have to read each row individually, in all dimensions */
      nelem = (stp[0] - str[0]) / inc[0] + 1;
      ninc = incr[0];
    }

    for (row = rstr; row <= rstp; row += rinc)
    {
     for (i8 = str[8]; i8 <= stp[8]; i8 += incr[8])
     {
      for (i7 = str[7]; i7 <= stp[7]; i7 += incr[7])
      {
       for (i6 = str[6]; i6 <= stp[6]; i6 += incr[6])
       {
        for (i5 = str[5]; i5 <= stp[5]; i5 += incr[5])
        {
         for (i4 = str[4]; i4 <= stp[4]; i4 += incr[4])
         {
          for (i3 = str[3]; i3 <= stp[3]; i3 += incr[3])
          {
           for (i2 = str[2]; i2 <= stp[2]; i2 += incr[2])
           {
            for (i1 = str[1]; i1 <= stp[1]; i1 += incr[1])
            {
              felem=str[0] + (i1 - 1) * dsize[1] + (i2 - 1) * dsize[2] + 
                             (i3 - 1) * dsize[3] + (i4 - 1) * dsize[4] +
                             (i5 - 1) * dsize[5] + (i6 - 1) * dsize[6] +
                             (i7 - 1) * dsize[7] + (i8 - 1) * dsize[8];

              if ( ffgclk(fptr, numcol, row, felem, nelem, ninc, nultyp,
                   nulval, &array[i0], &flagval[i0], &anyf, status) > 0)
                   return(*status);

              if (anyf)
                  *anynul = TRUE;

              i0 += nelem;
            }
           }
          }
         }
        }
       }
      }
     }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgsfe(fitsfile *fptr, /* I - FITS file pointer                         */
           int  colnum,    /* I - number of the column to read (1 = 1st)    */
           int naxis,      /* I - number of dimensions in the FITS array    */
           long  *naxes,   /* I - size of each dimension                    */
           long  *blc,     /* I - 'bottom left corner' of the subsection    */
           long  *trc,     /* I - 'top right corner' of the subsection      */
           long  *inc,     /* I - increment to be applied in each dimension */
           float *array,   /* O - array to be filled and returned           */
           char *flagval,  /* O - set to 1 if corresponding value is null   */
           int  *anynul,   /* O - set to 1 if any values are null; else 0   */
           int  *status)   /* IO - error status                             */
/*
  Read a subsection of data values from an image or a table column.
  This routine is set up to handle a maximum of nine dimensions.
*/
{
    long ii,i0, i1,i2,i3,i4,i5,i6,i7,i8,row,rstr,rstp,rinc;
    long str[9],stp[9],incr[9],dsize[10];
    long felem, nelem, nultyp, ninc, numcol;
    int anyf;
    float nulval;
    char msg[FLEN_ERRMSG];

    if (naxis < 1 || naxis > 9)
    {
        sprintf(msg, "NAXIS = %d in call to ffgsve is out of range", naxis);
        ffpmsg(msg);
        return(*status = BAD_DIMEN);
    }

/*
    if this is a primary array, then the input COLNUM parameter should
    be interpreted as the row number, and we will alway read the image
    data from column 2 (any group parameters are in column 1).
*/
    if (fptr->hdutype == 0)
    {
        /* this is a primary array, or image extension */
        if (colnum == 0)
        {
            rstr = 1;
            rstp = 1;
        }
        else
        {
            rstr = colnum - 1;
            rstp = colnum - 1;
        }
        rinc = 1;
        numcol = 2;
    }
    else
    {
        /* this is a table, so the row info is in the (naxis+1) elements */
        rstr = blc[naxis];
        rstp = trc[naxis];
        rinc = inc[naxis];
        numcol = colnum;
    }

    nultyp = 2;
    *anynul = FALSE;
    i0 = 0;
    for (ii = 0; ii < 9; ii++)
    {
        str[ii] = 1;
        stp[ii] = 1;
        incr[ii] = 1;
        dsize[ii] = 1;
    }

    for (ii = 0; ii < naxis; ii++)
    {
      if (trc[ii] < blc[ii])
      {
        sprintf(msg, "ffgsve: illegal range specified for axis %d", ii + 1);
        ffpmsg(msg);
        return(*status = BAD_PIX_NUM);
      }

      str[ii] = blc[ii];
      stp[ii] = trc[ii];
      incr[ii] = inc[ii];
      dsize[ii + 1] = dsize[ii] * naxes[ii];
    }

    if (naxis == 1 && naxes[0] == 1)
    {
      /* This is not a vector column, so read all the rows at once */
      nelem = (rstp - rstr) / rinc + 1;
      ninc = rinc;
      rstp = rstr;
    }
    else
    {
      /* have to read each row individually, in all dimensions */
      nelem = (stp[0] - str[0]) / inc[0] + 1;
      ninc = incr[0];
    }

    for (row = rstr; row <= rstp; row += rinc)
    {
     for (i8 = str[8]; i8 <= stp[8]; i8 += incr[8])
     {
      for (i7 = str[7]; i7 <= stp[7]; i7 += incr[7])
      {
       for (i6 = str[6]; i6 <= stp[6]; i6 += incr[6])
       {
        for (i5 = str[5]; i5 <= stp[5]; i5 += incr[5])
        {
         for (i4 = str[4]; i4 <= stp[4]; i4 += incr[4])
         {
          for (i3 = str[3]; i3 <= stp[3]; i3 += incr[3])
          {
           for (i2 = str[2]; i2 <= stp[2]; i2 += incr[2])
           {
            for (i1 = str[1]; i1 <= stp[1]; i1 += incr[1])
            {
              felem=str[0] + (i1 - 1) * dsize[1] + (i2 - 1) * dsize[2] + 
                             (i3 - 1) * dsize[3] + (i4 - 1) * dsize[4] +
                             (i5 - 1) * dsize[5] + (i6 - 1) * dsize[6] +
                             (i7 - 1) * dsize[7] + (i8 - 1) * dsize[8];

              if ( ffgcle(fptr, numcol, row, felem, nelem, ninc, nultyp,
                   nulval, &array[i0], &flagval[i0], &anyf, status) > 0)
                   return(*status);

              if (anyf)
                  *anynul = TRUE;

              i0 += nelem;
            }
           }
          }
         }
        }
       }
      }
     }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgsfd(fitsfile *fptr, /* I - FITS file pointer                         */
           int  colnum,    /* I - number of the column to read (1 = 1st)    */
           int naxis,      /* I - number of dimensions in the FITS array    */
           long  *naxes,   /* I - size of each dimension                    */
           long  *blc,     /* I - 'bottom left corner' of the subsection    */
           long  *trc,     /* I - 'top right corner' of the subsection      */
           long  *inc,     /* I - increment to be applied in each dimension */
           double *array,  /* O - array to be filled and returned           */
           char *flagval,  /* O - set to 1 if corresponding value is null   */
           int  *anynul,   /* O - set to 1 if any values are null; else 0   */
           int  *status)   /* IO - error status                             */
/*
  Read a subsection of data values from an image or a table column.
  This routine is set up to handle a maximum of nine dimensions.
*/
{
    long ii,i0, i1,i2,i3,i4,i5,i6,i7,i8,row,rstr,rstp,rinc;
    long str[9],stp[9],incr[9],dsize[10];
    long felem, nelem, nultyp, ninc, numcol;
    int anyf;
    double nulval;
    char msg[FLEN_ERRMSG];

    if (naxis < 1 || naxis > 9)
    {
        sprintf(msg, "NAXIS = %d in call to ffgsvd is out of range", naxis);
        ffpmsg(msg);
        return(*status = BAD_DIMEN);
    }

/*
    if this is a primary array, then the input COLNUM parameter should
    be interpreted as the row number, and we will alway read the image
    data from column 2 (any group parameters are in column 1).
*/
    if (fptr->hdutype == 0)
    {
        /* this is a primary array, or image extension */
        if (colnum == 0)
        {
            rstr = 1;
            rstp = 1;
        }
        else
        {
            rstr = colnum - 1;
            rstp = colnum - 1;
        }
        rinc = 1;
        numcol = 2;
    }
    else
    {
        /* this is a table, so the row info is in the (naxis+1) elements */
        rstr = blc[naxis];
        rstp = trc[naxis];
        rinc = inc[naxis];
        numcol = colnum;
    }

    nultyp = 2;
    *anynul = FALSE;
    i0 = 0;
    for (ii = 0; ii < 9; ii++)
    {
        str[ii] = 1;
        stp[ii] = 1;
        incr[ii] = 1;
        dsize[ii] = 1;
    }

    for (ii = 0; ii < naxis; ii++)
    {
      if (trc[ii] < blc[ii])
      {
        sprintf(msg, "ffgsvd: illegal range specified for axis %d", ii + 1);
        ffpmsg(msg);
        return(*status = BAD_PIX_NUM);
      }

      str[ii] = blc[ii];
      stp[ii] = trc[ii];
      incr[ii] = inc[ii];
      dsize[ii + 1] = dsize[ii] * naxes[ii];
    }

    if (naxis == 1 && naxes[0] == 1)
    {
      /* This is not a vector column, so read all the rows at once */
      nelem = (rstp - rstr) / rinc + 1;
      ninc = rinc;
      rstp = rstr;
    }
    else
    {
      /* have to read each row individually, in all dimensions */
      nelem = (stp[0] - str[0]) / inc[0] + 1;
      ninc = incr[0];
    }

    for (row = rstr; row <= rstp; row += rinc)
    {
     for (i8 = str[8]; i8 <= stp[8]; i8 += incr[8])
     {
      for (i7 = str[7]; i7 <= stp[7]; i7 += incr[7])
      {
       for (i6 = str[6]; i6 <= stp[6]; i6 += incr[6])
       {
        for (i5 = str[5]; i5 <= stp[5]; i5 += incr[5])
        {
         for (i4 = str[4]; i4 <= stp[4]; i4 += incr[4])
         {
          for (i3 = str[3]; i3 <= stp[3]; i3 += incr[3])
          {
           for (i2 = str[2]; i2 <= stp[2]; i2 += incr[2])
           {
            for (i1 = str[1]; i1 <= stp[1]; i1 += incr[1])
            {
              felem=str[0] + (i1 - 1) * dsize[1] + (i2 - 1) * dsize[2] + 
                             (i3 - 1) * dsize[3] + (i4 - 1) * dsize[4] +
                             (i5 - 1) * dsize[5] + (i6 - 1) * dsize[6] +
                             (i7 - 1) * dsize[7] + (i8 - 1) * dsize[8];

              if ( ffgcld(fptr, numcol, row, felem, nelem, ninc, nultyp,
                   nulval, &array[i0], &flagval[i0], &anyf, status) > 0)
                   return(*status);

              if (anyf)
                  *anynul = TRUE;

              i0 += nelem;
            }
           }
          }
         }
        }
       }
      }
     }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffggpb( fitsfile *fptr,   /* I - FITS file pointer                       */
            long  group,      /* I - group to read (1 = 1st group)           */
            long  firstelem,  /* I - first vector element to read (1 = 1st)  */
            long  nelem,      /* I - number of values to read                */
            unsigned char *array, /* O - array of values that are returned   */
            int  *status)     /* IO - error status                           */
/*
  Read an array of group parameters from the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of
  the FITS array is not the same as the array being read).
*/
{
    long row;
    int idummy;
    char *cdummy;
    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */

    row=maxvalue(1,group);

    ffgclb(fptr, 1, row, firstelem, nelem, 1, 1, 0,
               array, cdummy, &idummy, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffggpi( fitsfile *fptr,   /* I - FITS file pointer                       */
            long  group,      /* I - group to read (1 = 1st group)           */
            long  firstelem,  /* I - first vector element to read (1 = 1st)  */
            long  nelem,      /* I - number of values to read                */
            short *array,     /* O - array of values that are returned       */
            int  *status)     /* IO - error status                           */
/*
  Read an array of group parameters from the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of
  the FITS array is not the same as the array being read).
*/
{
    long row;
    int idummy;
    char *cdummy;
    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */

    row=maxvalue(1,group);

    ffgcli(fptr, 1, row, firstelem, nelem, 1, 1, 0,
               array, cdummy, &idummy, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffggpj( fitsfile *fptr,   /* I - FITS file pointer                       */
            long  group,      /* I - group to read (1 = 1st group)           */
            long  firstelem,  /* I - first vector element to read (1 = 1st)  */
            long  nelem,      /* I - number of values to read                */
            long  *array,     /* O - array of values that are returned       */
            int  *status)     /* IO - error status                           */
/*
  Read an array of group parameters from the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of
  the FITS array is not the same as the array being read).
*/
{
    long row;
    int idummy;
    char *cdummy;
    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */

    row=maxvalue(1,group);

    ffgclj(fptr, 1, row, firstelem, nelem, 1, 1, 0L,
               array, cdummy, &idummy, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffggpk( fitsfile *fptr,   /* I - FITS file pointer                       */
            long  group,      /* I - group to read (1 = 1st group)           */
            long  firstelem,  /* I - first vector element to read (1 = 1st)  */
            long  nelem,      /* I - number of values to read                */
            int  *array,     /* O - array of values that are returned       */
            int  *status)     /* IO - error status                           */
/*
  Read an array of group parameters from the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of
  the FITS array is not the same as the array being read).
*/
{
    long row;
    int idummy;
    char *cdummy;
    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */

    row=maxvalue(1,group);

    ffgclk(fptr, 1, row, firstelem, nelem, 1, 1, 0L,
               array, cdummy, &idummy, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffggpe( fitsfile *fptr,   /* I - FITS file pointer                       */
            long  group,      /* I - group to read (1 = 1st group)           */
            long  firstelem,  /* I - first vector element to read (1 = 1st)  */
            long  nelem,      /* I - number of values to read                */
            float *array,     /* O - array of values that are returned       */
            int  *status)     /* IO - error status                           */
/*
  Read an array of group parameters from the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of
  the FITS array is not the same as the array being read).
*/
{
    long row;
    int idummy;
    char *cdummy;
    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */

    row=maxvalue(1,group);

    ffgcle(fptr, 1, row, firstelem, nelem, 1, 1, 0.F,
               array, cdummy, &idummy, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffggpd( fitsfile *fptr,   /* I - FITS file pointer                       */
            long  group,      /* I - group to read (1 = 1st group)           */
            long  firstelem,  /* I - first vector element to read (1 = 1st)  */
            long  nelem,      /* I - number of values to read                */
            double *array,    /* O - array of values that are returned       */
            int  *status)     /* IO - error status                           */
/*
  Read an array of group parameters from the primary array. Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of
  the FITS array is not the same as the array being read).
*/
{
    long row;
    int idummy;
    char *cdummy;
    /*
      the primary array is represented as a binary table:
      each group of the primary array is a row in the table,
      where the first column contains the group parameters
      and the second column contains the image itself.
    */

    row=maxvalue(1,group);

    ffgcld(fptr, 1, row, firstelem, nelem, 1, 1, 0.,
               array, cdummy, &idummy, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgcv(  fitsfile *fptr,   /* I - FITS file pointer                       */
            int  datatype,    /* I - datatype of the value                   */
            int  colnum,      /* I - number of column to write (1 = 1st col) */
            long  firstrow,   /* I - first row to write (1 = 1st row)        */
            long  firstelem,  /* I - first vector element to read (1 = 1st)  */
            long nelem,       /* I - number of values to read                */
            void *nulval,     /* I - value for undefined pixels              */
            void *array,      /* O - array of values that are returned       */
            int  *anynul,     /* O - set to 1 if any values are null; else 0 */
            int  *status)     /* IO - error status                           */
/*
  Read an array of values from a table column. The datatype of the
  input array is defined by the 2nd argument.  Data conversion
  and scaling will be performed if necessary (e.g, if the datatype of
  the FITS array is not the same as the array being read).
  Undefined elements will be set equal to NULVAL, unless NULVAL=0
  in which case no checking for undefined values will be performed.
  ANYNUL is returned with a value of true if any pixels are undefined.
*/
{
    char cdummy[2];

    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    if (datatype == TBYTE)
    {
      if (nulval == 0)
        ffgclb(fptr, colnum, firstrow, firstelem, nelem, 1, 1, 0,
              (unsigned char *) array, cdummy, anynul, status);
      else
       ffgclb(fptr, colnum, firstrow, firstelem, nelem, 1, 1, *(unsigned char *)
              nulval, (unsigned char *) array, cdummy, anynul, status);
    }
    else if (datatype == TSHORT)
    {
      if (nulval == 0)
        ffgcli(fptr, colnum, firstrow, firstelem, nelem, 1, 1, 0,
              (short *) array, cdummy, anynul, status);
      else
        ffgcli(fptr, colnum, firstrow, firstelem, nelem, 1, 1, *(short *)
              nulval, (short *) array, cdummy, anynul, status);
    }
    else if (datatype == TINT)
    {
      if (nulval == 0)
        ffgclk(fptr, colnum, firstrow, firstelem, nelem, 1, 1, 0,
              (int *) array, cdummy, anynul, status);
      else
         ffgclk(fptr, colnum, firstrow, firstelem, nelem, 1, 1, *(int *)
            nulval, (int *) array, cdummy, anynul, status);
    }
    else if (datatype == TLONG)
    {
      if (nulval == 0)
        ffgclj(fptr, colnum, firstrow, firstelem, nelem, 1, 1, 0,
              (long *) array, cdummy, anynul, status);
      else
        ffgclj(fptr, colnum, firstrow, firstelem, nelem, 1, 1, *(long *)
              nulval, (long *) array, cdummy, anynul, status);
    }
    else if (datatype == TFLOAT)
    {
      if (nulval == 0)
        ffgcle(fptr, colnum, firstrow, firstelem, nelem, 1, 1, 0.,
              (float *) array, cdummy, anynul, status);
      else
      ffgcle(fptr, colnum, firstrow, firstelem, nelem, 1, 1, *(float *)
               nulval,(float *) array, cdummy, anynul, status);
    }
    else if (datatype == TDOUBLE)
    {
      if (nulval == 0)
        ffgcld(fptr, colnum, firstrow, firstelem, nelem, 1, 1, 0.,
              (double *) array, cdummy, anynul, status);
      else
        ffgcld(fptr, colnum, firstrow, firstelem, nelem, 1, 1, *(double *)
              nulval, (double *) array, cdummy, anynul, status);
    }
    else if (datatype == TSTRING)
    {
      if (nulval == 0)
      {
        cdummy[0] = '\0';
        ffgcls(fptr, colnum, firstrow, firstelem, nelem, 1, 
             cdummy, (char **) array, cdummy, anynul, status);
      }
      else
        ffgcls(fptr, colnum, firstrow, firstelem, nelem, 1, (char *)
             nulval, (char **) array, cdummy, anynul, status);
    }
    else
      *status = BAD_DATATYPE;

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgcvs( fitsfile *fptr,   /* I - FITS file pointer                       */
            int  colnum,      /* I - number of column to read (1 = 1st col)  */
            long  firstrow,   /* I - first row to read (1 = 1st row)         */
            long  firstelem,  /* I - first vector element to read (1 = 1st)  */
            long  nelem,      /* I - number of strings to read               */
            char *nulval,     /* I - string for null pixels                  */
            char **array,     /* O - array of values that are read           */
            int  *anynul,     /* O - set to 1 if any values are null; else 0 */
            int  *status)     /* IO - error status                           */
/*
  Read an array of string values from a column in the current FITS HDU.
  Any undefined pixels will be set equal to the value of 'nulval' unless
  nulval = null in which case no checks for undefined pixels will be made.
*/
{
    char dummy[2];

    ffgcls(fptr, colnum, firstrow, firstelem, nelem, 1, nulval,
           array, dummy, anynul, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgcl(  fitsfile *fptr,   /* I - FITS file pointer                       */
            int  colnum,      /* I - number of column to read (1 = 1st col)  */
            long  firstrow,   /* I - first row to read (1 = 1st row)         */
            long  firstelem,  /* I - first vector element to read (1 = 1st)  */
            long  nelem,      /* I - number of values to read                */
            char *array,      /* O - array of values                         */
            int  *status)     /* IO - error status                           */
/*
  Read an array of logical values from a column in the current FITS HDU.
  No checking for null values will be performed.
*/
{
    char cdummy[2];
    int idummy;

    cdummy[0] = 127;   /* special flag value to signal no null detection */
    idummy = 127;

    ffgcfl( fptr, colnum, firstrow, firstelem, nelem, array,
            cdummy, &idummy, status);

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgcvb(fitsfile *fptr,   /* I - FITS file pointer                       */
           int  colnum,      /* I - number of column to read (1 = 1st col)  */
           long  firstrow,   /* I - first row to read (1 = 1st row)         */
           long  firstelem,  /* I - first vector element to read (1 = 1st)  */
           long  nelem,      /* I - number of values to read                */
           unsigned char nulval, /* I - value for null pixels               */
           unsigned char *array, /* O - array of values that are read       */
           int  *anynul,     /* O - set to 1 if any values are null; else 0 */
           int  *status)     /* IO - error status                           */
/*
  Read an array of values from a column in the current FITS HDU. Automatic
  datatype conversion will be performed if the datatype of the column does not
  match the datatype of the array parameter. The output values will be scaled 
  by the FITS TSCALn and TZEROn values if these values have been defined.
  Any undefined pixels will be set equal to the value of 'nulval' unless
  nulval = 0 in which case no checks for undefined pixels will be made.
*/
{
    char *cdummy;

    ffgclb(fptr, colnum, firstrow, firstelem, nelem, 1, 1, nulval,
           array, cdummy, anynul, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgcvi(fitsfile *fptr,   /* I - FITS file pointer                       */
           int  colnum,      /* I - number of column to read (1 = 1st col)  */
           long  firstrow,   /* I - first row to read (1 = 1st row)         */
           long  firstelem,  /* I - first vector element to read (1 = 1st)  */
           long  nelem,      /* I - number of values to read                */
           short nulval,     /* I - value for null pixels                   */
           short *array,     /* O - array of values that are read           */
           int  *anynul,     /* O - set to 1 if any values are null; else 0 */
           int  *status)     /* IO - error status                           */
/*
  Read an array of values from a column in the current FITS HDU. Automatic
  datatype conversion will be performed if the datatype of the column does not
  match the datatype of the array parameter. The output values will be scaled 
  by the FITS TSCALn and TZEROn values if these values have been defined.
  Any undefined pixels will be set equal to the value of 'nulval' unless
  nulval = 0 in which case no checks for undefined pixels will be made.
*/
{
    char *cdummy;

    ffgcli(fptr, colnum, firstrow, firstelem, nelem, 1, 1, nulval,
           array, cdummy, anynul, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgcvj(fitsfile *fptr,   /* I - FITS file pointer                       */
           int  colnum,      /* I - number of column to read (1 = 1st col)  */
           long  firstrow,   /* I - first row to read (1 = 1st row)         */
           long  firstelem,  /* I - first vector element to read (1 = 1st)  */
           long  nelem,      /* I - number of values to read                */
           long  nulval,     /* I - value for null pixels                   */
           long *array,      /* O - array of values that are read           */
           int  *anynul,     /* O - set to 1 if any values are null; else 0 */
           int  *status)     /* IO - error status                           */
/*
  Read an array of values from a column in the current FITS HDU. Automatic
  datatype conversion will be performed if the datatype of the column does not
  match the datatype of the array parameter. The output values will be scaled 
  by the FITS TSCALn and TZEROn values if these values have been defined.
  Any undefined pixels will be set equal to the value of 'nulval' unless
  nulval = 0 in which case no checks for undefined pixels will be made.
*/
{
    char *cdummy;

    ffgclj(fptr, colnum, firstrow, firstelem, nelem, 1, 1, nulval,
           array, cdummy, anynul, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgcvk(fitsfile *fptr,   /* I - FITS file pointer                       */
           int  colnum,      /* I - number of column to read (1 = 1st col)  */
           long  firstrow,   /* I - first row to read (1 = 1st row)         */
           long  firstelem,  /* I - first vector element to read (1 = 1st)  */
           long  nelem,      /* I - number of values to read                */
           int   nulval,     /* I - value for null pixels                   */
           int  *array,      /* O - array of values that are read           */
           int  *anynul,     /* O - set to 1 if any values are null; else 0 */
           int  *status)     /* IO - error status                           */
/*
  Read an array of values from a column in the current FITS HDU. Automatic
  datatype conversion will be performed if the datatype of the column does not
  match the datatype of the array parameter. The output values will be scaled 
  by the FITS TSCALn and TZEROn values if these values have been defined.
  Any undefined pixels will be set equal to the value of 'nulval' unless
  nulval = 0 in which case no checks for undefined pixels will be made.
*/
{
    char *cdummy;

    ffgclk(fptr, colnum, firstrow, firstelem, nelem, 1, 1, nulval,
           array, cdummy, anynul, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgcve(fitsfile *fptr,   /* I - FITS file pointer                       */
           int  colnum,      /* I - number of column to read (1 = 1st col)  */
           long  firstrow,   /* I - first row to read (1 = 1st row)         */
           long  firstelem,  /* I - first vector element to read (1 = 1st)  */
           long  nelem,      /* I - number of values to read                */
           float nulval,     /* I - value for null pixels                   */
           float *array,     /* O - array of values that are read           */
           int  *anynul,     /* O - set to 1 if any values are null; else 0 */
           int  *status)     /* IO - error status                           */
/*
  Read an array of values from a column in the current FITS HDU. Automatic
  datatype conversion will be performed if the datatype of the column does not
  match the datatype of the array parameter. The output values will be scaled 
  by the FITS TSCALn and TZEROn values if these values have been defined.
  Any undefined pixels will be set equal to the value of 'nulval' unless
  nulval = 0 in which case no checks for undefined pixels will be made.
*/
{
    char *cdummy;

    ffgcle(fptr, colnum, firstrow, firstelem, nelem, 1, 1, nulval,
           array, cdummy, anynul, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgcvd(fitsfile *fptr,   /* I - FITS file pointer                       */
           int  colnum,      /* I - number of column to read (1 = 1st col)  */
           long  firstrow,   /* I - first row to read (1 = 1st row)         */
           long  firstelem,  /* I - first vector element to read (1 = 1st)  */
           long  nelem,      /* I - number of values to read                */
           double nulval,    /* I - value for null pixels                   */
           double *array,    /* O - array of values that are read           */
           int  *anynul,     /* O - set to 1 if any values are null; else 0 */
           int  *status)     /* IO - error status                           */
/*
  Read an array of values from a column in the current FITS HDU. Automatic
  datatype conversion will be performed if the datatype of the column does not
  match the datatype of the array parameter. The output values will be scaled 
  by the FITS TSCALn and TZEROn values if these values have been defined.
  Any undefined pixels will be set equal to the value of 'nulval' unless
  nulval = 0 in which case no checks for undefined pixels will be made.
*/
{
    char *cdummy;

    ffgcld(fptr, colnum, firstrow, firstelem, nelem, 1, 1, nulval,
           array, cdummy, anynul, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgcvc(fitsfile *fptr,   /* I - FITS file pointer                       */
           int  colnum,      /* I - number of column to read (1 = 1st col)  */
           long  firstrow,   /* I - first row to read (1 = 1st row)         */
           long  firstelem,  /* I - first vector element to read (1 = 1st)  */
           long  nelem,      /* I - number of values to read                */
           float nulval,     /* I - value for null pixels                   */
           float *array,     /* O - array of values that are read           */
           int  *anynul,     /* O - set to 1 if any values are null; else 0 */
           int  *status)     /* IO - error status                           */
/*
  Read an array of values from a column in the current FITS HDU. Automatic
  datatype conversion will be performed if the datatype of the column does not
  match the datatype of the array parameter. The output values will be scaled 
  by the FITS TSCALn and TZEROn values if these values have been defined.
  Any undefined pixels will be set equal to the value of 'nulval' unless
  nulval = 0 in which case no checks for undefined pixels will be made.

  TSCAL and ZERO should not be used with complex values. 
*/
{
    long firstvalue, nvalue;
    char *cdummy;

    /* a complex value is interpreted as a pair of float values, thus */
    /* need to multiply the first element and number of elements by 2 */

    firstvalue = (firstelem - 1) * 2 + 1;
    nvalue = nelem *2;

    ffgcle(fptr, colnum, firstrow, firstvalue, nvalue, 1, 1, nulval,
           array, cdummy, anynul, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgcvm(fitsfile *fptr,   /* I - FITS file pointer                       */
           int  colnum,      /* I - number of column to read (1 = 1st col)  */
           long  firstrow,   /* I - first row to read (1 = 1st row)         */
           long  firstelem,  /* I - first vector element to read (1 = 1st)  */
           long  nelem,      /* I - number of values to read                */
           double nulval,    /* I - value for null pixels                   */
           double *array,    /* O - array of values that are read           */
           int  *anynul,     /* O - set to 1 if any values are null; else 0 */
           int  *status)     /* IO - error status                           */
/*
  Read an array of values from a column in the current FITS HDU. Automatic
  datatype conversion will be performed if the datatype of the column does not
  match the datatype of the array parameter. The output values will be scaled 
  by the FITS TSCALn and TZEROn values if these values have been defined.
  Any undefined pixels will be set equal to the value of 'nulval' unless
  nulval = 0 in which case no checks for undefined pixels will be made.

  TSCAL and ZERO should not be used with complex values. 
*/
{
    long firstvalue, nvalue;
    char *cdummy;

    /* a complex double value is interpreted as a pair of double values,   */
    /* thus need to multiply the first element and number of elements by 2 */

    firstvalue = (firstelem - 1) * 2 + 1;
    nvalue = nelem *2;

    ffgcld(fptr, colnum, firstrow, firstvalue, nvalue, 1, 1, nulval,
           array, cdummy, anynul, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgcfs( fitsfile *fptr,   /* I - FITS file pointer                       */
            int  colnum,      /* I - number of column to write (1 = 1st col) */
            long  firstrow,   /* I - first row to write (1 = 1st row)        */
            long  firstelem,  /* I - first vector element to write (1 = 1st) */
            long  nelem,      /* I - number of strings to write              */
            char **array,     /* O - array of values that are read           */
            char *nularray,   /* O - array of flags = 1 if nultyp = 2        */
            int  *anynul,     /* O - set to 1 if any values are null; else 0 */
            int  *status)     /* IO - error status                           */
/*
  Read an array of string values from a column in the current FITS HDU.
  Nularray will be set = 1 if the corresponding array pixel is undefined, 
  otherwise nularray will = 0.
*/
{
    char dummy[2];

    ffgcls(fptr, colnum, firstrow, firstelem, nelem, 2, dummy,
           array, nularray, anynul, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgcfb(fitsfile *fptr,   /* I - FITS file pointer                       */
           int  colnum,      /* I - number of column to read (1 = 1st col)  */
           long  firstrow,   /* I - first row to read (1 = 1st row)         */
           long  firstelem,  /* I - first vector element to read (1 = 1st)  */
           long  nelem,      /* I - number of values to read                */
           unsigned char *array, /* O - array of values that are read       */
           char *nularray,   /* O - array of flags: 1 if null pixel; else 0 */
           int  *anynul,     /* O - set to 1 if any values are null; else 0 */
           int  *status)     /* IO - error status                           */
/*
  Read an array of values from a column in the current FITS HDU. Automatic
  datatype conversion will be performed if the datatype of the column does not
  match the datatype of the array parameter. The output values will be scaled 
  by the FITS TSCALn and TZEROn values if these values have been defined.
  Nularray will be set = 1 if the corresponding array pixel is undefined, 
  otherwise nularray will = 0.
*/
{
    unsigned char dummy;

    ffgclb(fptr, colnum, firstrow, firstelem, nelem, 1, 2, dummy,
           array, nularray, anynul, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgcfi(fitsfile *fptr,   /* I - FITS file pointer                       */
           int  colnum,      /* I - number of column to read (1 = 1st col)  */
           long  firstrow,   /* I - first row to read (1 = 1st row)         */
           long  firstelem,  /* I - first vector element to read (1 = 1st)  */
           long  nelem,      /* I - number of values to read                */
           short *array,     /* O - array of values that are read           */
           char *nularray,   /* O - array of flags: 1 if null pixel; else 0 */
           int  *anynul,     /* O - set to 1 if any values are null; else 0 */
           int  *status)     /* IO - error status                           */
/*
  Read an array of values from a column in the current FITS HDU. Automatic
  datatype conversion will be performed if the datatype of the column does not
  match the datatype of the array parameter. The output values will be scaled 
  by the FITS TSCALn and TZEROn values if these values have been defined.
  Nularray will be set = 1 if the corresponding array pixel is undefined, 
  otherwise nularray will = 0.
*/
{
    short dummy;

    ffgcli(fptr, colnum, firstrow, firstelem, nelem, 1, 2, dummy,
           array, nularray, anynul, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgcfj(fitsfile *fptr,   /* I - FITS file pointer                       */
           int  colnum,      /* I - number of column to read (1 = 1st col)  */
           long  firstrow,   /* I - first row to read (1 = 1st row)         */
           long  firstelem,  /* I - first vector element to read (1 = 1st)  */
           long  nelem,      /* I - number of values to read                */
           long  *array,     /* O - array of values that are read           */
           char *nularray,   /* O - array of flags: 1 if null pixel; else 0 */
           int  *anynul,     /* O - set to 1 if any values are null; else 0 */
           int  *status)     /* IO - error status                           */
/*
  Read an array of values from a column in the current FITS HDU. Automatic
  datatype conversion will be performed if the datatype of the column does not
  match the datatype of the array parameter. The output values will be scaled 
  by the FITS TSCALn and TZEROn values if these values have been defined.
  Nularray will be set = 1 if the corresponding array pixel is undefined, 
  otherwise nularray will = 0.
*/
{
    long dummy;

    ffgclj(fptr, colnum, firstrow, firstelem, nelem, 1, 2, dummy,
           array, nularray, anynul, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgcfk(fitsfile *fptr,   /* I - FITS file pointer                       */
           int  colnum,      /* I - number of column to read (1 = 1st col)  */
           long  firstrow,   /* I - first row to read (1 = 1st row)         */
           long  firstelem,  /* I - first vector element to read (1 = 1st)  */
           long  nelem,      /* I - number of values to read                */
           int   *array,     /* O - array of values that are read           */
           char *nularray,   /* O - array of flags: 1 if null pixel; else 0 */
           int  *anynul,     /* O - set to 1 if any values are null; else 0 */
           int  *status)     /* IO - error status                           */
/*
  Read an array of values from a column in the current FITS HDU. Automatic
  datatype conversion will be performed if the datatype of the column does not
  match the datatype of the array parameter. The output values will be scaled 
  by the FITS TSCALn and TZEROn values if these values have been defined.
  Nularray will be set = 1 if the corresponding array pixel is undefined, 
  otherwise nularray will = 0.
*/
{
    int dummy;

    ffgclk(fptr, colnum, firstrow, firstelem, nelem, 1, 2, dummy,
           array, nularray, anynul, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgcfe(fitsfile *fptr,   /* I - FITS file pointer                       */
           int  colnum,      /* I - number of column to read (1 = 1st col)  */
           long  firstrow,   /* I - first row to read (1 = 1st row)         */
           long  firstelem,  /* I - first vector element to read (1 = 1st)  */
           long  nelem,      /* I - number of values to read                */
           float *array,     /* O - array of values that are read           */
           char *nularray,   /* O - array of flags: 1 if null pixel; else 0 */
           int  *anynul,     /* O - set to 1 if any values are null; else 0 */
           int  *status)     /* IO - error status                           */
/*
  Read an array of values from a column in the current FITS HDU. Automatic
  datatype conversion will be performed if the datatype of the column does not
  match the datatype of the array parameter. The output values will be scaled 
  by the FITS TSCALn and TZEROn values if these values have been defined.
  Nularray will be set = 1 if the corresponding array pixel is undefined, 
  otherwise nularray will = 0.
*/
{
    float dummy;

    ffgcle(fptr, colnum, firstrow, firstelem, nelem, 1, 2, dummy,
           array, nularray, anynul, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgcfd(fitsfile *fptr,   /* I - FITS file pointer                       */
           int  colnum,      /* I - number of column to read (1 = 1st col)  */
           long  firstrow,   /* I - first row to read (1 = 1st row)         */
           long  firstelem,  /* I - first vector element to read (1 = 1st)  */
           long  nelem,      /* I - number of values to read                */
           double *array,    /* O - array of values that are read           */
           char *nularray,   /* O - array of flags: 1 if null pixel; else 0 */
           int  *anynul,     /* O - set to 1 if any values are null; else 0 */
           int  *status)     /* IO - error status                           */
/*
  Read an array of values from a column in the current FITS HDU. Automatic
  datatype conversion will be performed if the datatype of the column does not
  match the datatype of the array parameter. The output values will be scaled 
  by the FITS TSCALn and TZEROn values if these values have been defined.
  Nularray will be set = 1 if the corresponding array pixel is undefined, 
  otherwise nularray will = 0.
*/
{
    double dummy;

    ffgcld(fptr, colnum, firstrow, firstelem, nelem, 1, 2, dummy,
           array, nularray, anynul, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgcfc(fitsfile *fptr,   /* I - FITS file pointer                       */
           int  colnum,      /* I - number of column to read (1 = 1st col)  */
           long  firstrow,   /* I - first row to read (1 = 1st row)         */
           long  firstelem,  /* I - first vector element to read (1 = 1st)  */
           long  nelem,      /* I - number of values to read                */
           float *array,     /* O - array of values that are read           */
           char *nularray,   /* O - array of flags: 1 if null pixel; else 0 */
           int  *anynul,     /* O - set to 1 if any values are null; else 0 */
           int  *status)     /* IO - error status                           */
/*
  Read an array of values from a column in the current FITS HDU. Automatic
  datatype conversion will be performed if the datatype of the column does not
  match the datatype of the array parameter. The output values will be scaled 
  by the FITS TSCALn and TZEROn values if these values have been defined.
  Nularray will be set = 1 if the corresponding array pixel is undefined, 
  otherwise nularray will = 0.

  TSCAL and ZERO should not be used with complex values. 
*/
{
    long firstvalue, nvalue;
    float dummy;

    /* a complex value is interpreted as a pair of float values, thus */
    /* need to multiply the first element and number of elements by 2 */

    firstvalue = (firstelem - 1) * 2 + 1;
    nvalue = nelem *2;

    ffgcle(fptr, colnum, firstrow, firstvalue, nvalue, 1, 2, dummy,
           array, nularray, anynul, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgcfm(fitsfile *fptr,   /* I - FITS file pointer                       */
           int  colnum,      /* I - number of column to read (1 = 1st col)  */
           long  firstrow,   /* I - first row to read (1 = 1st row)         */
           long  firstelem,  /* I - first vector element to read (1 = 1st)  */
           long  nelem,      /* I - number of values to read                */
           double *array,    /* O - array of values that are read           */
           char *nularray,   /* O - array of flags: 1 if null pixel; else 0 */
           int  *anynul,     /* O - set to 1 if any values are null; else 0 */
           int  *status)     /* IO - error status                           */
/*
  Read an array of values from a column in the current FITS HDU. Automatic
  datatype conversion will be performed if the datatype of the column does not
  match the datatype of the array parameter. The output values will be scaled 
  by the FITS TSCALn and TZEROn values if these values have been defined.
  Nularray will be set = 1 if the corresponding array pixel is undefined, 
  otherwise nularray will = 0.

  TSCAL and ZERO should not be used with complex values. 
*/
{
    long firstvalue, nvalue;
    double dummy;

    /* a complex double value is interpreted as a pair of double values,   */
    /* thus need to multiply the first element and number of elements by 2 */

    firstvalue = (firstelem - 1) * 2 + 1;
    nvalue = nelem *2;

    ffgcld(fptr, colnum, firstrow, firstvalue, nvalue, 1, 2, dummy,
           array, nularray, anynul, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgcls( fitsfile *fptr,   /* I - FITS file pointer                       */
            int  colnum,      /* I - number of column to write (1 = 1st col) */
            long  firstrow,   /* I - first row to write (1 = 1st row)        */
            long  firstelem,  /* I - first vector element to write (1 = 1st) */
            long  nelem,      /* I - number of strings to write              */
            int   nultyp,     /* I - null value handling code:               */
                              /*     1: set undefined pixels = nulval        */
                              /*     2: set nularray=1 for undefined pixels  */
            char  *nulval,    /* I - value for null pixels if nultyp = 1     */
            char **array,     /* O - array of values that are read           */
            char *nularray,   /* O - array of flags = 1 if nultyp = 2        */
            int  *anynul,     /* O - set to 1 if any values are null; else 0 */
            int  *status)     /* IO - error status                           */
/*
  Read an array of string values from a column in the current FITS HDU.
*/
{
    size_t nullen; 
    int tcode, maxelem, hdutype, nulcheck, ii;
    long twidth, offset, incre, repeat, rowlen, rownum, elemnum;
    long tnull, startpos, readptr, remain, next;
    double scale, zero;
    char tform[20];
    char message[FLEN_ERRMSG];
    char snull[20];   /*  the FITS null value  */
    tcolumn *colptr;

    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    *anynul = 0;
    if (nultyp == 2)
    {                                 /* initialize nullarray */
        for (ii = 0; ii < nelem; ii++)
            nularray[ii] = 0;
    }

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
      /* only read a single string; ignore value of firstelem */

      if (ffgcpr( fptr, colnum, firstrow, 1, 1, 0, &scale, &zero,
        tform, &twidth, &tcode, &maxelem, &startpos,  &elemnum, &incre,
        &repeat, &rowlen, &hdutype, &tnull, snull, status) > 0)
        return(*status);

      remain = 1;
      twidth = repeat;  
    }
    else if (tcode == TSTRING)
    {
      if (ffgcpr( fptr, colnum, firstrow, firstelem, nelem, 0, &scale, &zero,
        tform, &twidth, &tcode, &maxelem, &startpos,  &elemnum, &incre,
        &repeat, &rowlen, &hdutype, &tnull, snull, status) > 0)
        return(*status);

      remain = nelem;
    }
    else
        return(*status = NOT_ASCII_COL);

    nullen = strlen(snull);   /* length of the undefined pixel string */
    if (nullen == 0)
        nullen = 1;
 
    /*------------------------------------------------------------------*/
    /*  Decide whether to check for null values in the input FITS file: */
    /*------------------------------------------------------------------*/
    nulcheck = nultyp; /* by default check for null values in the FITS file */

    if (nultyp == 1 && nulval[0] == 0)
       nulcheck = 0;    /* calling routine does not want to check for nulls */

    else if (snull[0] == ASCII_NULL_UNDEFINED)
         nulcheck = 0;   /* null value string in ASCII table not defined */

    /*---------------------------------------------------------------------*/
    /*  Now read the strings one at a time from the FITS column.            */
    /*---------------------------------------------------------------------*/
    next = 0;                 /* next element in array to be read  */
    rownum = 0;               /* row number, relative to firstrow     */

    while (remain)
    {
      readptr = startpos + (rownum * rowlen) + (elemnum * incre);
 
      ffmbyt(fptr, readptr, REPORT_EOF, status);  /* move to read position */
      ffgbyt(fptr, twidth, array[next], status);

      array[next][twidth] = 0;
      for (ii = twidth - 1; ii > 0; ii--)
      {
        if (array[next][ii] == ' ')
          array[next][ii] = 0;  /* strip trailing blanks; leave 1 if all blanks */
        else
          break;
      }

      /* check if null value is defined, and if the   */
      /* column string is identical to the null string */

      if (nulcheck && !strncmp(snull, array[next], nullen) )
      {
        *anynul = 1;   /* this is a null value */
        if (nultyp == 1)
           strcpy(array[next], nulval);
        else
           nularray[next] = 1;
      }
    
      if (*status > 0)  /* test for error during previous read operation */
      {
         sprintf(message,
           "Error reading element %ld of output array of strings (ffgcls).",
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
int ffgcfl( fitsfile *fptr,   /* I - FITS file pointer                       */
            int  colnum,      /* I - number of column to read (1 = 1st col)  */
            long  firstrow,   /* I - first row to read (1 = 1st row)         */
            long  firstelem,  /* I - first vector element to read (1 = 1st)  */
            long  nelem,      /* I - number of values to read                */
            char *array,      /* O - array of values                         */
            char *nularray,   /* O - array of flags = 1 if nultyp = 2        */
            int  *anynul,     /* O - set to 1 if any values are null; else 0 */
            int  *status)     /* IO - error status                           */
/*
  Read an array of logical values from a column in the current FITS HDU.
*/
{
    int tcode, maxelem, hdutype, ii, nulcheck;
    long twidth, offset, incre, repeat, rowlen, rownum, elemnum;
    long tnull, startpos, readptr, remain, next, ntodo;
    double scale, zero;
    char tform[20];
    char message[FLEN_ERRMSG];
    char snull[20];   /*  the FITS null value  */
    unsigned char buffer[DBUFFSIZE];

    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    if (*anynul == 127 && *nularray == 127)
        nulcheck = 0;      /* do not test for null values */

    else
    {
        nulcheck = 1;
        *anynul = 0;
                 
        for (ii = 0; ii < nelem; ii++)
          nularray[ii] = 0;                 /* initialize nullarray */
    }

    /*---------------------------------------------------*/
    /*  Check input and get parameters about the column: */
    /*---------------------------------------------------*/
    if (ffgcpr( fptr, colnum, firstrow, firstelem, nelem, 0, &scale, &zero,
        tform, &twidth, &tcode, &maxelem, &startpos,  &elemnum, &incre,
        &repeat, &rowlen, &hdutype, &tnull, snull, status) > 0)
        return(*status);

    if (tcode != TLOGICAL)   
        return(*status = NOT_LOGICAL_COL);
 
    /*---------------------------------------------------------------------*/
    /*  Now read the logical values from the FITS column.                  */
    /*---------------------------------------------------------------------*/

    remain = nelem;           /* remaining number of values to read */
    next = 0;                 /* next element in array to be read   */
    rownum = 0;               /* row number, relative to firstrow   */
    ntodo = remain;           /* max number of elements to read at one time */

    while (ntodo)
    {
      /*
         limit the number of pixels to read at one time to the number that
         remain in the current vector.    
      */
      ntodo = minvalue(ntodo, maxelem);      
      ntodo = minvalue(ntodo, (repeat - elemnum));

      readptr = startpos + (rownum * rowlen) + (elemnum * incre);

      ffmbyt(fptr, readptr, REPORT_EOF, status);  /* move to read position */

      ffgi1b(fptr, ntodo, incre, buffer, status);

      for (ii = 0; ii < ntodo; ii++, next++) /* convert from T or F to 1 or 0 */
      {
        if (buffer[ii] == 'T')
          array[next] = 1;
        else if (buffer[ii] =='F' || nulcheck == 0)
          array[next] = 0;
        else
        {
          nularray[next] = 1;    /* this is an undefined pixel value */
          *anynul = 1;
        }
      }

      if (*status > 0)  /* test for error during previous read operation */
      {
        sprintf(message,
          "Error reading elements %ld thruough %ld of logical array (ffgcl).",
           next+1, next + ntodo);
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

        if (elemnum == repeat)  /* completed a row; start on later row */
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
int ffgclb( fitsfile *fptr,   /* I - FITS file pointer                       */
            int  colnum,      /* I - number of column to read (1 = 1st col)  */
            long  firstrow,   /* I - first row to read (1 = 1st row)         */
            long  firstelem,  /* I - first vector element to read (1 = 1st)  */
            long  nelem,      /* I - number of values to read                */
            long  elemincre,  /* I - pixel increment; e.g., 2 = every other  */
            int   nultyp,     /* I - null value handling code:               */
                              /*     1: set undefined pixels = nulval        */
                              /*     2: set nularray=1 for undefined pixels  */
            unsigned char nulval, /* I - value for null pixels if nultyp = 1 */
            unsigned char *array, /* O - array of values that are read       */
            char *nularray,   /* O - array of flags = 1 if nultyp = 2        */
            int  *anynul,     /* O - set to 1 if any values are null; else 0 */
            int  *status)     /* IO - error status                           */
/*
  Read an array of values from a column in the current FITS HDU.
  The column number may refer to a real column in an ASCII or binary table, 
  or it may refer be a virtual column in a 1 or more grouped FITS primary
  array or image extension.  FITSIO treats a primary array as a binary table
  with 2 vector columns: the first column contains the group parameters (often
  with length = 0) and the second column contains the array of image pixels.
  Each row of the table represents a group in the case of multigroup FITS
  images.

  The output array of values will be converted from the datatype of the column 
  and will be scaled by the FITS TSCALn and TZEROn values if necessary.
*/
{
    double scale, zero, dblvalue;
    int tcode, maxelem, hdutype, nullen;
    long twidth, incre, repeat, rowlen, rownum, elemnum, remain, next, ntodo;
    long ii, rowincre, tnull;
    int convert, nulcheck;
    long startpos, readptr;
    char tform[20], cform[20];
    unsigned char cstring[50];
    char message[81];
    char *cptr;

    char snull[20];   /*  the FITS null value if reading from ASCII table  */

    double cbuff[DBUFFSIZE / sizeof(double)]; /* align cbuff on word boundary */
    void *buffer;

    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    buffer = cbuff;

    *anynul = 0;
    if (nultyp == 2)
    {                                 /* initialize nullarray */
        for (ii = 0; ii < nelem; ii++)
            nularray[ii] = 0;
    }

    /*---------------------------------------------------*/
    /*  Check input and get parameters about the column: */
    /*---------------------------------------------------*/
    if ( ffgcpr( fptr, colnum, firstrow, firstelem, nelem, 0, &scale, &zero,
         tform, &twidth, &tcode, &maxelem, &startpos, &elemnum, &incre,
         &repeat, &rowlen, &hdutype, &tnull, snull, status) > 0 )
         return(*status);

    incre *= elemincre;   /* multiply incre to just get every nth pixel */

    if (tcode == TSTRING)    /* setup for ASCII tables */
    {
        nullen = strlen(snull);   /* length of the undefined pixel string */
        cform[0] = '%';           /* construct read format string */
        sprintf(&cform[1], "%ldlf", twidth); 
    }

    /*------------------------------------------------------------------*/
    /*  Decide whether to check for null values in the input FITS file: */
    /*------------------------------------------------------------------*/
    nulcheck = nultyp; /* by default, check for null values in the FITS file */

    if (nultyp == 1 && nulval == 0)
       nulcheck = 0;    /* calling routine does not want to check for nulls */

    else if (tcode%10 == 1 &&        /* if reading an integer column, and  */ 
            tnull == NULL_UNDEFINED) /* if a null value is not defined,    */
            nulcheck = 0;            /* then do not check for null values. */

    else if (tcode == TSTRING && snull[0] == ASCII_NULL_UNDEFINED)
         nulcheck = 0;


    /*----------------------------------------------------------------------*/
    /*  If FITS column and output data array have same datatype, then we do */
    /*  not need to use a temporary buffer to store intermediate datatype.  */
    /*----------------------------------------------------------------------*/
    if (tcode == TBYTE) /* Special Case:                        */
    {                             /* no type convertion required, so read */
        maxelem = nelem;          /* data directly into output buffer.    */

        if (nulcheck == 0 && scale == 1. && zero == 0.)
            convert = 0;  /* no need to scale data or find nulls */
        else
            convert = 1;
    }

    /*---------------------------------------------------------------------*/
    /*  Now read the pixels from the FITS column. If the column does not   */
    /*  have the same datatype as the output array, then we have to read   */
    /*  the raw values into a temporary buffer (of limited size).  In      */
    /*  the case of a vector colum read only 1 vector of values at a time  */
    /*  then skip to the next row if more values need to be read.          */
    /*  After reading the raw values, then call the fffXXYY routine to (1) */
    /*  test for undefined values, (2) convert the datatype if necessary,  */
    /*  and (3) scale the values by the FITS TSCALn and TZEROn linear      */
    /*  scaling parameters.                                                */
    /*---------------------------------------------------------------------*/
    remain = nelem;           /* remaining number of values to read */
    next = 0;                 /* next element in array to be read   */
    rownum = 0;               /* row number, relative to firstrow   */
    ntodo = remain;           /* max number of elements to read at one time */

    while (ntodo)
    {
        /* limit the number of pixels to read at one time to the number that
           will fit in the buffer or to the number of pixels that remain in
           the current vector, which ever is smaller.
        */
        ntodo = minvalue(ntodo, maxelem);      
        ntodo = minvalue(ntodo, ((repeat - elemnum - 1)/elemincre +1));

        readptr = startpos + (rownum * rowlen) + (elemnum * incre / elemincre);

        ffmbyt(fptr, readptr, 1, status);  /* move to read position in file */
        switch (tcode) 
        {
            case (TBYTE):
                ffgi1b(fptr, ntodo, incre, &array[next], status);
                if (convert)
                    fffi1i1(&array[next], ntodo, scale, zero, nulcheck, 
                    (unsigned char) tnull, nulval, &nularray[next], anynul, 
                           &array[next], status);
                break;
            case (TSHORT):
                ffgi2b(fptr, ntodo, incre, (short *) buffer, status);
                fffi2i1((short  *) buffer, ntodo, scale, zero, nulcheck, 
                       (short) tnull, nulval, &nularray[next], anynul, 
                       &array[next], status);
                break;
            case (TLONG):
                ffgi4b(fptr, ntodo, incre, (long  *) buffer, status);
                fffi4i1((long  *) buffer, ntodo, scale, zero, nulcheck, 
                       tnull, nulval, &nularray[next], anynul, 
                       &array[next], status);
                break;
            case (TFLOAT):
                ffgr4b(fptr, ntodo, incre, (float  *) buffer, status);
                fffr4i1((float  *) buffer, ntodo, scale, zero, nulcheck, 
                       nulval, &nularray[next], anynul, 
                       &array[next], status);
                break;
            case (TDOUBLE):
                ffgr8b(fptr, ntodo, incre, (double *) buffer, status);
                fffr8i1((double *) buffer, ntodo, scale, zero, nulcheck, 
                          nulval, &nularray[next], anynul, 
                          &array[next], status);
                break;
            case (TSTRING):
                if (ffgi1b(fptr, twidth, 1, cstring, status) <= 0)
                {


                    /* read the ASCII column string successfully */
                    cstring[twidth] = 0;  /* terminate the string */

                    /* check if null value is defined, and if the   */
                    /* column string is identical to the null string */
                    if (nulcheck &&
                       !strncmp(snull, (char *) cstring, nullen) )
                    {
                        *anynul = 1;   /* this is a null value */
                        if (nultyp == 1)
                            array[next] = nulval;
                        else
                            nularray[next] = 1;

                        break;
                    }
    
                    /* C doesn't support a D exponent character, */
                    /* so change it to an E, as in 1.2345E+03    */
                    cptr = strchr((char *) cstring, 'D');
                    if (cptr)
                       *cptr = 'E';

                    /* read the string as a double value */
                    if (sscanf((char *) cstring, cform, &dblvalue) == 1)
                    {
                        /* scale and datatype conversion */
                        fffr8i1(&dblvalue, ntodo, scale, zero, 0,
                          nulval, &nularray[next], anynul,
                          &array[next], status);

                        break;
                    }
                    else if (strspn((char *) cstring, " ") == twidth)
                    {
                        /* field is completely blank; interprete as = 0 */
                        dblvalue = 0.;

                        /* scale and datatype conversion */
                        fffr8i1(&dblvalue, ntodo, scale, zero, 0,
                          nulval, &nularray[next], anynul,
                          &array[next], status);

                        break;
                    }
                }

                sprintf(message, 
                   "Cannot read number from column %d which has format %s.",
                    colnum, tform);
                ffpmsg(message);
                sprintf(message, "Column field = %s.", cstring);
                ffpmsg(message);
                return(*status);

            default:  /*  error trap for invalid column format */
                sprintf(message, 
                   "Cannot read bytes from column %d which has format %s",
                    colnum, tform);
                ffpmsg(message);
                if (hdutype == ASCII_TBL)
                    return(*status = BAD_ATABLE_FORMAT);
                else
                    return(*status = BAD_BTABLE_FORMAT);

        } /* End of switch block */

        /*-------------------------*/
        /*  Check for fatal error  */
        /*-------------------------*/
        if (*status > 0)  /* test for error during previous read operation */
        {
         sprintf(message,
          "Error reading elements %ld thru %ld of input data array (ffgclb).",
              next+1, next+ntodo);
         return(*status);
        }

        /*--------------------------------------------*/
        /*  increment the counters for the next loop  */
        /*--------------------------------------------*/
        remain -= ntodo;
        if (remain)
        {
            next += ntodo;
            elemnum = elemnum + (ntodo * elemincre);

            if (elemnum >= repeat)  /* completed a row; start on later row */
            {
                rowincre = elemnum / repeat;
                rownum += rowincre;
                elemnum = elemnum - (rowincre * repeat);
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
        "Numerical overflow during type conversion while reading FITS data.");
        *status = NUM_OVERFLOW;
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgcli( fitsfile *fptr,   /* I - FITS file pointer                       */
            int  colnum,      /* I - number of column to read (1 = 1st col)  */
            long  firstrow,   /* I - first row to read (1 = 1st row)         */
            long  firstelem,  /* I - first vector element to read (1 = 1st)  */
            long  nelem,      /* I - number of values to read                */
            long  elemincre,  /* I - pixel increment; e.g., 2 = every other  */
            int   nultyp,     /* I - null value handling code:               */
                              /*     1: set undefined pixels = nulval        */
                              /*     2: set nularray=1 for undefined pixels  */
            short nulval,     /* I - value for null pixels if nultyp = 1     */
            short *array,     /* O - array of values that are read           */
            char *nularray,   /* O - array of flags = 1 if nultyp = 2        */
            int  *anynul,     /* O - set to 1 if any values are null; else 0 */
            int  *status)     /* IO - error status                           */
/*
  Read an array of values from a column in the current FITS HDU.
  The column number may refer to a real column in an ASCII or binary table, 
  or it may refer be a virtual column in a 1 or more grouped FITS primary
  array or image extension.  FITSIO treats a primary array as a binary table
  with 2 vector columns: the first column contains the group parameters (often
  with length = 0) and the second column contains the array of image pixels.
  Each row of the table represents a group in the case of multigroup FITS
  images.

  The output array of values will be converted from the datatype of the column 
  and will be scaled by the FITS TSCALn and TZEROn values if necessary.
*/
{
    double scale, zero, dblvalue;
    int tcode, maxelem, hdutype, nullen;
    long twidth, incre, repeat, rowlen, rownum, elemnum, remain, next, ntodo;
    long ii, rowincre, tnull;
    int convert, nulcheck;
    long startpos, readptr;
    char tform[20], cform[20];
    unsigned char cstring[50];
    char message[81];
    char *cptr;

    char snull[20];   /*  the FITS null value if reading from ASCII table  */

    double cbuff[DBUFFSIZE / sizeof(double)]; /* align cbuff on word boundary */
    void *buffer;

    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    buffer = cbuff;

    *anynul = 0;
    if (nultyp == 2)
    {                                 /* initialize nullarray */
        for (ii = 0; ii < nelem; ii++)
            nularray[ii] = 0;
    }

    /*---------------------------------------------------*/
    /*  Check input and get parameters about the column: */
    /*---------------------------------------------------*/
    if ( ffgcpr( fptr, colnum, firstrow, firstelem, nelem, 0, &scale, &zero,
         tform, &twidth, &tcode, &maxelem, &startpos, &elemnum, &incre,
         &repeat, &rowlen, &hdutype, &tnull, snull, status) > 0 )
         return(*status);

    incre *= elemincre;   /* multiply incre to just get every nth pixel */

    if (tcode == TSTRING)    /* setup for ASCII tables */
    {
        nullen = strlen(snull);   /* length of the undefined pixel string */
        cform[0] = '%';           /* construct read format string */
        sprintf(&cform[1], "%ldlf", twidth); 
    }

    /*------------------------------------------------------------------*/
    /*  Decide whether to check for null values in the input FITS file: */
    /*------------------------------------------------------------------*/
    nulcheck = nultyp; /* by default check for null values in the FITS file */

    if (nultyp == 1 && nulval == 0)
       nulcheck = 0;    /* calling routine does not want to check for nulls */

    else if (tcode%10 == 1 &&        /* if reading an integer column, and  */ 
            tnull == NULL_UNDEFINED) /* if a null value is not defined,    */
            nulcheck = 0;            /* then do not check for null values. */

    else if (tcode == TSTRING && snull[0] == ASCII_NULL_UNDEFINED)
         nulcheck = 0;

    /*----------------------------------------------------------------------*/
    /*  If FITS column and output data array have same datatype, then we do */
    /*  not need to use a temporary buffer to store intermediate datatype.  */
    /*----------------------------------------------------------------------*/
    if (tcode == TSHORT) /* Special Case:                        */
    {                             /* no type convertion required, so read */
        maxelem = nelem;          /* data directly into output buffer.    */

        if (nulcheck == 0 && scale == 1. && zero == 0.)
            convert = 0;  /* no need to scale data or find nulls */
        else
            convert = 1;
    }

    /*---------------------------------------------------------------------*/
    /*  Now read the pixels from the FITS column. If the column does not   */
    /*  have the same datatype as the output array, then we have to read   */
    /*  the raw values into a temporary buffer (of limited size).  In      */
    /*  the case of a vector colum read only 1 vector of values at a time  */
    /*  then skip to the next row if more values need to be read.          */
    /*  After reading the raw values, then call the fffXXYY routine to (1) */
    /*  test for undefined values, (2) convert the datatype if necessary,  */
    /*  and (3) scale the values by the FITS TSCALn and TZEROn linear      */
    /*  scaling parameters.                                                */
    /*---------------------------------------------------------------------*/
    remain = nelem;           /* remaining number of values to read */
    next = 0;                 /* next element in array to be read   */
    rownum = 0;               /* row number, relative to firstrow   */
    ntodo = remain;           /* max number of elements to read at one time */

    while (ntodo)
    {
        /* limit the number of pixels to read at one time to the number that
           will fit in the buffer or to the number of pixels that remain in
           the current vector, which ever is smaller.
        */
        ntodo = minvalue(ntodo, maxelem);      
        ntodo = minvalue(ntodo, ((repeat - elemnum - 1)/elemincre +1));

        readptr = startpos + (rownum * rowlen) + (elemnum * incre / elemincre);

        ffmbyt(fptr, readptr, 1, status);  /* move to read position in file */

        switch (tcode) 
        {
            case (TSHORT):
                ffgi2b(fptr, ntodo, incre, &array[next], status);
                if (convert)
                    fffi2i2(&array[next], ntodo, scale, zero, nulcheck, 
                           (short) tnull, nulval, &nularray[next], anynul, 
                           &array[next], status);
                break;
            case (TBYTE):
                ffgi1b(fptr, ntodo, incre, (unsigned char *) buffer, status);
                fffi1i2((unsigned char *) buffer, ntodo, scale, zero, nulcheck, 
                    (unsigned char) tnull, nulval, &nularray[next], anynul, 
                    &array[next], status);
                break;
            case (TLONG):
                ffgi4b(fptr, ntodo, incre, (long  *) buffer, status);
                fffi4i2((long  *) buffer, ntodo, scale, zero, nulcheck, 
                       tnull, nulval, &nularray[next], anynul, 
                       &array[next], status);
                break;
            case (TFLOAT):
                ffgr4b(fptr, ntodo, incre, (float  *) buffer, status);
                fffr4i2((float  *) buffer, ntodo, scale, zero, nulcheck, 
                       nulval, &nularray[next], anynul, 
                       &array[next], status);
                break;
            case (TDOUBLE):
                ffgr8b(fptr, ntodo, incre, (double *) buffer, status);
                fffr8i2((double *) buffer, ntodo, scale, zero, nulcheck, 
                          nulval, &nularray[next], anynul, 
                          &array[next], status);
                break;
            case (TSTRING):
                if (ffgi1b(fptr, twidth, 1, cstring, status) <= 0)
                {
                    /* read the ASCII column string successfully */
                    cstring[twidth] = 0;  /* terminate the string */


                    /* check if null value is defined, and if the   */
                    /* column string is identical to the null string */
                    if (nulcheck && 
                       !strncmp(snull, (char *) cstring, nullen) )
                    {
                        *anynul = 1;   /* this is a null value */
                        if (nultyp == 1)
                            array[next] = nulval;
                        else
                            nularray[next] = 1;

                        break;
                    }
    
                    /* C doesn't support a D exponent character, */
                    /* so change it to an E, as in 1.2345E+03    */
                    cptr = strchr((char *) cstring, 'D');
                    if (cptr)
                       *cptr = 'E';

                    /* read the string as a double value */
                    if (sscanf((char *) cstring, cform, &dblvalue) == 1)
                    {
                        /* scale and datatype conversion */
                        fffr8i2(&dblvalue, ntodo, scale, zero, 0,
                          nulval, &nularray[next], anynul,
                          &array[next], status);

                        break;
                    }
                    else if (strspn((char *) cstring, " ") == twidth)
                    {
                        /* field is completely blank; interprete as = 0 */
                        dblvalue = 0.;

                        /* scale and datatype conversion */
                        fffr8i2(&dblvalue, ntodo, scale, zero, 0,
                          nulval, &nularray[next], anynul,
                          &array[next], status);

                        break;
                    }
                }

                sprintf(message, 
                   "Cannot read number from column %d which has format %s.",
                    colnum, tform);
                ffpmsg(message);
                sprintf(message, "Column field = %s.", cstring);
                ffpmsg(message);
                return(*status);

            default:  /*  error trap for invalid column format */
                sprintf(message, 
                   "Cannot read numbers from column %d which has format %s",
                    colnum, tform);
                ffpmsg(message);
                if (hdutype == ASCII_TBL)
                    return(*status = BAD_ATABLE_FORMAT);
                else
                    return(*status = BAD_BTABLE_FORMAT);

        } /* End of switch block */

        /*-------------------------*/
        /*  Check for fatal error  */
        /*-------------------------*/
        if (*status > 0)  /* test for error during previous read operation */
        {
          sprintf(message,
          "Error reading elements %ld thru %ld of input data array (ffgcli).",
             next+1, next+ntodo);
           return(*status);
        }

        /*--------------------------------------------*/
        /*  increment the counters for the next loop  */
        /*--------------------------------------------*/
        remain -= ntodo;
        if (remain)
        {
            next += ntodo;
            elemnum = elemnum + (ntodo * elemincre);

            if (elemnum >= repeat)  /* completed a row; start on later row */
            {
                rowincre = elemnum / repeat;
                rownum += rowincre;
                elemnum = elemnum - (rowincre * repeat);
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
        "Numerical overflow during type conversion while reading FITS data.");
        *status = NUM_OVERFLOW;
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgclj( fitsfile *fptr,   /* I - FITS file pointer                       */
            int  colnum,      /* I - number of column to read (1 = 1st col)  */
            long  firstrow,   /* I - first row to read (1 = 1st row)         */
            long  firstelem,  /* I - first vector element to read (1 = 1st)  */
            long  nelem,      /* I - number of values to read                */
            long  elemincre,  /* I - pixel increment; e.g., 2 = every other  */
            int   nultyp,     /* I - null value handling code:               */
                              /*     1: set undefined pixels = nulval        */
                              /*     2: set nularray=1 for undefined pixels  */
            long  nulval,     /* I - value for null pixels if nultyp = 1     */
            long  *array,     /* O - array of values that are read           */
            char *nularray,   /* O - array of flags = 1 if nultyp = 2        */
            int  *anynul,     /* O - set to 1 if any values are null; else 0 */
            int  *status)     /* IO - error status                           */
/*
  Read an array of values from a column in the current FITS HDU.
  The column number may refer to a real column in an ASCII or binary table, 
  or it may refer be a virtual column in a 1 or more grouped FITS primary
  array or image extension.  FITSIO treats a primary array as a binary table
  with 2 vector columns: the first column contains the group parameters (often
  with length = 0) and the second column contains the array of image pixels.
  Each row of the table represents a group in the case of multigroup FITS
  images.

  The output array of values will be converted from the datatype of the column 
  and will be scaled by the FITS TSCALn and TZEROn values if necessary.
*/
{
    double scale, zero, dblvalue;
    int tcode, maxelem, hdutype, nullen;
    long twidth, incre, repeat, rowlen, rownum, elemnum, remain, next, ntodo;
    long ii, rowincre, tnull;
    int convert, nulcheck;
    long startpos, readptr;
    char tform[20], cform[20];
    unsigned char cstring[50];
    char message[81];
    char *cptr;

    char snull[20];   /*  the FITS null value if reading from ASCII table  */

    double cbuff[DBUFFSIZE / sizeof(double)]; /* align cbuff on word boundary */
    void *buffer;

    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    buffer = cbuff;

    *anynul = 0;
    if (nultyp == 2)
    {                                 /* initialize nullarray */
        for (ii = 0; ii < nelem; ii++)
            nularray[ii] = 0;
    }

    /*---------------------------------------------------*/
    /*  Check input and get parameters about the column: */
    /*---------------------------------------------------*/
    if ( ffgcpr( fptr, colnum, firstrow, firstelem, nelem, 0, &scale, &zero,
         tform, &twidth, &tcode, &maxelem, &startpos, &elemnum, &incre,
         &repeat, &rowlen, &hdutype, &tnull, snull, status) > 0 )
         return(*status);

    incre *= elemincre;   /* multiply incre to just get every nth pixel */

    if (tcode == TSTRING)    /* setup for ASCII tables */
    {
        nullen = strlen(snull);   /* length of the undefined pixel string */
        cform[0] = '%';           /* construct read format string */
        sprintf(&cform[1], "%ldlf", twidth); 
    }

    /*------------------------------------------------------------------*/
    /*  Decide whether to check for null values in the input FITS file: */
    /*------------------------------------------------------------------*/
    nulcheck = nultyp; /* by default check for null values in the FITS file */

    if (nultyp == 1 && nulval == 0)
       nulcheck = 0;    /* calling routine does not want to check for nulls */

    else if (tcode%10 == 1 &&        /* if reading an integer column, and  */ 
            tnull == NULL_UNDEFINED) /* if a null value is not defined,    */
            nulcheck = 0;            /* then do not check for null values. */

    else if (tcode == TSTRING && snull[0] == ASCII_NULL_UNDEFINED)
         nulcheck = 0;

    /*----------------------------------------------------------------------*/
    /*  If FITS column and output data array have same datatype, then we do */
    /*  not need to use a temporary buffer to store intermediate datatype.  */
    /*----------------------------------------------------------------------*/
    if (tcode == TLONG)  /* Special Case:                        */
    {                             /* no type convertion required, so read */
        maxelem = nelem;          /* data directly into output buffer.    */

        if (nulcheck == 0 && scale == 1. && zero == 0.)
            convert = 0;  /* no need to scale data or find nulls */
        else
            convert = 1;
    }

    /*---------------------------------------------------------------------*/
    /*  Now read the pixels from the FITS column. If the column does not   */
    /*  have the same datatype as the output array, then we have to read   */
    /*  the raw values into a temporary buffer (of limited size).  In      */
    /*  the case of a vector colum read only 1 vector of values at a time  */
    /*  then skip to the next row if more values need to be read.          */
    /*  After reading the raw values, then call the fffXXYY routine to (1) */
    /*  test for undefined values, (2) convert the datatype if necessary,  */
    /*  and (3) scale the values by the FITS TSCALn and TZEROn linear      */
    /*  scaling parameters.                                                */
    /*---------------------------------------------------------------------*/
    remain = nelem;           /* remaining number of values to read */
    next = 0;                 /* next element in array to be read   */
    rownum = 0;               /* row number, relative to firstrow   */
    ntodo = remain;           /* max number of elements to read at one time */

    while (ntodo)
    {
        /* limit the number of pixels to read at one time to the number that
           will fit in the buffer or to the number of pixels that remain in
           the current vector, which ever is smaller.
        */
        ntodo = minvalue(ntodo, maxelem);      
        ntodo = minvalue(ntodo, ((repeat - elemnum - 1)/elemincre +1));

        readptr = startpos + (rownum * rowlen) + (elemnum * incre / elemincre);

        ffmbyt(fptr, readptr, 1, status);  /* move to read position in file */

        switch (tcode) 
        {
            case (TLONG):
                ffgi4b(fptr, ntodo, incre, &array[next], status);
                if (convert)
                    fffi4i4(&array[next], ntodo, scale, zero, nulcheck, 
                           tnull, nulval, &nularray[next], anynul, 
                           &array[next], status);
                break;
            case (TBYTE):
                ffgi1b(fptr, ntodo, incre, (unsigned char *) buffer, status);
                fffi1i4((unsigned char *) buffer, ntodo, scale, zero, nulcheck, 
                     (unsigned char) tnull, nulval, &nularray[next], anynul, 
                     &array[next], status);
                break;
            case (TSHORT):
                ffgi2b(fptr, ntodo, incre, (short  *) buffer, status);
                fffi2i4((short  *) buffer, ntodo, scale, zero, nulcheck, 
                      (short) tnull, nulval, &nularray[next], anynul, 
                      &array[next], status);
                break;
            case (TFLOAT):
                ffgr4b(fptr, ntodo, incre, (float  *) buffer, status);
                fffr4i4((float  *) buffer, ntodo, scale, zero, nulcheck, 
                       nulval, &nularray[next], anynul, 
                       &array[next], status);
                break;
            case (TDOUBLE):
                ffgr8b(fptr, ntodo, incre, (double *) buffer, status);
                fffr8i4((double *) buffer, ntodo, scale, zero, nulcheck, 
                          nulval, &nularray[next], anynul, 
                          &array[next], status);
                break;
            case (TSTRING):
                if (ffgi1b(fptr, twidth, 1, cstring, status) <= 0)
                {
                    /* read the ASCII column string successfully */
                    cstring[twidth] = 0;  /* terminate the string */


                    /* check if null value is defined, and if the   */
                    /* column string is identical to the null string */
                    if (nulcheck && 
                       !strncmp(snull, (char *) cstring, nullen) )
                    {
                        *anynul = 1;   /* this is a null value */
                        if (nultyp == 1)
                            array[next] = nulval;
                        else
                            nularray[next] = 1;

                        break;
                    }
    
                    /* C doesn't support a D exponent character, */
                    /* so change it to an E, as in 1.2345E+03    */
                    cptr = strchr((char *) cstring, 'D');
                    if (cptr)
                       *cptr = 'E';

                    /* read the string as a double value */
                    if (sscanf((char *) cstring, cform, &dblvalue) == 1)
                    {
                        /* scale and datatype conversion */
                        fffr8i4(&dblvalue, ntodo, scale, zero, 0,
                          nulval, &nularray[next], anynul,
                          &array[next], status);
                        break;
                    }
                    else if (strspn((char *) cstring, " ") == twidth)
                    {
                        /* field is completely blank; interprete as = 0 */
                        dblvalue = 0.;

                        /* scale and datatype conversion */
                        fffr8i4(&dblvalue, ntodo, scale, zero, 0,
                          nulval, &nularray[next], anynul,
                          &array[next], status);

                        break;
                    }
                }

                sprintf(message, 
                   "Cannot read number from column %d which has format %s.",
                    colnum, tform);
                ffpmsg(message);
                sprintf(message, "Column field = %s.", cstring);
                ffpmsg(message);
                return(*status);

            default:  /*  error trap for invalid column format */
                sprintf(message, 
                   "Cannot read numbers from column %d which has format %s",
                    colnum, tform);
                ffpmsg(message);
                if (hdutype == ASCII_TBL)
                    return(*status = BAD_ATABLE_FORMAT);
                else
                    return(*status = BAD_BTABLE_FORMAT);

        } /* End of switch block */

        /*-------------------------*/
        /*  Check for fatal error  */
        /*-------------------------*/
        if (*status > 0)  /* test for error during previous read operation */
        {
          sprintf(message,
          "Error reading elements %ld thru %ld of input data array (ffgclj).",
            next+1, next+ntodo);
           return(*status);
        }

        /*--------------------------------------------*/
        /*  increment the counters for the next loop  */
        /*--------------------------------------------*/
        remain -= ntodo;
        if (remain)
        {
            next += ntodo;
            elemnum = elemnum + (ntodo * elemincre);

            if (elemnum >= repeat)  /* completed a row; start on later row */
            {
                rowincre = elemnum / repeat;
                rownum += rowincre;
                elemnum = elemnum - (rowincre * repeat);
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
        "Numerical overflow during type conversion while reading FITS data.");
        *status = NUM_OVERFLOW;
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgclk( fitsfile *fptr,   /* I - FITS file pointer                       */
            int  colnum,      /* I - number of column to read (1 = 1st col)  */
            long  firstrow,   /* I - first row to read (1 = 1st row)         */
            long  firstelem,  /* I - first vector element to read (1 = 1st)  */
            long  nelem,      /* I - number of values to read                */
            long  elemincre,  /* I - pixel increment; e.g., 2 = every other  */
            int   nultyp,     /* I - null value handling code:               */
                              /*     1: set undefined pixels = nulval        */
                              /*     2: set nularray=1 for undefined pixels  */
            int   nulval,     /* I - value for null pixels if nultyp = 1     */
            int  *array,      /* O - array of values that are read           */
            char *nularray,   /* O - array of flags = 1 if nultyp = 2        */
            int  *anynul,     /* O - set to 1 if any values are null; else 0 */
            int  *status)     /* IO - error status                           */
/*
  Read an array of values from a column in the current FITS HDU.
  The column number may refer to a real column in an ASCII or binary table, 
  or it may refer be a virtual column in a 1 or more grouped FITS primary
  array or image extension.  FITSIO treats a primary array as a binary table
  with 2 vector columns: the first column contains the group parameters (often
  with length = 0) and the second column contains the array of image pixels.
  Each row of the table represents a group in the case of multigroup FITS
  images.

  The output array of values will be converted from the datatype of the column 
  and will be scaled by the FITS TSCALn and TZEROn values if necessary.
*/
{
    double scale, zero, dblvalue;
    int tcode, maxelem, hdutype, nullen;
    long twidth, incre, repeat, rowlen, rownum, elemnum, remain, next, ntodo;
    long ii, rowincre, tnull;
    int nulcheck;
    long startpos, readptr;
    char tform[20], cform[20];
    unsigned char cstring[50];
    char message[81];
    char *cptr;

    char snull[20];   /*  the FITS null value if reading from ASCII table  */

    double cbuff[DBUFFSIZE / sizeof(double)]; /* align cbuff on word boundary */
    void *buffer;

    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    /* call the 'short' or 'long' version of this routine, if possible */
    if (sizeof(int) == sizeof(short))
        ffgcli(fptr, colnum, firstrow, firstelem, nelem, elemincre, nultyp,
              (short) nulval, (short *) array, nularray, anynul, status);
    else if (sizeof(int) == sizeof(long))
        ffgclj(fptr, colnum, firstrow, firstelem, nelem, elemincre, nultyp,
              (long) nulval, (long *) array, nularray, anynul, status);

    /*
      This is a special case: sizeof(int) is not equal to sizeof(short) or
      sizeof(long).  This occurs on Alpha OSF systems where short = 2 bytes,
      int = 4 bytes, and long = 8 bytes.
    */

    buffer = cbuff;

    *anynul = 0;
    if (nultyp == 2)
    {                                 /* initialize nullarray */
        for (ii = 0; ii < nelem; ii++)
            nularray[ii] = 0;
    }

    /*---------------------------------------------------*/
    /*  Check input and get parameters about the column: */
    /*---------------------------------------------------*/
    if ( ffgcpr( fptr, colnum, firstrow, firstelem, nelem, 0, &scale, &zero,
         tform, &twidth, &tcode, &maxelem, &startpos, &elemnum, &incre,
         &repeat, &rowlen, &hdutype, &tnull, snull, status) > 0 )
         return(*status);

    incre *= elemincre;   /* multiply incre to just get every nth pixel */

    if (tcode == TSTRING)    /* setup for ASCII tables */
    {
        nullen = strlen(snull);   /* length of the undefined pixel string */
        cform[0] = '%';           /* construct read format string */
        sprintf(&cform[1], "%ldlf", twidth); 
    }

    /*------------------------------------------------------------------*/
    /*  Decide whether to check for null values in the input FITS file: */
    /*------------------------------------------------------------------*/
    nulcheck = nultyp; /* by default check for null values in the FITS file */

    if (nultyp == 1 && nulval == 0)
       nulcheck = 0;    /* calling routine does not want to check for nulls */

    else if (tcode%10 == 1 &&        /* if reading an integer column, and  */ 
            tnull == NULL_UNDEFINED) /* if a null value is not defined,    */
            nulcheck = 0;            /* then do not check for null values. */

    else if (tcode == TSTRING && snull[0] == ASCII_NULL_UNDEFINED)
         nulcheck = 0;

    /*---------------------------------------------------------------------*/
    /*  Now read the pixels from the FITS column. If the column does not   */
    /*  have the same datatype as the output array, then we have to read   */
    /*  the raw values into a temporary buffer (of limited size).  In      */
    /*  the case of a vector colum read only 1 vector of values at a time  */
    /*  then skip to the next row if more values need to be read.          */
    /*  After reading the raw values, then call the fffXXYY routine to (1) */
    /*  test for undefined values, (2) convert the datatype if necessary,  */
    /*  and (3) scale the values by the FITS TSCALn and TZEROn linear      */
    /*  scaling parameters.                                                */
    /*---------------------------------------------------------------------*/
    remain = nelem;           /* remaining number of values to read */
    next = 0;                 /* next element in array to be read   */
    rownum = 0;               /* row number, relative to firstrow   */
    ntodo = remain;           /* max number of elements to read at one time */

    while (ntodo)
    {
        /* limit the number of pixels to read at one time to the number that
           will fit in the buffer or to the number of pixels that remain in
           the current vector, which ever is smaller.
        */
        ntodo = minvalue(ntodo, maxelem);      
        ntodo = minvalue(ntodo, ((repeat - elemnum - 1)/elemincre +1));

        readptr = startpos + (rownum * rowlen) + (elemnum * incre / elemincre);

        ffmbyt(fptr, readptr, 1, status);  /* move to read position in file */

        switch (tcode) 
        {
            case (TLONG):
                ffgi4b(fptr, ntodo, incre, (long  *) buffer, status);
                fffi4int((long  *) buffer, ntodo, scale, zero, nulcheck, 
                           tnull, nulval, &nularray[next], anynul, 
                           &array[next], status);
                break;
            case (TBYTE):
                ffgi1b(fptr, ntodo, incre, (unsigned char *) buffer, status);
                fffi1int((unsigned char *) buffer, ntodo, scale, zero, nulcheck, 
                     (unsigned char) tnull, nulval, &nularray[next], anynul, 
                     &array[next], status);
                break;
            case (TSHORT):
                ffgi2b(fptr, ntodo, incre, (short  *) buffer, status);
                fffi2int((short  *) buffer, ntodo, scale, zero, nulcheck, 
                      (short) tnull, nulval, &nularray[next], anynul, 
                      &array[next], status);
                break;
            case (TFLOAT):
                ffgr4b(fptr, ntodo, incre, (float  *) buffer, status);
                fffr4int((float  *) buffer, ntodo, scale, zero, nulcheck, 
                       nulval, &nularray[next], anynul, 
                       &array[next], status);
                break;
            case (TDOUBLE):
                ffgr8b(fptr, ntodo, incre, (double *) buffer, status);
                fffr8int((double *) buffer, ntodo, scale, zero, nulcheck, 
                          nulval, &nularray[next], anynul, 
                          &array[next], status);
                break;
            case (TSTRING):
                if (ffgi1b(fptr, twidth, 1, cstring, status) <= 0)
                {
                    /* read the ASCII column string successfully */
                    cstring[twidth] = 0;  /* terminate the string */


                    /* check if null value is defined, and if the   */
                    /* column string is identical to the null string */
                    if (nulcheck && 
                       !strncmp(snull, (char *) cstring, nullen) )
                    {
                        *anynul = 1;   /* this is a null value */
                        if (nultyp == 1)
                            array[next] = nulval;
                        else
                            nularray[next] = 1;

                        break;
                    }
    
                    /* C doesn't support a D exponent character, */
                    /* so change it to an E, as in 1.2345E+03    */
                    cptr = strchr((char *) cstring, 'D');
                    if (cptr)
                       *cptr = 'E';

                    /* read the string as a double value */
                    if (sscanf((char *) cstring, cform, &dblvalue) == 1)
                    {
                        /* scale and datatype conversion */
                        fffr8int(&dblvalue, ntodo, scale, zero, 0,
                          nulval, &nularray[next], anynul,
                          &array[next], status);
                        break;
                    }
                    else if (strspn((char *) cstring, " ") == twidth)
                    {
                        /* field is completely blank; interprete as = 0 */
                        dblvalue = 0.;

                        /* scale and datatype conversion */
                        fffr8int(&dblvalue, ntodo, scale, zero, 0,
                          nulval, &nularray[next], anynul,
                          &array[next], status);

                        break;
                    }
                }

                sprintf(message, 
                   "Cannot read number from column %d which has format %s.",
                    colnum, tform);
                ffpmsg(message);
                sprintf(message, "Column field = %s.", cstring);
                ffpmsg(message);
                return(*status);

            default:  /*  error trap for invalid column format */
                sprintf(message, 
                   "Cannot read numbers from column %d which has format %s",
                    colnum, tform);
                ffpmsg(message);
                if (hdutype == ASCII_TBL)
                    return(*status = BAD_ATABLE_FORMAT);
                else
                    return(*status = BAD_BTABLE_FORMAT);

        } /* End of switch block */

        /*-------------------------*/
        /*  Check for fatal error  */
        /*-------------------------*/
        if (*status > 0)  /* test for error during previous read operation */
        {
          sprintf(message,
          "Error reading elements %ld thru %ld of input data array (ffgclk).",
            next+1, next+ntodo);
           return(*status);
        }

        /*--------------------------------------------*/
        /*  increment the counters for the next loop  */
        /*--------------------------------------------*/
        remain -= ntodo;
        if (remain)
        {
            next += ntodo;
            elemnum = elemnum + (ntodo * elemincre);

            if (elemnum >= repeat)  /* completed a row; start on later row */
            {
                rowincre = elemnum / repeat;
                rownum += rowincre;
                elemnum = elemnum - (rowincre * repeat);
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
        "Numerical overflow during type conversion while reading FITS data.");
        *status = NUM_OVERFLOW;
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgcle( fitsfile *fptr,   /* I - FITS file pointer                       */
            int  colnum,      /* I - number of column to read (1 = 1st col)  */
            long  firstrow,   /* I - first row to read (1 = 1st row)         */
            long  firstelem,  /* I - first vector element to read (1 = 1st)  */
            long  nelem,      /* I - number of values to read                */
            long  elemincre,  /* I - pixel increment; e.g., 2 = every other  */
            int   nultyp,     /* I - null value handling code:               */
                              /*     1: set undefined pixels = nulval        */
                              /*     2: set nularray=1 for undefined pixels  */
            float nulval,     /* I - value for null pixels if nultyp = 1     */
            float *array,     /* O - array of values that are read           */
            char *nularray,   /* O - array of flags = 1 if nultyp = 2        */
            int  *anynul,     /* O - set to 1 if any values are null; else 0 */
            int  *status)     /* IO - error status                           */
/*
  Read an array of values from a column in the current FITS HDU.
  The column number may refer to a real column in an ASCII or binary table, 
  or it may refer be a virtual column in a 1 or more grouped FITS primary
  array or image extension.  FITSIO treats a primary array as a binary table
  with 2 vector columns: the first column contains the group parameters (often
  with length = 0) and the second column contains the array of image pixels.
  Each row of the table represents a group in the case of multigroup FITS
  images.

  The output array of values will be converted from the datatype of the column 
  and will be scaled by the FITS TSCALn and TZEROn values if necessary.
*/
{
    double scale, zero, dblvalue;
    int tcode, maxelem, hdutype, nullen;
    long twidth, incre, repeat, rowlen, rownum, elemnum, remain, next, ntodo;
    long ii, rowincre, tnull;
    int convert, nulcheck;
    long startpos, readptr;
    char tform[20], cform[20];
    unsigned char cstring[50];
    char message[81];
    char *cptr;

    char snull[20];   /*  the FITS null value if reading from ASCII table  */

    double cbuff[DBUFFSIZE / sizeof(double)]; /* align cbuff on word boundary */
    void *buffer;

    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    buffer = cbuff;

    *anynul = 0;
    if (nultyp == 2)
    {                                 /* initialize nullarray */
        for (ii = 0; ii < nelem; ii++)
            nularray[ii] = 0;
    }

    /*---------------------------------------------------*/
    /*  Check input and get parameters about the column: */
    /*---------------------------------------------------*/
    if ( ffgcpr( fptr, colnum, firstrow, firstelem, nelem, 0, &scale, &zero,
         tform, &twidth, &tcode, &maxelem, &startpos, &elemnum, &incre,
         &repeat, &rowlen, &hdutype, &tnull, snull, status) > 0 )
         return(*status);

    incre *= elemincre;   /* multiply incre to just get every nth pixel */

    if (tcode == TSTRING)    /* setup for ASCII tables */
    {
        nullen = strlen(snull);   /* length of the undefined pixel string */
        cform[0] = '%';           /* construct read format string */
        sprintf(&cform[1], "%ldlf", twidth); 
    }

    /*------------------------------------------------------------------*/
    /*  Decide whether to check for null values in the input FITS file: */
    /*------------------------------------------------------------------*/
    nulcheck = nultyp; /* by default check for null values in the FITS file */

    if (nultyp == 1 && nulval == 0)
       nulcheck = 0;    /* calling routine does not want to check for nulls */

    else if (tcode%10 == 1 &&        /* if reading an integer column, and  */ 
            tnull == NULL_UNDEFINED) /* if a null value is not defined,    */
            nulcheck = 0;            /* then do not check for null values. */

    else if (tcode == TSTRING && snull[0] == ASCII_NULL_UNDEFINED)
         nulcheck = 0;

    /*----------------------------------------------------------------------*/
    /*  If FITS column and output data array have same datatype, then we do */
    /*  not need to use a temporary buffer to store intermediate datatype.  */
    /*----------------------------------------------------------------------*/
    if (tcode == TFLOAT) /* Special Case:                        */
    {                             /* no type convertion required, so read */
        maxelem = nelem;          /* data directly into output buffer.    */

        if (nulcheck == 0 && scale == 1. && zero == 0.)
            convert = 0;  /* no need to scale data or find nulls */
        else
            convert = 1;
    }

    /*---------------------------------------------------------------------*/
    /*  Now read the pixels from the FITS column. If the column does not   */
    /*  have the same datatype as the output array, then we have to read   */
    /*  the raw values into a temporary buffer (of limited size).  In      */
    /*  the case of a vector colum read only 1 vector of values at a time  */
    /*  then skip to the next row if more values need to be read.          */
    /*  After reading the raw values, then call the fffXXYY routine to (1) */
    /*  test for undefined values, (2) convert the datatype if necessary,  */
    /*  and (3) scale the values by the FITS TSCALn and TZEROn linear      */
    /*  scaling parameters.                                                */
    /*---------------------------------------------------------------------*/
    remain = nelem;           /* remaining number of values to read */
    next = 0;                 /* next element in array to be read   */
    rownum = 0;               /* row number, relative to firstrow   */
    ntodo = remain;           /* max number of elements to read at one time */

    while (ntodo)
    {
        /* limit the number of pixels to read at one time to the number that
           will fit in the buffer or to the number of pixels that remain in
           the current vector, which ever is smaller.
        */
        ntodo = minvalue(ntodo, maxelem);      
        ntodo = minvalue(ntodo, ((repeat - elemnum - 1)/elemincre +1));

        readptr = startpos + (rownum * rowlen) + (elemnum * incre / elemincre);

        ffmbyt(fptr, readptr, 1, status);  /* move to read position in file */

        switch (tcode) 
        {
            case (TFLOAT):
                ffgr4b(fptr, ntodo, incre, &array[next], status);
                if (convert)
                    fffr4r4(&array[next], ntodo, scale, zero, nulcheck, 
                           nulval, &nularray[next], anynul, 
                           &array[next], status);
                break;
            case (TBYTE):
                ffgi1b(fptr, ntodo, incre, (unsigned char *) buffer, status);
                fffi1r4((unsigned char *) buffer, ntodo, scale, zero, nulcheck, 
                    (unsigned char) tnull, nulval, &nularray[next], anynul, 
                     &array[next], status);
                break;
            case (TSHORT):
                ffgi2b(fptr, ntodo, incre, (short  *) buffer, status);
                fffi2r4((short  *) buffer, ntodo, scale, zero, nulcheck, 
                       (short) tnull, nulval, &nularray[next], anynul, 
                       &array[next], status);
                break;
            case (TLONG):
                ffgi4b(fptr, ntodo, incre, (long  *) buffer, status);
                fffi4r4((long  *) buffer, ntodo, scale, zero, nulcheck, 
                       tnull, nulval, &nularray[next], anynul, 
                       &array[next], status);
                break;
            case (TDOUBLE):
                ffgr8b(fptr, ntodo, incre, (double *) buffer, status);
                fffr8r4((double *) buffer, ntodo, scale, zero, nulcheck, 
                          nulval, &nularray[next], anynul, 
                          &array[next], status);
                break;
            case (TSTRING):
                if (ffgi1b(fptr, twidth, 1, cstring, status) <= 0)
                {
                    /* read the ASCII column string successfully */
                    cstring[twidth] = 0;  /* terminate the string */


                    /* check if null value is defined, and if the   */
                    /* column string is identical to the null string */
                    if (nulcheck && 
                        !strncmp(snull, (char *) cstring, nullen) )
                    {
                        *anynul = 1;   /* this is a null value */
                        if (nultyp == 1)
                            array[next] = nulval;
                        else
                            nularray[next] = 1;

                        break;
                    }
    
                    /* C doesn't support a D exponent character, */
                    /* so change it to an E, as in 1.2345E+03    */
                    cptr = strchr((char *) cstring, 'D');
                    if (cptr)
                       *cptr = 'E';

                    /* read the string as a double value */
                    if (sscanf((char *) cstring, cform, &dblvalue) == 1)
                    {
                        /* scale and datatype conversion */
                        fffr8r4(&dblvalue, ntodo, scale, zero, 0,
                          nulval, &nularray[next], anynul,
                          &array[next], status);

                        break;
                    }
                    else if (strspn((char *) cstring, " ") == twidth)
                    {
                        /* field is completely blank; interprete as = 0 */
                        dblvalue = 0.;

                        /* scale and datatype conversion */
                        fffr8r4(&dblvalue, ntodo, scale, zero, 0,
                          nulval, &nularray[next], anynul,
                          &array[next], status);

                        break;
                    }
                }

                sprintf(message, 
                   "Cannot read number from column %d which has format %s.",
                    colnum, tform);
                ffpmsg(message);
                sprintf(message, "Column field = %s.", cstring);
                ffpmsg(message);
                return(*status);

            default:  /*  error trap for invalid column format */
                sprintf(message, 
                   "Cannot read numbers from column %d which has format %s",
                    colnum, tform);
                ffpmsg(message);
                if (hdutype == ASCII_TBL)
                    return(*status = BAD_ATABLE_FORMAT);
                else
                    return(*status = BAD_BTABLE_FORMAT);

        } /* End of switch block */

        /*-------------------------*/
        /*  Check for fatal error  */
        /*-------------------------*/
        if (*status > 0)  /* test for error during previous read operation */
        {
         sprintf(message,
          "Error reading elements %ld thru %ld of input data array (ffgcle).",
              next+1, next+ntodo);
         return(*status);
        }

        /*--------------------------------------------*/
        /*  increment the counters for the next loop  */
        /*--------------------------------------------*/
        remain -= ntodo;
        if (remain)
        {
            next += ntodo;
            elemnum = elemnum + (ntodo * elemincre);

            if (elemnum >= repeat)  /* completed a row; start on later row */
            {
                rowincre = elemnum / repeat;
                rownum += rowincre;
                elemnum = elemnum - (rowincre * repeat);
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
        "Numerical overflow during type conversion while reading FITS data.");
        *status = NUM_OVERFLOW;
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgcld( fitsfile *fptr,   /* I - FITS file pointer                       */
            int  colnum,      /* I - number of column to read (1 = 1st col)  */
            long  firstrow,   /* I - first row to read (1 = 1st row)         */
            long  firstelem,  /* I - first vector element to read (1 = 1st)  */
            long  nelem,      /* I - number of values to read                */
            long  elemincre,  /* I - pixel increment; e.g., 2 = every other  */
            int   nultyp,     /* I - null value handling code:               */
                              /*     1: set undefined pixels = nulval        */
                              /*     2: set nularray=1 for undefined pixels  */
            double nulval,    /* I - value for null pixels if nultyp = 1     */
            double *array,    /* O - array of values that are read           */
            char *nularray,   /* O - array of flags = 1 if nultyp = 2        */
            int  *anynul,     /* O - set to 1 if any values are null; else 0 */
            int  *status)     /* IO - error status                           */
/*
  Read an array of values from a column in the current FITS HDU.
  The column number may refer to a real column in an ASCII or binary table, 
  or it may refer be a virtual column in a 1 or more grouped FITS primary
  array or image extension.  FITSIO treats a primary array as a binary table
  with 2 vector columns: the first column contains the group parameters (often
  with length = 0) and the second column contains the array of image pixels.
  Each row of the table represents a group in the case of multigroup FITS
  images.

  The output array of values will be converted from the datatype of the column
  and will be scaled by the FITS TSCALn and TZEROn values if necessary.
*/
{
    double scale, zero, dblvalue;
    int tcode, maxelem, hdutype, nullen;
    long twidth, incre, repeat, rowlen, rownum, elemnum, remain, next, ntodo;
    long ii, rowincre, tnull;
    int convert, nulcheck;
    long startpos, readptr;
    char tform[20], cform[20];
    unsigned char cstring[50];
    char message[81];
    char *cptr;

    char snull[20];   /*  the FITS null value if reading from ASCII table  */

    double cbuff[DBUFFSIZE / sizeof(double)]; /* align cbuff on word boundary */
    void *buffer;

    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    buffer = cbuff;

    *anynul = 0;
    if (nultyp == 2)
    {                                 /* initialize nullarray */
        for (ii = 0; ii < nelem; ii++)
            nularray[ii] = 0;
    }

    /*---------------------------------------------------*/
    /*  Check input and get parameters about the column: */
    /*---------------------------------------------------*/
    if ( ffgcpr( fptr, colnum, firstrow, firstelem, nelem, 0, &scale, &zero,
         tform, &twidth, &tcode, &maxelem, &startpos, &elemnum, &incre,
         &repeat, &rowlen, &hdutype, &tnull, snull, status) > 0 )
         return(*status);

    incre *= elemincre;   /* multiply incre to just get every nth pixel */

    if (tcode == TSTRING)    /* setup for ASCII tables */
    {
        nullen = strlen(snull);   /* length of the undefined pixel string */
        cform[0] = '%';           /* construct read format string */
        sprintf(&cform[1], "%ldle", twidth); 
    }

    /*------------------------------------------------------------------*/
    /*  Decide whether to check for null values in the input FITS file: */
    /*------------------------------------------------------------------*/
    nulcheck = nultyp; /* by default check for null values in the FITS file */

    if (nultyp == 1 && nulval == 0)
       nulcheck = 0;    /* calling routine does not want to check for nulls */

    else if (tcode%10 == 1 &&        /* if reading an integer column, and  */ 
            tnull == NULL_UNDEFINED) /* if a null value is not defined,    */
            nulcheck = 0;            /* then do not check for null values. */

    else if (tcode == TSTRING && snull[0] == ASCII_NULL_UNDEFINED)
         nulcheck = 0;

    /*----------------------------------------------------------------------*/
    /*  If FITS column and output data array have same datatype, then we do */
    /*  not need to use a temporary buffer to store intermediate datatype.  */
    /*----------------------------------------------------------------------*/
    if (tcode == TDOUBLE) /* Special Case:                        */
    {                              /* no type convertion required, so read */
        maxelem = nelem;           /* data directly into output buffer.    */

        if (nulcheck == 0 && scale == 1. && zero == 0.)
            convert = 0;  /* no need to scale data or find nulls */
        else
            convert = 1;
    }

    /*---------------------------------------------------------------------*/
    /*  Now read the pixels from the FITS column. If the column does not   */
    /*  have the same datatype as the output array, then we have to read   */
    /*  the raw values into a temporary buffer (of limited size).  In      */
    /*  the case of a vector colum read only 1 vector of values at a time  */
    /*  then skip to the next row if more values need to be read.          */
    /*  After reading the raw values, then call the fffXXYY routine to (1) */
    /*  test for undefined values, (2) convert the datatype if necessary,  */
    /*  and (3) scale the values by the FITS TSCALn and TZEROn linear      */
    /*  scaling parameters.                                                */
    /*---------------------------------------------------------------------*/
    remain = nelem;           /* remaining number of values to read */
    next = 0;                 /* next element in array to be read   */
    rownum = 0;               /* row number, relative to firstrow   */
    ntodo = remain;           /* max number of elements to read at one time */

    while (ntodo)
    {
        /* limit the number of pixels to read at one time to the number that
           will fit in the buffer or to the number of pixels that remain in
           the current vector, which ever is smaller.
        */
        ntodo = minvalue(ntodo, maxelem);      
        ntodo = minvalue(ntodo, ((repeat - elemnum - 1)/elemincre +1));

        readptr = startpos + (rownum * rowlen) + (elemnum * incre / elemincre);

        ffmbyt(fptr, readptr, 1, status);  /* move to read position in file */

        switch (tcode) 
        {
            case (TDOUBLE):
                ffgr8b(fptr, ntodo, incre, &array[next], status);
                if (convert)
                    fffr8r8(&array[next], ntodo, scale, zero, nulcheck, 
                           nulval, &nularray[next], anynul, 
                           &array[next], status);
                break;
            case (TBYTE):
                ffgi1b(fptr, ntodo, incre, (unsigned char *) buffer, status);
                fffi1r8((unsigned char *) buffer, ntodo, scale, zero, nulcheck, 
                   (unsigned char) tnull, nulval, &nularray[next], anynul, 
                   &array[next], status);
                break;
            case (TSHORT):
                ffgi2b(fptr, ntodo, incre, (short  *) buffer, status);
                fffi2r8((short  *) buffer, ntodo, scale, zero, nulcheck, 
                    (short) tnull, nulval, &nularray[next], anynul, 
                       &array[next], status);
                break;
            case (TLONG):
                ffgi4b(fptr, ntodo, incre, (long  *) buffer, status);
                fffi4r8((long  *) buffer, ntodo, scale, zero, nulcheck, 
                       tnull, nulval, &nularray[next], anynul, 
                       &array[next], status);
                break;
            case (TFLOAT):
                ffgr4b(fptr, ntodo, incre, (float  *) buffer, status);
                fffr4r8((float  *) buffer, ntodo, scale, zero, nulcheck, 
                          nulval, &nularray[next], anynul, 
                          &array[next], status);
                break;
            case (TSTRING):
                if (ffgi1b(fptr, twidth, 1, cstring, status) <= 0)
                {
                    /* read the ASCII column string successfully */
                    cstring[twidth] = 0;  /* terminate the string */


                    /* check if null value is defined, and if the   */
                    /* column string is identical to the null string */
                    if (nulcheck && 
                        !strncmp(snull, (char *) cstring, nullen) )
                    {
                        *anynul = 1;   /* this is a null value */
                        if (nultyp == 1)
                            array[next] = nulval;
                        else
                            nularray[next] = 1;

                        break;
                    }
    
                    /* C doesn't support a D exponent character, */
                    /* so change it to an E, as in 1.2345E+03    */
                    cptr = strchr((char *) cstring, 'D');
                    if (cptr)
                       *cptr = 'E';

                    /* read the string as a double value */
                    if (sscanf((char *) cstring, cform, &dblvalue) == 1)
                    {
                        /* scale and datatype conversion */
                        fffr8r8(&dblvalue, ntodo, scale, zero, 0,
                          nulval, &nularray[next], anynul,
                          &array[next], status);

                        break;
                    }
                    else if (strspn((char *) cstring, " ") == twidth)
                    {
                        /* field is completely blank; interprete as = 0 */
                        dblvalue = 0.;

                        /* scale and datatype conversion */
                        fffr8r8(&dblvalue, ntodo, scale, zero, 0,
                          nulval, &nularray[next], anynul,
                          &array[next], status);

                        break;
                    }
                }

                sprintf(message, 
                   "Cannot read number from column %d which has format %s.",
                    colnum, tform);
                ffpmsg(message);
                sprintf(message, "Column field = %s.", cstring);
                ffpmsg(message);
                return(*status);

            default:  /*  error trap for invalid column format */
                sprintf(message, 
                   "Cannot read numbers from column %d which has format %s",
                    colnum, tform);
                ffpmsg(message);
                if (hdutype == ASCII_TBL)
                    return(*status = BAD_ATABLE_FORMAT);
                else
                    return(*status = BAD_BTABLE_FORMAT);

        } /* End of switch block */

        /*-------------------------*/
        /*  Check for fatal error  */
        /*-------------------------*/
        if (*status > 0)  /* test for error during previous read operation */
        {
         sprintf(message,
          "Error reading elements %ld thru %ld of input data array (ffgcld).",
              next+1, next+ntodo);
         return(*status);
        }

        /*--------------------------------------------*/
        /*  increment the counters for the next loop  */
        /*--------------------------------------------*/
        remain -= ntodo;
        if (remain)
        {
            next += ntodo;
            elemnum = elemnum + (ntodo * elemincre);

            if (elemnum >= repeat)  /* completed a row; start on later row */
            {
                rowincre = elemnum / repeat;
                rownum += rowincre;
                elemnum = elemnum - (rowincre * repeat);
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
        "Numerical overflow during type conversion while reading FITS data.");
        *status = NUM_OVERFLOW;
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgcx(  fitsfile *fptr,  /* I - FITS file pointer                       */
            int   colnum,    /* I - number of column to write (1 = 1st col) */
            long  frow,      /* I - first row to write (1 = 1st row)        */
            long  fbit,      /* I - first bit to write (1 = 1st)            */
            long  nbit,      /* I - number of bits to write                 */
            char *larray,    /* O - array of logicals corresponding to bits */
            int  *status)    /* IO - error status                           */
/*
  read an array of logical values from a specified bit or byte
  column of the binary table.   If larray is TRUE, then the corresponding
  bit is set to 1, otherwise it is set to 0.
  The binary table column being read to must have datatype 'B' or 'X'. 
*/
{
    long bstart, offset, fbyte, bitloc, ndone;
    long ii, repeat, rstart, estart;
    int tcode, descrp;
    unsigned char cbuff;
    static unsigned char onbit[8] = {128,  64,  32,  16,   8,   4,   2,   1};
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
        /* N.B: REPEAT is the number of bytes, not number of bits */
        repeat = colptr->trepeat;

        if (tcode == TBIT)
            repeat = (repeat + 7) / 8;  /* convert from bits to bytes */

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

        ffgdes(fptr, colnum, frow, &repeat, &offset, status);
        repeat = (repeat + 7) / 8;

        if ((fbit + nbit + 6) / 8 > repeat)
            return(*status = BAD_ELEM_NUM);

        /* calc the i/o pointer location to start of sequence of pixels */
        bstart = fptr->datastart + offset + fptr->heapstart + estart;
    }

    /* move the i/o pointer to the start of the pixel sequence */
    if (ffmbyt(fptr, bstart, REPORT_EOF, status) > 0)
        return(*status);

    /* read the next byte */
    while (1)
    {
      if (ffgbyt(fptr, 1, &cbuff, status) > 0)
        return(*status);

      for (ii = bitloc; (ii < 8) && (ndone < nbit); ii++, ndone++)
      {
        if(cbuff & onbit[ii])       /* test if bit is set */
          larray[ndone] = TRUE;
        else
          larray[ndone] = FALSE;
      }

      if (ndone == nbit)   /* finished all the bits */
        return(*status);

      /* not done, so get the next byte */
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

          ffmbyt(fptr, bstart, REPORT_EOF, status);
        }
      }
      bitloc = 0;
    }
}
/*--------------------------------------------------------------------------*/
int ffgdes(fitsfile *fptr, /* I - FITS file pointer                         */
           int colnum,     /* I - column number (1 = 1st column of table)   */
           long rownum,    /* I - row number (1 = 1st row of table)         */
           long *length,   /* O - number of elements in the row             */
           long *heapaddr, /* O - heap pointer to the data                  */
           int *status)    /* IO - error status                             */
/*
  get (read) the variable length vector descriptor from the table.
*/
{
    long bytepos, descript[2];
    tcolumn *colptr;

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

        ffmbyt(fptr, bytepos, REPORT_EOF, status); /* move to element */

        ffgi4b(fptr, 2, 4, descript, status); /* read the descriptor */

        *length = descript[0];   /* 1st word of descriptor is the length  */
        *heapaddr = descript[1]; /* 2nd word of descriptor is the address */
     }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgtbb(fitsfile *fptr,        /* I - FITS file pointer                 */
           long firstrow,         /* I - starting row (1 = first row)      */
           long firstchar,        /* I - starting byte in row (1=first)    */
           long nchars,           /* I - number of bytes to read           */
           unsigned char *values, /* I - array of bytes to read            */
           int *status)           /* IO - error status                     */
/*
  read a consecutive string of bytes from an ascii or binary table.
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

    /* move the i/o pointer to the start of the sequence of characters */
    bytepos = fptr->datastart + ( (firstrow - 1) * fptr->rowlength )
            + firstchar - 1;

    ffmbyt(fptr, bytepos, REPORT_EOF, status);
    ffgbyt(fptr, nchars, values, status);  /* read the bytes */

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgi1b(fitsfile *fptr, /* I - FITS file pointer                         */
           long nvals,     /* I - number of pixels to read                  */
           long incre,     /* I - byte increment between pixels             */
           unsigned char *values, /* O - returned array of values           */
           int *status)    /* IO - error status                             */
/*
  get (read) the array of values from the FITS file, doing machine dependent
  format conversion (e.g. byte-swapping) if necessary.
*/
{
    long ii, offset;

    if (incre == 1)      /* read all the values at once (contiguous bytes) */

        ffgbyt(fptr, nvals, values, status);

    else         /* have to read each value individually (not contiguous ) */
    {
        offset = incre - 1;   /* the number of bytes to move between values */

        ffgbyt(fptr, 1, values, status);    /* read first value */

        for (ii = 1; ii < nvals; ii++)
        {
            ffmoff(fptr, offset, REPORT_EOF, status); /* offset to next pos */
            ffgbyt(fptr, 1, &values[ii], status);    /* read the next value */
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgi2b(fitsfile *fptr,  /* I - FITS file pointer                        */
           long nvals,      /* I - number of pixels to read                 */
           long incre,      /* I - byte increment between pixels            */
           short *values,   /* O - returned array of values                 */
           int *status)     /* IO - error status                            */
/*
  get (read) the array of values from the FITS file, doing machine dependent
  format conversion (e.g. byte-swapping) if necessary.
*/
{
    long ii, offset;

    if (incre == 2)      /* read all the values at once (contiguous bytes) */

        ffgbyt(fptr, nvals * 2, values, status);

    else         /* have to read each value individually (not contiguous ) */
    {
        offset = incre - 2;   /* the number of bytes to move between values */

        ffgbyt(fptr, 2, values, status);    /* read first value */

        for (ii = 1; ii < nvals; ii++)
        {
            ffmoff(fptr, offset, REPORT_EOF, status); /* offset to next pos */
            ffgbyt(fptr, 2, &values[ii], status);    /* read the next value */
        }
    }

#if BYTESWAPPED == TRUE

    ffswap2(values, nvals);    /* reverse order of bytes in each value */

#endif

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgi4b(fitsfile *fptr,  /* I - FITS file pointer                        */
           long nvals,      /* I - number of pixels to read                 */
           long incre,      /* I - byte increment between pixels            */
           long *values,    /* O - returned array of values                 */
           int *status)     /* IO - error status                            */
/*
  get (read) the array of values from the FITS file, doing machine dependent
  format conversion (e.g. byte-swapping) if necessary.
*/
{
    long ii, offset;
    char *ptr;


    if (incre == 4)      /* read all the values at once (contiguous bytes) */

        ffgbyt(fptr, nvals * 4, values, status);

    else         /* have to read each value individually (not contiguous ) */
    {
        offset = incre - 4;   /* the number of bytes to move between values */

        ptr = (char *) values;

        ffgbyt(fptr, 4, ptr, status);    /* read first value */

        for (ii = 1; ii < nvals; ii++)
        {
            ffmoff(fptr, offset, REPORT_EOF, status); /* offset to next pos */
            ptr += 4;
            ffgbyt(fptr, 4, ptr, status);    /* read the next value */
        }
    }

#if BYTESWAPPED == TRUE

    ffunswaplong(values, nvals);    /* reverse order of bytes in each value */

#endif

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgr4b(fitsfile *fptr,  /* I - FITS file pointer                        */
           long nvals,      /* I - number of pixels to read                 */
           long incre,      /* I - byte increment between pixels            */
           float *values,   /* O - returned array of values                 */
           int *status)     /* IO - error status                            */
/*
  get (read) the array of values from the FITS file, doing machine dependent
  format conversion (e.g. byte-swapping) if necessary.
*/
{
    short *sptr;
    long ii, offset;

    if (incre == 4)      /* read all the values at once (contiguous bytes) */

        ffgbyt(fptr, nvals * 4, values, status);

    else         /* have to read each value individually (not contiguous ) */
    {
        offset = incre - 4;   /* the number of bytes to move between values */

        ffgbyt(fptr, 4, values, status);    /* read first value */

        for (ii = 1; ii < nvals; ii++)
        {
            ffmoff(fptr, offset, REPORT_EOF, status); /* offset to next pos */
            ffgbyt(fptr, 4, &values[ii], status);    /* read the next value */
        }
    }

#if MACHINE == VAXVMS

    ii = nvals;                      /* call VAX macro routine to convert */
    ieevur(values, values, &ii);     /* from  IEEE float -> F float       */

#elif MACHINE == ALPHAVMS

    if (*(short *) &testfloat == GFLOAT)
    {
        ffswap2( (short *) values, nvals * 2);  /* swap pairs of bytes */

        /* convert from IEEE float format to VMS GFLOAT float format */
        sptr = (short *) values;
        for (ii = 0; ii < nvals; ii++, sptr += 2)
        {
            if (!fnan(*sptr) )  /* test for NaN or underflow */
                values[ii] *= 4.0;
        }
    }
    else
        ffswapfloat(values, nvals); /* reverse order of bytes in each value */

#elif BYTESWAPPED == TRUE

    ffswapfloat(values, nvals);  /* reverse order of bytes in each value */

#endif

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgr8b(fitsfile *fptr,  /* I - FITS file pointer                        */
           long nvals,      /* I - number of pixels to read                 */
           long incre,      /* I - byte increment between pixels            */
           double *values,  /* O - returned array of values                 */
           int *status)     /* IO - error status                            */
/*
  get (read) the array of values from the FITS file, doing machine dependent
  format conversion (e.g. byte-swapping) if necessary.
*/
{
    short *sptr;
    long ii, offset;

    if (incre == 8)      /* read all the values at once (contiguous bytes) */

        ffgbyt(fptr, nvals * 8, values, status);

    else          /* have to read each value individually (not contiguous ) */
    {
        offset = incre - 8;   /* the number of bytes to move between values */

        ffgbyt(fptr, 8, values, status);    /* read first value */

        for (ii = 1; ii < nvals; ii++)
        {
            ffmoff(fptr, offset, REPORT_EOF, status); /* offset to next pos */
            ffgbyt(fptr, 8, &values[ii], status);    /* read the next value */
        }
    }

#if MACHINE == VAXVMS

    ii = nvals;                      /* call VAX macro routine to convert */
    ieevud(values, values, &ii);     /* from  IEEE float -> D float       */

#elif MACHINE == ALPHAVMS

    if (*(short *) &testfloat == GFLOAT)
    {
        ffswap2( (short *) values, nvals * 4);  /* swap pairs of bytes */

        /* convert from IEEE float format to VMS GFLOAT float format */
        sptr = (short *) values;
        for (ii = 0; ii < nvals; ii++, sptr += 4)
        {
            if (!dnan(*sptr) )  /* test for NaN or underflow */
                values[ii] *= 4.0;
        }
    }
    else
        ffswap8(values, nvals); /* reverse order of bytes in each value */

#elif BYTESWAPPED == TRUE

    ffswap8(values, nvals);   /* reverse order of bytes in each value */

#endif

    return(*status);
}

