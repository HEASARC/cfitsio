/*  This file, fitscore.c, contains the core set of FITSIO routines.       */

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
#include <math.h>
#include <limits.h>
#include <time.h>
#include "fitsio2.h"

/*--------------------------------------------------------------------------*/
int ffopen(fitsfile **fptr,      /* O - FITS file pointer                   */ 
           const char *filename, /* I - name of file to open                */
           int readwrite,        /* I - 0 = open readonly; 1 = read/write   */
           int *status)          /* IO - error status                       */
{
    int ii, hdutype, slen, tstatus;
    long filesize;
    FILE *diskfile;

    /* initialize a null file pointer */
    *fptr = 0;

    if (*status > 0)
        return(*status);

    ii = 0;
    while (filename[ii] == ' ')  /* ignore leading spaces in the filename */
        ii++;

    if (!filename[ii])
    {
        ffpmsg("Name of file to open is blank. (ffopen)");
        return(*status = FILE_NOT_OPENED);
    }

    /* allocate mem and init = 0 */
    *fptr = (fitsfile *) calloc(1, sizeof(fitsfile)); 
    if (*fptr)
    {
      ffopenx(&diskfile, &filename[ii], 0, readwrite, 
              &filesize, &tstatus);                   /* open the file */

      if (!tstatus)
      {
        (*fptr)->fileptr = diskfile;  /* copy file pointer to the structure */

        slen = strlen(filename) + 1;
        slen = maxvalue(slen, 20); /* malloc sometimes crashes on small lens */ 
        (*fptr)->filename = (char *) malloc(slen); /* mem for   name */

        if ( (*fptr)->filename )
        {
          strcpy((*fptr)->filename, filename);   /* store the filename */

          (*fptr)->filesize = filesize;
          (*fptr)->writemode = readwrite;        /* read-write mode    */
          (*fptr)->datastart = DATA_UNDEFINED;   /* unknown start of data */

          if (ffrhdu(*fptr, &hdutype, status) > 0)  /* set HDU structure */
          {
             ffpmsg(
                "ffopen could not interpret primary array header of file:");
             ffpmsg(&filename[ii]);

             if (*status == UNKNOWN_REC)
                 ffpmsg("This does not look like a FITS file.");
          }
          return(*status);   
        }
        /* ERROR: failed to get memory for file name string */
        ffclosex(diskfile, &filename[ii], 1, &tstatus);  /* close the file */
      }
      free(*fptr);            /* free memory for the FITS structure */
      *fptr = 0;              /* return null file pointer */
    }

    ffpmsg("ffopen failed to find and/or open the following file:");
    ffpmsg(filename);

    return(*status = FILE_NOT_OPENED);
}
/*--------------------------------------------------------------------------*/
int ffinit(fitsfile **fptr,      /* O - FITS file pointer                   */
           const char *filename, /* I - name of file to create              */
           int *status)          /* IO - error status                       */
{
    int ii, slen, tstatus;
    long dummy;
    FILE *diskfile;

    /* initialize a null file pointer */
    *fptr = 0;

    if (*status > 0)
        return(*status);

    ii = 0;
    while (filename[ii] == ' ')  /* ignore leading spaces in the filename */
        ii++;

    if (!filename[ii])
    {
        ffpmsg("Name of file to create is blank. (ffinit)");
        return(*status = FILE_NOT_CREATED);
    }

    /* allocate mem and init = 0 */
    *fptr = (fitsfile *) calloc(1, sizeof(fitsfile));
    if (*fptr)
    {
      ffopenx( &diskfile, &filename[ii], 1, 1, 
               &dummy, &tstatus);                /* create file */

      if (!tstatus)
      {
        (*fptr)->fileptr = diskfile;  /* copy file pointer to the structure */

        slen = strlen(filename) + 1;
        slen = maxvalue(slen, 20); /* malloc sometimes crashes on small lens */ 
        (*fptr)->filename = (char *) malloc(slen); /* mem for   name */

        if ( (*fptr)->filename )
        {
          strcpy((*fptr)->filename, filename);   /* store the filename */
          (*fptr)->writemode = 1;                /* read-write mode    */
          (*fptr)->datastart = DATA_UNDEFINED;   /* unknown start of data */
          return(*status);                       /* successful return */
        }

        /* ERROR: failed to get memory for file name string */
        ffclosex(diskfile, &filename[ii], 0, &tstatus);  /* DELETE the file */
      } 
      free(*fptr);            /* free memory for the FITS structure */
      *fptr = 0;              /* return null file pointer */
     }

    ffpmsg("ffinit failed to create the following new file:");
    ffpmsg(filename);

    return(*status = FILE_NOT_CREATED);
}
/*--------------------------------------------------------------------------*/
int ffclos(fitsfile *fptr,      /* I - FITS file pointer */
           int *status)         /* IO - error status     */
/*
  close the FITS file by completing the current HDU, flushing it to disk,
  then calling the system dependent routine to physically close the FITS file
*/   
{
    int tstatus;

    /* if null pointer file is not opened, so just return */
    if (!fptr)
        return(*status);

    ffchdu(fptr, status);           /* close and flush the current HDU */

    ffclosex(fptr->fileptr, fptr->filename, 1, &tstatus);  /* close file */

    if (*status <= 0 && tstatus > 0)
        *status = FILE_NOT_CLOSED;  /* report error if no previous error */

    free(fptr->filename);       /* free memory for the filename */
    fptr->filename = 0;
    free(fptr);                 /* free memory for the FITS file structure */

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffdelt(fitsfile *fptr,      /* I - FITS file pointer */
           int *status)         /* IO - error status     */
/*
  close and DELETE the FITS file 
*/
{    
    int tstatus = 0;

    ffchdu(fptr, &tstatus);    /* close the current HDU, ignore any errors */

    ffclosex(fptr->fileptr, fptr->filename, 0, &tstatus);  /* DELETE file */

    if (*status <= 0 && tstatus > 0)
        *status = FILE_NOT_CLOSED;  /* report error if no previous error */

    free(fptr->filename);       /* free memory for the filename */
    fptr->filename = 0;
    free(fptr);                 /* free memory for the FITS file structure */

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffflus(fitsfile *fptr,   /* I - FITS file pointer                       */
           int *status)      /* IO - error status                           */
/*
  Flush all the data in the current FITS file to disk. This ensures that if
  the program subsequently dies, the disk FITS file will be closed correctly.
*/
{
    int hdunum, hdutype;

    if (*status > 0)
        return(*status);

    ffghdn(fptr, &hdunum);     /* get the current HDU number */
  
    if (ffchdu(fptr,status) > 0)   /* close out the current HDU */
        ffpmsg("ffflus could not close the current HDU.");

    else if (ffgext(fptr, hdunum - 1, &hdutype, status) > 0) /* reopen HDU */
        ffpmsg("ffflus could not reopen the current HDU.");

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffghdn(fitsfile *fptr,   /* I - FITS file pointer                      */
            int *chdunum)    /* O - number of the CHDU; 1 = primary array  */
/*
  Return the number of the Current HDU in the FITS file.  The primary array
  is HDU number 1.  Note that this is one of the few cfitsio routines that
  does not return the error status value as the value of the function.
*/
{
    *chdunum = (fptr->curhdu) + 1;       

    return(*chdunum);
}
/*--------------------------------------------------------------------------*/
void ffghad(fitsfile *fptr,   /* I - FITS file pointer                     */
            long *chduaddr,   /* O - byte offset to beginning of CHDU      */
            long *nextaddr)   /* O - byte offset to beginning of next HDU  */
/*
  Return the address (= byte offset) in the FITS file to the beginning of
  the current HDU and the offset to the beginning of the next HDU following
  the CHDU.
*/
{
    *chduaddr = fptr->headstart[fptr->curhdu];       
    *nextaddr = fptr->headstart[(fptr->curhdu) + 1];       

    return;
}
/*--------------------------------------------------------------------------*/
int ffmbyt(fitsfile *fptr,    /* I - FITS file pointer                */
           long bytepos,      /* I - byte position in file to move to */
           int err_mode,      /* I - 1=ignore error, 0 = return error */
           int *status)       /* IO - error status                    */
{
/*
  Move to the input byte location in the file.  When writing to a file, a move
  may sometimes be made to a position beyond the current EOF.  The err_mode
  parameter determines whether such conditions should be returned as an error
  or simply ignored.
*/
    if (*status > 0)
       return(*status);

    if (bytepos < 0)
        return(*status = NEG_FILE_POS);

    fptr->bytepos = bytepos;

    if (bytepos > fptr->filesize && err_mode == REPORT_EOF)
        *status = END_OF_FILE;

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffmoff(fitsfile *fptr,    /* I - FITS file pointer                 */
           long offset,       /* I - number of bytes to move (+ or - ) */
           int err_mode,      /* I - 1=ignore error, 0 = return error  */
           int *status)       /* IO - error status                     */
{
/*
  Offset the file pointer.  When writing to a file, a move
  may sometimes be made to a position beyond the current EOF.  The err_mode
  parameter determines whether such conditions should be returned as an error
  or simply ignored.
*/
    if (*status > 0)
       return(*status);

    fptr->bytepos += offset;

    if (fptr->bytepos > fptr->filesize && err_mode == REPORT_EOF)
        *status = END_OF_FILE;

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpbyt(fitsfile *fptr,   /* I - FITS file pointer                    */
           int nbytes,       /* I - number of bytes to write             */
           void *buffer,     /* I - buffer containing the bytes to write */
           int *status)      /* IO - error status                        */
/*
  put (write) the buffer of bytes to the output FITS file, starting at
  the current file position.  If the write starting position is beyond
  the current EOF, then fill in the gap with the correct fill value for
  this type of HDU.
*/
{
    static int initialized = 0;
    static char *zeros;
    static char *blanks;

    int ii;
    long ntowrite, ntodo;

    char *cptr;

    if (fptr->bytepos > fptr->filesize)
    {
        /* fill in the gap between end of file and starting write position */

        if (!initialized)
        {
          /* set up 2 static arrays full of zeros and blanks that will    */
          /* be used to initialize any gaps in the FITS file between      */
          /* the current end of the file and the starting write position. */

            zeros  = (char *) calloc(2880, 1); /* block of zeros            */
            blanks = (char *) calloc(2880, 1); /* block of blanks: ASCII 32 */

            for (ii = 0; ii < 2880; ii++)
                 blanks[ii] = 32;

            initialized = 1;
        }


        if (fptr->hdutype == ASCII_TBL)
            cptr = blanks;  /* this HDU requires blank fill values */
        else
            cptr = zeros;   /* this HDU requires zero fill values */

        ntowrite = ((fptr->bytepos) - (fptr->filesize)); /* total fill */
        while (ntowrite)
        {
            ntodo = minvalue(ntowrite, 2880);

            ffwrite(fptr->fileptr, fptr->filesize, ntodo, cptr, status);

            fptr->filesize += ntodo;   /* increment the file size */
            ntowrite -= ntodo;         /* decrement the remaining bytes */
        }            

    }

    /*  write the data */
    ffwrite(fptr->fileptr, fptr->bytepos, nbytes, buffer, status);

    fptr->bytepos += nbytes;    /* update current pointer position and ... */
    fptr->filesize = maxvalue(fptr->filesize, fptr->bytepos); /* file size */

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgbyt(fitsfile *fptr,    /* I - FITS file pointer             */
           long nbytes,       /* I - number of bytes to read       */
           void *buffer,      /* O - buffer to read into           */
           int *status)       /* IO - error status                 */
/*
  get (read) the requested number of bytes from the file, starting at
  the current file position
*/
{
    ffread(fptr->fileptr, fptr->bytepos, nbytes, buffer, status);

    fptr->bytepos += nbytes;  /* update current pointer position */

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffrhdu(fitsfile *fptr,    /* I - FITS file pointer */
           int *hdutype,      /* O - type of HDU       */
           int *status)       /* IO - error status     */
/*
  read the required keywords of the CHDU and initialize the corresponding
  structure elements that describe the format of the HDU
*/
{
    int ii, tstatus;
    char card[FLEN_CARD];
    char name[FLEN_KEYWORD], value[FLEN_VALUE], comm[FLEN_COMMENT];
    char xtension[FLEN_VALUE];

    if (*status > 0)
        return(*status);

    if (ffgrec(fptr, 1, card, status) > 0 )  /* get the 80-byte card */
    {
        ffpmsg("Cannot read first keyword in header (ffrhdu).");
        return(*status);
    }

    strncpy(name,card,8);  /* first 8 characters = the keyword name */
    name[8] = '\0';

    for (ii=7; ii >= 0; ii--)  /* replace trailing blanks with nulls */
    {
        if (name[ii] == ' ')
            name[ii] = '\0';
        else
            break;
    }
    if (ffpsvc(card, value, comm, status) > 0)   /* parse value and comment */
    {
        ffpmsg("Cannot read value of first  keyword in header (ffrhdu):");
        ffpmsg(card);
        return(*status);
    }

    if (!strcmp(name, "SIMPLE"))        /* this is the primary array */
    {
       ffpinit(fptr, status);           /* initialize the primary array */
       *hdutype = 0;
    }

    else if (!strcmp(name, "XTENSION"))   /* this is an XTENSION keyword */
    {
        if (ffc2s(value, xtension, status) > 0)  /* get the value string */
        {
            ffpmsg("Bad value string for XTENSION keyword:");
            ffpmsg(value);
            return(*status);
        }

        if (!strcmp(xtension, "TABLE"))
        {
            ffainit(fptr, status);       /* initialize the ASCII table */
            *hdutype = 1;
        }

        else if (!strcmp(xtension, "BINTABLE") ||
                 !strcmp(xtension, "A3DTABLE") ||
                 !strcmp(xtension, "3DTABLE") )
        {
            ffbinit(fptr, status);       /* initialize the binary table */
            *hdutype = 2;
        }

        else
        {
            tstatus = 0;
            ffpinit(fptr, &tstatus);       /* probably an IMAGE extension */

            if (tstatus == UNKNOWN_EXT)
                *hdutype = -1;       /* don't recognize this extension type */
            else
            {
                *status = tstatus;
                *hdutype = 0;
            }
        }
    }

    else     /*  not the start of a new extension */
    {
        if (card[0] == 0  ||
            card[0] == 10)     /* some editors append this character to EOF */
        {           
            *status = END_OF_FILE;
        }
        else
        {
          *status = UNKNOWN_REC;  /* found unknown type of record */
          ffpmsg
        ("Extension doesn't start with SIMPLE or XTENSION keyword. (ffrhdu)");
        }
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpinit(fitsfile *fptr,      /* I - FITS file pointer */
           int *status)          /* IO - error status     */
/*
  initialize the parameters defining the structure of the primary array
  or an Image extension 
*/
{
    int groups, tstatus, simple, bitpix, naxis, extend, nspace;
    int ttype, bytlen, ii;
    long naxes[999], pcount, gcount, npix, blank;
    double bscale, bzero;
    char comm[FLEN_COMMENT];
    tcolumn *colptr;

    if (*status > 0)
        return(*status);

    fptr->hdutype = IMAGE_HDU; /* primary array or IMAGE extension  */
    fptr->headend = 2000000000;  /* temporarily set huge header size  */

    groups = 0;
    tstatus = *status;

    /* get all the descriptive info about this HDU */
    ffgphd(fptr, 999, &simple, &bitpix, &naxis, naxes, &pcount, &gcount, 
           &extend, &bscale, &bzero, &blank, &nspace, status);

    if (*status == NOT_IMAGE)
        *status = tstatus;    /* ignore 'unknown extension type' error */
    else if (*status > 0)
        return(*status);

    /*
       the logical end of the header is 80 bytes before the current position, 
       minus any trailing blank keywords just before the END keyword.
    */
    fptr->headend = fptr->nextkey - (80 * (nspace + 1));

    /* the data unit begins at the beginning of the next logical block */
    fptr->datastart = ( (fptr->nextkey - 80) / 2880 + 1) * 2880;

    if (naxis > 0 && naxes[0] == 0)  /* test for 'random groups' */
    {
        tstatus = 0;
        if (ffgkyl(fptr, "GROUPS", &groups, comm, &tstatus))
            groups = 0;          /* GROUPS keyword not found */
    }

    if (bitpix == 8)   /* test  bitpix and set the datatype code */
    {
        ttype=TBYTE;
        bytlen=1;
    }
    else if (bitpix == 16)
    {
        ttype=TSHORT;
        bytlen=2;
    }
    else if (bitpix == 32)
    {
        ttype=TLONG;
        bytlen=4;
    }
    else if (bitpix == -32)
    {
        ttype=TFLOAT;
        bytlen=4;
    }
    else if (bitpix == -64)
    {
        ttype=TDOUBLE;
        bytlen=8;
    }
        
    /*   calculate the size of the primary array  */
    if (naxis == 0)
        npix = 0;
    else
    {
        if (groups)
            npix = 1;  /* NAXIS1 = 0 is a special flag for 'random groups' */
        else
            npix = naxes[0];

        for (ii=1; ii < naxis; ii++)
             npix = npix*naxes[ii];   /* calc number of pixels in the array */
    }

    /*
       now we know everything about the array; just fill in the parameters:
       the next HDU begins in the next logical block after the data
    */
    fptr->headstart[ fptr->curhdu + 1] =
         fptr->datastart + 
         ( (pcount + npix) * bytlen * gcount + 2879) / 2880 * 2880;

    /*
      initialize the fictitious heap starting address (immediately following
      the array data) and a zero length heap.  This is used to find the
      end of the data when checking the fill values in the last block. 
    */
    fptr->heapstart = (pcount + npix) * bytlen * gcount;
    fptr->heapsize = 0;

    if (naxis == 0)
    {
        fptr->rowlength = 0;    /* rows have zero length */
        fptr->tfield = 0;       /* table has no fields   */
        fptr->tableptr = 0;     /* set a null table structure pointer */
    }
    else
    {
      /*
        The primary array is actually interpreted as a binary table.  There
        are two columns: the first column contains the group parameters if any.
        The second column contains the primary array of data as a single vector
        column element. In the case of 'random grouped' format, each group
        is stored in a separate row of the table.
      */
        fptr->rowlength = (pcount + npix) * bytlen; /* total size of image */
        fptr->tfield = 2;  /* 2 fields: group parameters and the image */

        if (fptr->tableptr)
           free(fptr->tableptr); /* free memory for the old CHDU structure */

        colptr = (tcolumn *) calloc(2, sizeof(tcolumn) ) ;

        if (!colptr)
        {
          ffpmsg
          ("malloc failed to get memory for FITS array descriptors (ffpinit)");
          fptr->tableptr = 0;  /* set a null table structure pointer */
          return(*status = ARRAY_TOO_BIG);
        }

        /* copy the table structure address to the fitsfile structure */
        fptr->tableptr = colptr; 

        /* the first column represents the group parameters, if any */
        colptr->tbcol = 0;
        colptr->tdatatype = ttype;
        colptr->trepeat = pcount;
        colptr->tscale = 1.;
        colptr->tzero = 0.;
        colptr->tnull = blank;

        colptr++;  /* increment pointer to the second column */

        /* the second column represents the image array */
        colptr->tbcol = pcount * bytlen; /* col starts after the group parms */
        colptr->tdatatype = ttype; 
        colptr->trepeat = npix;
        colptr->tscale = bscale;
        colptr->tzero = bzero;
        colptr->tnull = blank;
    }

    /* reset next keyword pointer to the start of the header */
    fptr->nextkey = fptr->headstart[ fptr->curhdu ];

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffainit(fitsfile *fptr,      /* I - FITS file pointer */
            int *status)         /* IO - error status     */
{
/*
  initialize the parameters defining the structure of an ASCII table 
*/
    int  ii, nspace, tbcoln;
    long nrows, rowlen, pcount, tfield;
    tcolumn *colptr;
    char name[FLEN_KEYWORD], value[FLEN_VALUE], comm[FLEN_COMMENT];
    char message[FLEN_ERRMSG];

    if (*status > 0)
        return(*status);

    fptr->hdutype = ASCII_TBL;  /* set that this is an ASCII table */
    fptr->headend = 2000000000;       /* temporarily set huge header size  */

    /* get table parameters and test that the header is a valid: */
    if (ffgttb(fptr, &rowlen, &nrows, &pcount, &tfield, status) > 0)  
       return(*status);

    if (pcount != 0)
    {
       ffpmsg("PCOUNT keyword not equal to 0 in ASCII table (ffainit).");
       return(*status = BAD_PCOUNT);
    }

    fptr->rowlength = rowlen; /* store length of a row */
    fptr->tfield = tfield;    /* store number of table fields in each row */

    if (fptr->tableptr)
       free(fptr->tableptr); /* free memory for the old CHDU structure */

    /* mem for column structures ; space is initialized = 0  */
    colptr = (tcolumn *) calloc(tfield, sizeof(tcolumn) );
    if (!colptr)
        {
          ffpmsg
          ("malloc failed to get memory for FITS table descriptors (ffainit)");
          fptr->tableptr = 0;  /* set a null table structure pointer */
          return(*status = ARRAY_TOO_BIG);
        }

    /* copy the table structure address to the fitsfile structure */
    fptr->tableptr = colptr; 

    /*  initialize the table field parameters */
    for (ii = 0; ii < tfield; ii++, colptr++)
    {
        colptr->ttype[0] = '\0';  /* null column name */
        colptr->tscale = 1.;
        colptr->tzero  = 0.;
        colptr->strnull[0] = ASCII_NULL_UNDEFINED;  /* null value undefined */
        colptr->tbcol = -1;          /* initialize to illegal value */
        colptr->tdatatype = -9999;   /* initialize to illegal value */
    }

    /*
      Initialize the fictitious heap starting address (immediately following
      the table data) and a zero length heap.  This is used to find the
      end of the table data when checking the fill values in the last block. 
      There is no special data following an ASCII table.
    */
    fptr->heapstart = rowlen * nrows;
    fptr->heapsize = 0;

    /* now search for the table column keywords and the END keyword */

    for (nspace = 0, ii = 8; 1; ii++)  /* infinite loop  */
    {
        ffgkyn(fptr, ii, name, value, comm, status);

        if (*status == END_OF_FILE)
        {
            ffpmsg("END keyword not found in ASCII table header (ffainit).");
            return(*status = NO_END);
        }
        else if (*status > 0)
            return(*status);

        else if (name[0] == 'T')   /* keyword starts with 'T' ? */
            ffgtbp(fptr, name, value, status); /* test if column keyword */

        else if (!strcmp(name, "END"))  /* is this the END keyword? */
            break;

        if (!name[0] && !value[0] && !comm[0])  /* a blank keyword? */
            nspace++;

        else
            nspace = 0;
    }

    /* test that all the required keywords were found and have legal values */
    colptr = fptr->tableptr;
    for (ii = 0; ii < tfield; ii++, colptr++)
    {
        tbcoln = colptr->tbcol;  /* the starting column number (zero based) */

        if (colptr->tdatatype == -9999)
        {
            ffkeyn("TFORM", ii+1, name, status);  /* construct keyword name */
            sprintf(message,"Required %s keyword not found (ffainit).", name);
            ffpmsg(message);
            return(*status = NO_TFORM);
        }

        else if (tbcoln == -1)
        {
            ffkeyn("TBCOL", ii+1, name, status); /* construct keyword name */
            sprintf(message,"Required %s keyword not found (ffainit).", name);
            ffpmsg(message);
            return(*status = NO_TBCOL);
        }

        else if (fptr->rowlength != 0 && 
                (tbcoln < 0 || tbcoln >= fptr->rowlength ) )
        {
            ffkeyn("TBCOL", ii+1, name, status);  /* construct keyword name */
            sprintf(message,"Value of %s keyword out of range: %d (ffainit).",
            name, tbcoln);
            ffpmsg(message);
            return(*status = BAD_TBCOL);
        }

        else if (fptr->rowlength != 0 && 
                 tbcoln + colptr->twidth > fptr->rowlength )
        {
            sprintf(message,"Column %d is too wide to fit in table (ffainit)",
            ii+1);
            ffpmsg(message);
            sprintf(message, " TFORM = %s and NAXIS1 = %ld",
                    colptr->tform, fptr->rowlength);
            ffpmsg(message);
            return(*status = COL_TOO_WIDE);
        }
    }

    /*
      now we know everything about the table; just fill in the parameters:
      the 'END' record is 80 bytes before the current position, minus
      any trailing blank keywords just before the END keyword.
    */
    fptr->headend = fptr->nextkey - (80 * (nspace + 1));
 
    /* the data unit begins at the beginning of the next logical block */
    fptr->datastart = ( (fptr->nextkey - 80) / 2880 + 1) * 2880;

    /* the next HDU begins in the next logical block after the data  */
    fptr->headstart[ fptr->curhdu + 1] = fptr->datastart +
         ( (rowlen * nrows + 2879) / 2880 * 2880 );

    /* reset next keyword pointer to the start of the header */
    fptr->nextkey = fptr->headstart[ fptr->curhdu ];

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffbinit(fitsfile *fptr,     /* I - FITS file pointer */
            int *status)        /* IO - error status     */
{
/*
  initialize the parameters defining the structure of a binary table 
*/
    int  ii, nspace;
    long nrows, rowlen, tfield, pcount, totalwidth, strrepeat;
    tcolumn *colptr;
    char name[FLEN_KEYWORD], value[FLEN_VALUE], comm[FLEN_COMMENT];
    char message[FLEN_ERRMSG];

    if (*status > 0)
        return(*status);

    fptr->hdutype = BINARY_TBL;  /* set that this is a binary table */
    fptr->headend = 2000000000;       /* temporarily set huge header size  */

    /* get table parameters and test that the header is valid: */
    if (ffgttb(fptr, &rowlen, &nrows, &pcount, &tfield, status) > 0)
       return(*status);

    fptr->rowlength = rowlen; /* store length of a row */
    fptr->tfield = tfield;   /* store number of table fields in each row */

    if (fptr->tableptr)
       free(fptr->tableptr); /* free memory for the old CHDU structure */

   /* mem for column structures ; space is initialized = 0  */
    colptr = (tcolumn *) calloc(tfield, sizeof(tcolumn) );
    if (!colptr)
    {
        ffpmsg
        ("malloc failed to get memory for FITS table descriptors (ffbinit)");
        fptr->tableptr = 0;  /* set a null table structure pointer */
        return(*status = ARRAY_TOO_BIG);
    }

    /* copy the table structure address to the fitsfile structure */
    fptr->tableptr = colptr; 

    /*  initialize the table field parameters */
    for (ii = 0; ii < tfield; ii++, colptr++)
    {
        colptr->ttype[0] = '\0';  /* null column name */
        colptr->tscale = 1.;
        colptr->tzero  = 0.;
        colptr->tnull  = NULL_UNDEFINED; /* (integer) null value undefined */
        colptr->tdatatype = -9999;   /* initialize to illegal value */
        colptr->trepeat = 1;
        colptr->strnull[0] = '\0'; /* for ASCII string columns (TFORM = rA) */
    }

    /*
      Initialize the heap starting address (immediately following
      the table data) and the size of the heap.  This is used to find the
      end of the table data when checking the fill values in the last block. 
    */
    fptr->heapstart = rowlen * nrows;
    fptr->heapsize = pcount;

    /* now search for the table column keywords and the END keyword */

    for (nspace = 0, ii = 8; 1; ii++)  /* infinite loop  */
    {
        ffgkyn(fptr, ii, name, value, comm, status);

        if (*status == END_OF_FILE)
        {
            ffpmsg("END keyword not found in binary table header (ffbinit).");
            return(*status = NO_END);
        }
        else if (*status > 0)
            return(*status);

        else if (name[0] == 'T')   /* keyword starts with 'T' ? */
            ffgtbp(fptr, name, value, status); /* test if column keyword */

        else if (!strcmp(name, "END"))  /* is this the END keyword? */
            break;


        if (!name[0] && !value[0] && !comm[0])  /* a blank keyword? */
            nspace++;

        else
            nspace = 0; /* reset number of consecutive spaces before END */
    }

    /* test that all the required keywords were found and have legal values */
    colptr = fptr->tableptr;  /* set pointer to first column */

    for (ii = 0; ii < tfield; ii++, colptr++)
    {
        if (colptr->tdatatype == -9999)
        {
            ffkeyn("TFORM", ii+1, name, status);  /* construct keyword name */
            sprintf(message,"Required %s keyword not found (ffbinit).", name);
            ffpmsg(message);
            return(*status = NO_TFORM);
        }
    }

    /*
      now we know everything about the table; just fill in the parameters:
      the 'END' record is 80 bytes before the current position, minus
      any trailing blank keywords just before the END keyword.
    */
    fptr->headend = fptr->nextkey - (80 * (nspace + 1));
 
    /* the data unit begins at the beginning of the next logical block */
    fptr->datastart = ( (fptr->nextkey - 80) / 2880 + 1) * 2880;

    /* the next HDU begins in the next logical block after the data  */
    fptr->headstart[ fptr->curhdu + 1] = fptr->datastart +
         ( (rowlen * nrows + pcount + 2879) / 2880 * 2880 );

    /* determine the byte offset to the beginning of each column */
    ffgtbc(fptr, &totalwidth, status);

    if (totalwidth != rowlen)
    {
        sprintf(message,
        "NAXIS1 = %ld is not equal to the sum of column widths: %ld", 
        rowlen, totalwidth);
        ffpmsg(message);
        *status = BAD_ROW_WIDTH;
    }

    /*
    FITSIO deals with ASCII columns as arrays of strings, not
    arrays of characters, so need to change the repeat count
    to indicate the number of strings in the field, not the 
    total number of characters in the field.
    */

    colptr = fptr->tableptr;  /* set pointer to first column */

    for (ii = 0; ii < tfield; ii++, colptr++)
    {
        if (colptr->tdatatype == TSTRING)
        {
            strrepeat = colptr->trepeat / colptr->twidth;
            colptr->trepeat = maxvalue(strrepeat, 1);
        }
    }

    /* reset next keyword pointer to the start of the header */
    fptr->nextkey = fptr->headstart[ fptr->curhdu ];

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgabc(int tfields,     /* I - number of columns in the table           */
           char **tform,    /* I - value of TFORMn keyword for each column  */
           int space,       /* I - number of spaces to leave between cols   */
           long *rowlen,    /* O - total width of a table row               */
           long *tbcol,     /* O - starting byte in row for each column     */
           int *status)     /* IO - error status                            */
/*
  calculate the starting byte offset of each column of an ASCII table
  and the total length of a row, in bytes.  The input space value determines
  how many blank spaces to leave between each column (1 is recommended).
*/
{
    int ii, datacode, decims;
    long width;

    if (*status > 0)
        return(*status);

    *rowlen=0;
    tbcol[0] = 1;

    for (ii = 0; ii < tfields; ii++)
    {
        tbcol[ii] = *rowlen + 1;    /* starting byte in row of column */

        ffasfm(tform[ii], &datacode, &width, &decims, status);

        *rowlen += (width + space);  /* total length of row */
    }

    *rowlen -= space;  /*  don't add space after the last field */

    return (*status);
}
/*--------------------------------------------------------------------------*/
int ffgtbc(fitsfile *fptr,    /* I - FITS file pointer          */
           long *totalwidth,  /* O - total width of a table row */
           int *status)       /* IO - error status              */
{
/*
  calculate the starting byte offset of each column of a binary table.
  Use the values of the datatype code and repeat counts in the
  column structure. Return the total length of a row, in bytes.
*/
    int tfields, ii;
    long nbytes;
    tcolumn *colptr;

    if (*status > 0)
        return(*status);

    if (fptr->datastart == DATA_UNDEFINED)
        if ( ffrdef(fptr, status) > 0)               /* rescan header */
            return(*status);

    tfields = fptr->tfield;
    colptr = fptr->tableptr;  /* point to first column structure */

    *totalwidth = 0;

    for (ii = 0; ii < tfields; ii++, colptr++)
    {
        colptr->tbcol = *totalwidth;  /* byte offset in row to this column */

        if (colptr->tdatatype == TSTRING)
        {
            nbytes = colptr->trepeat;   /* one byte per char */
        }
        else if (colptr->tdatatype == TBIT)
        {
            nbytes = (colptr->trepeat + 7) / 8;
        }
        else if (colptr->tdatatype > 0)
        {
            nbytes = colptr->trepeat * (colptr->tdatatype / 10);
        }
        else   /* this is a variable length descriptor (neg. tdatatype) */
            nbytes = 8;

        *totalwidth = *totalwidth + nbytes;
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgtbp(fitsfile *fptr,     /* I - FITS file pointer   */
           char *name,         /* I - name of the keyword */
           char *value,        /* I - value string of the keyword */
           int *status)        /* IO - error status       */
{
/*
  Get TaBle Parameter.  The input keyword name begins with the letter T.
  Test if the keyword is one of the table column definition keywords
  of an ASCII or binary table. If so, decode it and update the value 
  in the structure.
*/
    int tstatus, datacode, decimals;
    long width, repeat, nfield, ivalue;
    double dvalue;
    char tvalue[FLEN_VALUE];
    char message[FLEN_ERRMSG];
    tcolumn *colptr;

    if (*status > 0)
        return(*status);

    tstatus = 0;

    if (!strncmp(name, "TTYPE", 5) ||
        !strncmp(name, "TFORM", 5) ||
        !strncmp(name, "TBCOL", 5) ||
        !strncmp(name, "TSCAL", 5) ||
        !strncmp(name, "TZERO", 5) ||
        !strncmp(name, "TNULL", 5) ||
        !strncmp(name, "THEAP", 5) )
    {

    if( ffc2ii(&name[5], &nfield, &tstatus) > 0) /* read index no. */
        return(*status);    /* must not be an indexed keyword */

    if (nfield < 1 || nfield > fptr->tfield )  /* index out of range */
        return(*status);

    colptr = fptr->tableptr;        /* get pointer to columns */
    colptr = colptr + nfield - 1;   /* point to the correct column */

    if (!strncmp(name, "TTYPE", 5))
    {
        if (ffc2s(value, tvalue, &tstatus) > 0)  /* remove quotes */
            return(*status);

        strcpy(colptr->ttype, tvalue);  /* copy col name to structure */
    }

    else if (!strncmp(name, "TFORM", 5))
    {
        if (ffc2s(value, tvalue, &tstatus) > 0)  /* remove quotes */
            return(*status);

        strncpy(colptr->tform, tvalue, 9);  /* copy TFORM to structure */
        colptr->tform[9] = '\0';            /* make sure it is terminated */

        if (fptr->hdutype == ASCII_TBL)  /* ASCII table */
        {
          if (ffasfm(tvalue, &datacode, &width, &decimals, status) > 0)
              return(*status);  /* bad format code */

          colptr->tdatatype = TSTRING; /* store datatype code */
          colptr->trepeat = 1;      /* field repeat count == 1 */
          colptr->twidth = width;   /* the width of the field, in bytes */
        }
        else  /* binary table */
        {
          if (ffbnfm(tvalue, &datacode, &repeat, &width, status) > 0)
              return(*status);  /* bad format code */

          colptr->tdatatype = datacode; /* store datatype code */
          colptr->trepeat = repeat;     /* field repeat count  */
          colptr->twidth = width;   /*  width of an ASCII field in chars */
        }
    }

    else if (!strncmp(name, "TBCOL", 5))
    {
        if (fptr->hdutype == BINARY_TBL)
            return(*status);  /* binary tables don't have TBCOL keywords */

        if (ffc2ii(value, &ivalue, status) > 0)
        {
            sprintf(message,
            "Error reading value of %s as an integer: %s", name, value);
            ffpmsg(message);
            return(*status);
        }
        colptr->tbcol = ivalue - 1; /* convert to zero base */
    }
    else if (!strncmp(name, "TSCAL", 5))
    {
        if (ffc2dd(value, &dvalue, status) > 0)
        {
            sprintf(message,
            "Error reading value of %s as a double: %s", name, value);
            ffpmsg(message);
            return(*status);
        }
        colptr->tscale = dvalue;
    }
    else if (!strncmp(name, "TZERO", 5))
    {
        if (ffc2dd(value, &dvalue, status) > 0)
        {
            sprintf(message,
            "Error reading value of %s as a double: %s", name, value);
            ffpmsg(message);
            return(*status);
        }
        colptr->tzero = dvalue;
    }
    else if (!strncmp(name, "TNULL", 5))
    {
        if (fptr->hdutype == ASCII_TBL)  /* ASCII table */
        {
            if (ffc2s(value, tvalue, &tstatus) > 0)  /* remove quotes */
                return(*status);

            strncpy(colptr->strnull, tvalue, 17);  /* copy TNULL string */
            colptr->strnull[17] = '\0';  /* terminate the strnull field */
        }
        else  /* binary table */
        {
            if (ffc2ii(value, &ivalue, status) > 0) 
            {
                sprintf(message,
                "Error reading value of %s as an integer: %s", name, value);
                ffpmsg(message);
                return(*status);
            }
            colptr->tnull = ivalue; /* null value for integer column */
        }
    }

    else if (!strncmp(name, "THEAP", 5))
    {
        if (fptr->hdutype == ASCII_TBL)  /* ASCII table */
            return(*status);  /* ASCII tables don't have THEAPn keywords */ 

        if (ffc2ii(value, &ivalue, status) > 0) 
        {
            sprintf(message,
            "Error reading value of %s as an integer: %s", name, value);
            ffpmsg(message);
            return(*status);
        }
        fptr->heapstart = ivalue; /* the starting byte of the heap */
    }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgcpr( fitsfile *fptr, /* I - FITS file pointer                        */
        int colnum,     /* I - column number (1 = 1st column of table)      */
        long firstrow,  /* I - first row (1 = 1st row of table)             */
        long firstelem, /* I - first element within vector (1 = 1st)        */
        long nelem,     /* I - number of elements to read or write          */
        int writemode,  /* I - = 1 if writing data, = 0 if reading data     */
        double *scale,  /* O - FITS scaling factor (TSCALn keyword value)   */
        double *zero,   /* O - FITS scaling zero pt (TZEROn keyword value)  */
        char *tform,    /* O - ASCII column format: value of TFORMn keyword */
        long *twidth,   /* O - width of ASCII column (characters)           */
        int *tcode,     /* O - column datatype code: I*4=41, R*4=42, etc    */
        int *maxelem,   /* O - max number of elements that fit in buffer    */
        long *startpos, /* O - offset in file to starting row & column      */
        long *elemnum,  /* O - starting element number ( 0 = 1st element)   */
        long *incre,    /* O - byte offset between elements within a row    */
        long *repeat,   /* O - number of elements in a row (vector column)  */
        long *rowlen,   /* O - length of a row, in bytes                    */
        int  *hdutype,  /* O - HDU type: 0, 1, 2 = primary, table, bintable */
        long *tnull,    /* O - null value for integer columns               */
        char *snull,    /* O - null value for ASCII table columns           */
        int *status)    /* IO - error status                                */
/*
  Get Column PaRameters, and test starting row and element numbers for 
  validity.  
*/
{
    int  bytepix;
    long datastart, tbcol;
    char message[81];
    tcolumn *colptr;

    if (firstrow < 1)
    {
        sprintf(message, "Starting row number is out of range: %ld",
                firstrow);
        ffpmsg(message);
        return(*status = BAD_ROW_NUM);
    }

    if (nelem < 0)
    {
        sprintf(message, "Negative no. of elements to read or write: %ld",
                nelem);
        ffpmsg(message);
        return(*status = NEG_BYTES);
    }

    if (fptr->datastart == DATA_UNDEFINED)
        if ( ffrdef(fptr, status) > 0)               /* rescan header */
            return(*status);

    if (colnum < 1 || colnum > fptr->tfield)
    {
        sprintf(message, "Specified column number is out of range: %d",
                colnum);
        ffpmsg(message);
        return(*status = BAD_COL_NUM);
    }

    /*  copy relevant parameters from the structure */

    *hdutype = fptr->hdutype;
    *rowlen   = fptr->rowlength;
    datastart = fptr->datastart;

    colptr  = fptr->tableptr;   /* point to first column */
    colptr += (colnum - 1);     /* offset to correct column structure */

    *scale    = colptr->tscale;
    *zero     = colptr->tzero;
    *repeat   = colptr->trepeat;
    *tcode    = colptr->tdatatype;

    if (abs(*tcode) == TBIT)
    {
        /* interprete 'X' column as 'B' */
        *tcode = *tcode / TBIT * TBYTE;
        *repeat = (*repeat + 7) / 8;
    }

    *twidth   = colptr->twidth;
    *tnull    = colptr->tnull;
    tbcol     = colptr->tbcol;

    strcpy(tform, colptr->tform);
    strcpy(snull, colptr->strnull);

    /*---------------------------------*/
    /*   check starting element number */
    /*---------------------------------*/
    if (*hdutype == ASCII_TBL)
    {
        *elemnum = 0;   /* ASCII tables don't have vectors */
        *maxelem = 1;   /* just process one pixel at a time */
    }
    else
    {
      if (abs(*tcode) >= TCOMPLEX)
      {
        /* interprete complex and dbl complex as pairs of floats or doubles */
        if (*tcode > 0)
          *tcode = (*tcode + 1) / 2;
        else
          *tcode = (*tcode - 1) / 2;

        *repeat = *repeat * 2;
        *elemnum = (firstelem - 1) * 2;
      }
      else
      {
          *elemnum = firstelem - 1;
      }

      if (firstelem < 1)
      {
        sprintf(message, "Starting element number is out of range: %ld",
                firstelem);
        ffpmsg(message);
        return(*status = BAD_ELEM_NUM);
      }

      bytepix = abs(*tcode / 10);
      bytepix = maxvalue(bytepix, 1);  /* bytes per data pixel */
      *incre = bytepix;

      /* calculate no. of pixels that fit in buffer */
      /* allow for case where longs or floats are 8 bytes long */
      if (abs(*tcode) == TLONG)
      {
         *maxelem = DBUFFSIZE / sizeof(long);
      }
      if (abs(*tcode) == TFLOAT)
      {
         *maxelem = DBUFFSIZE / sizeof(float);
      }
      else
        *maxelem = DBUFFSIZE / bytepix; 
    }

    /*----------------------------------------------------*/
    /* calc starting byte position to 1st element of col  */
    /*  (this does not apply to variable length columns)  */
    /*----------------------------------------------------*/
    *startpos = datastart + ((firstrow - 1) * *rowlen) + tbcol;

    /*---------------------------------*/
    /*   check repeat count            */
    /*---------------------------------*/
    if (*hdutype == IMAGE_HDU && writemode) /*  Primary Array or IMAGE */
    { /*
        For primary arrays, set the repeat count greater than the total
        number of pixels to be written.  This prevents an out-of-range
        error message in cases where the final image array size is not
        yet known or defined.
      */
        *repeat = *elemnum + nelem; 
    }

    else if (*tcode > 0)     /*  Fixed length binary table column  */
    {
        if (*elemnum >= *repeat)
        {
            sprintf(message, "Starting element number is out of range: %ld",
                    firstelem);
            ffpmsg(message);
            return(*status = BAD_ELEM_NUM);
        }
        else if (*repeat == 1 && nelem > 1)
        { /*
            When accessing a scalar column, fool the calling routine into
            thinking that this is a vector column with very big elements.
            This allows multiple values (up to the maxelem number of elements
            that will fit in the buffer) to be read or written with a single
            routine call, which increases the efficiency.
          */           
            *incre = *rowlen;
            *repeat = nelem;
        }
    }

    else    /*  Variable length Binary Table column */
    {
      *tcode *= (-1);  

      if (writemode)     /* return next empty heap address for writing */
      {
        *repeat = nelem + *elemnum; /* total no. of elements in the field */

        /*  calculate starting position (for writing new data) in the heap */
        *startpos = datastart + fptr->heapstart + fptr->heapsize;

        /*  write the descriptor into the fixed length part of table */
        ffpdes(fptr, colnum, firstrow, *repeat, fptr->heapsize, status);

        /* increment the address to the next empty heap position */
        fptr->heapsize += (*repeat * (*incre)); 
      }
      else    /*  get the read start position in the heap */
      {
        ffgdes(fptr, colnum, firstrow, repeat, startpos, status);

        if (colptr->tdatatype == -TBIT)
            *repeat = (*repeat + 7) / 8;  /* convert from bits to bytes */

        if (*elemnum >= *repeat)
        {
            sprintf(message, "Starting element number is out of range: %ld",
                    firstelem);
            ffpmsg(message);
            return(*status = BAD_ELEM_NUM);
        }

        *startpos = *startpos + datastart + fptr->heapstart;
      }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffcopy(fitsfile *infptr,    /* I - FITS file pointer to input file  */
           fitsfile *outfptr,   /* I - FITS file pointer to output file */
           int morekeys,        /* I - reserve space in output header   */
           int *status)         /* IO - error status     */
/*
  copy the CHDU from infptr to the CHDU of outfptr.
  This will also allocate space in the output header for MOREKY keywords
*/
{
    int simple, bitpix, naxis, extend;
    int nkeys, nadd, ii;
    long naxes[99], pcount, gcount;
    char card[FLEN_CARD];

    if (*status > 0)
        return(*status);

    if (infptr == outfptr)
        return(*status = SAME_FILE);

    ffghsp(infptr, &nkeys, &nadd, status); /* get no. of keywords in header */

    if ( ( infptr->curhdu == 0 && outfptr->curhdu != 0 )  ||
         ( infptr->curhdu != 0 && outfptr->curhdu == 0 ) )
    {
        /* copying between primary array and image extension */

        /* get required keywords from input file */
        if (ffghpr(infptr, 99, &simple, &bitpix, &naxis, naxes, &pcount,
            &gcount, &extend, status) > 0)
            return(*status);

        simple = 1;
        extend = 1;

        /* write required keywords to output file */
        if (ffphpr(outfptr, simple, bitpix, naxis, naxes, pcount,
            gcount, extend, status) > 0)
            return(*status);

        /* copy remaining keywords, excluding pcount, gcount and extend */
        for (ii = naxis + 4; ii <= nkeys; ii++)
        {
            ffgrec(infptr, ii, card, status);
            if (strncmp(card, "PCOUNT  ", 8) && strncmp(card, "GCOUNT  ", 8)
            &&  strncmp(card, "EXTEND  ", 8) )
            {
                 ffprec(outfptr, card, status);
            }
        }
    }
    else
    {
        /* input and output HDUs are same type; simply copy all keywords */
        for (ii = 1; ii <= nkeys; ii++)
        {
            ffgrec(infptr,  ii, card, status);
            ffprec(outfptr, card, status);
        }
    }

    if (morekeys > 0)
      ffhdef(outfptr, morekeys, status); /* reserve space for more keywords */

    ffcpdt(infptr, outfptr, status);  /* now copy the data unit */

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffcpdt(fitsfile *infptr,    /* I - FITS file pointer to input file  */
           fitsfile *outfptr,   /* I - FITS file pointer to output file */
           int *status)         /* IO - error status     */
{
/*
  copy the data unit from the CHDU of infptr to the CHDU of outfptr. 
  This will overwrite any data already in the outfptr CHDU.
*/
    long nb, ii;
    char buffer[2880];

    if (*status > 0)
        return(*status);

    if (infptr == outfptr)
        return(*status = SAME_FILE);

    if (infptr->datastart == DATA_UNDEFINED)
        ffrdef(infptr, status);                /* rescan header */

    if (outfptr->datastart == DATA_UNDEFINED)
        ffrdef(outfptr, status);               /* rescan header */

    /* Calculate the number of bytes to be copied  */
    nb = (infptr->headstart[(infptr->curhdu) + 1] - infptr->datastart) / 2880;

    if (nb > 0)
    {
        /* move the initial copy position in each of the files */
        ffmbyt(infptr,  infptr->datastart,  REPORT_EOF, status);
        ffmbyt(outfptr, outfptr->datastart, IGNORE_EOF, status);

        for (ii = 0; ii < nb; ii++)
        {
            ffgbyt(infptr,  2880L, buffer, status); /* read input block */
            ffpbyt(outfptr, 2880L, buffer, status); /* write output block */
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffchdu(fitsfile *fptr,      /* I - FITS file pointer */
           int *status)         /* IO - error status     */
{
/*
  close the current HDU.  If we have write access to the file, then:
    - write the END keyword and pad header with blanks if necessary
    - check the data fill values, and rewrite them if not correct
*/
    long pcount;
    char comm[FLEN_COMMENT], message[FLEN_ERRMSG];

    if (fptr->writemode == 1)  /* write access to the file? */
    {
        /* if data has been written to variable length columns in a  */
        /* binary table, then we may need to update the PCOUNT value */
        if (fptr->heapsize > 0)
        {
          ffgkyj(fptr, "PCOUNT", &pcount, comm, status);
          if (fptr->heapsize > pcount)
             ffmkyj(fptr, "PCOUNT", fptr->heapsize, "&", status);
        }
        ffrdef(fptr, status);  /* scan header to redefine structure */
        ffpdfl(fptr, status);  /* insure correct data file values */
    }

    free(fptr->tableptr);      /* free memory for the CHDU structure */
    fptr->tableptr = 0;

    if (*status > 0)
    {
        sprintf(message,
        "Error while closing HDU number %d (ffchdu).", fptr->curhdu);
        ffpmsg(message);
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffrdef(fitsfile *fptr,      /* I - FITS file pointer */
           int *status)         /* IO - error status     */
/*
  ReDEFine the structure of a data unit.  This routine re-reads
  the CHDU header keywords to determine the structure and length of the
  current data unit.  This redefines the start of the next HDU.
*/
{
    int dummy;

    if (*status <= 0 && fptr->writemode == 1) /* write access to the file? */
        if (ffwend(fptr, status) <= 0)     /* rewrite END keyword and fill */
            ffrhdu(fptr, &dummy, status);  /* re-scan the header keywords  */

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffhdef(fitsfile *fptr,      /* I - FITS file pointer                    */
           int morekeys,        /* I - reserve space for this many keywords */
           int *status)         /* IO - error status                        */
/*
  based on the number of keywords which have already been written,
  plus the number of keywords to reserve space for, we then can
  define where the data unit should start (it must start at the
  beginning of a 2880-byte logical block).

  This routine will only have any effect if the starting location of the
  data unit following the header is not already defined.  In any case,
  it is always possible to add more keywords to the header even if the
  data has already been written.  It is just more efficient to reserve
  the space in advance.
*/
{
    if (*status > 0 || morekeys < 1)
        return(*status);

    if (fptr->datastart == DATA_UNDEFINED)
    {
      ffrdef(fptr, status);
      fptr->datastart = ((fptr->headend + (morekeys * 80)) / 2880 + 1) * 2880;
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpthp(fitsfile *fptr,      /* I - FITS file pointer */
           long theap,           /* I - starting addrss for the heap */
           int *status)         /* IO - error status     */
/*
  Define the starting address for the heap for a binary table.
  The default address is NAXIS1 * NAXIS2.  It is in units of
  bytes relative to the beginning of the regular binary table data.
  This routine also writes the appropriate THEAP keyword to the
  FITS header.
*/
{
    if (*status > 0 || theap < 1)
        return(*status);

    fptr->heapstart = theap;

    ffukyj(fptr, "THEAP", theap, "byte offset to heap area", status);

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpscl(fitsfile *fptr,      /* I - FITS file pointer               */
           double scale,        /* I - scaling factor: value of BSCALE */
           double zero,         /* I - zero point: value of BZERO      */
           int *status)         /* IO - error status                   */
/*
  Define the linear scaling factor for the primary array or image extension
  pixel values. This routine overrides the scaling values given by the
  BSCALE and BZERO keywords if present.  Note that this routine does not
  write or modify the BSCALE and BZERO keywords, but instead only modifies
  the values temporarily in the internal buffer.  Thus, a subsequent call to
  the ffrdef routine will reset the scaling back to the BSCALE and BZERO
  keyword values (or 1. and 0. respectively if the keywords are not present).
*/
{
    tcolumn *colptr;

    if (*status > 0)
        return(*status);

    if (scale == 0)
        return(*status = ZERO_SCALE);  /* zero scale value is illegal */

    if (fptr->datastart == DATA_UNDEFINED)
        if ( ffrdef(fptr, status) > 0)               /* rescan header */
            return(*status);

    if (fptr->hdutype != IMAGE_HDU)
        return(*status = NOT_IMAGE);         /* not proper HDU type */

    /* set pointer to the first 'column' (contains group parameters if any) */
    colptr = fptr->tableptr; 

    colptr++;   /* increment to the 2nd 'column' pointer  (the image itself) */

    colptr->tscale = scale;
    colptr->tzero = zero;

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpnul(fitsfile *fptr,      /* I - FITS file pointer                */
           long nulvalue,       /* I - null pixel value: value of BLANK */
           int *status)         /* IO - error status                    */
/*
  Define the value used to represent undefined pixels in the primary array or
  image extension. This only applies to integer image pixel (i.e. BITPIX > 0).
  This routine overrides the null pixel value given by the BLANK keyword
  if present.  Note that this routine does not write or modify the BLANK
  keyword, but instead only modifies the value temporarily in the internal
  buffer. Thus, a subsequent call to the ffrdef routine will reset the null
  value back to the BLANK  keyword value (or not defined if the keyword is not
  present).
*/
{
    tcolumn *colptr;

    if (*status > 0)
        return(*status);

    if (fptr->datastart == DATA_UNDEFINED)
        if ( ffrdef(fptr, status) > 0)               /* rescan header */
            return(*status);

    if (fptr->hdutype != IMAGE_HDU)
        return(*status = NOT_IMAGE);         /* not proper HDU type */

    /* set pointer to the first 'column' (contains group parameters if any) */
    colptr = fptr->tableptr; 

    colptr++;   /* increment to the 2nd 'column' pointer  (the image itself) */

    colptr->tnull = nulvalue;

    return(*status);
}
/*--------------------------------------------------------------------------*/
int fftscl(fitsfile *fptr,      /* I - FITS file pointer */
           int colnum,          /* I - column number to apply scaling to */
           double scale,        /* I - scaling factor: value of TSCALn   */
           double zero,         /* I - zero point: value of TZEROn       */
           int *status)         /* IO - error status     */
/*
  Define the linear scaling factor for the TABLE or BINTABLE extension
  column values. This routine overrides the scaling values given by the
  TSCALn and TZEROn keywords if present.  Note that this routine does not
  write or modify the TSCALn and TZEROn keywords, but instead only modifies
  the values temporarily in the internal buffer.  Thus, a subsequent call to
  the ffrdef routine will reset the scaling back to the TSCALn and TZEROn
  keyword values (or 1. and 0. respectively if the keywords are not present).
*/
{
    tcolumn *colptr;

    if (*status > 0)
        return(*status);

    if (scale == 0)
        return(*status = ZERO_SCALE);  /* zero scale value is illegal */

    if (fptr->datastart == DATA_UNDEFINED)
        if ( ffrdef(fptr, status) > 0)               /* rescan header */
            return(*status);

    if (fptr->hdutype == IMAGE_HDU)
        return(*status = NOT_TABLE);         /* not proper HDU type */

    colptr = fptr->tableptr;   /* set pointer to the first column */
    colptr += (colnum - 1);     /* increment to the correct column */

    colptr->tscale = scale;
    colptr->tzero = zero;

    return(*status);
}
/*--------------------------------------------------------------------------*/
int fftnul(fitsfile *fptr,      /* I - FITS file pointer                  */
           int colnum,          /* I - column number to apply nulvalue to */
           long nulvalue,       /* I - null pixel value: value of TNULLn  */
           int *status)         /* IO - error status                      */
/*
  Define the value used to represent undefined pixels in the BINTABLE column.
  This only applies to integer datatype columns (TFORM = B, I, or J).
  This routine overrides the null pixel value given by the TNULLn keyword
  if present.  Note that this routine does not write or modify the TNULLn
  keyword, but instead only modifies the value temporarily in the internal
  buffer. Thus, a subsequent call to the ffrdef routine will reset the null
  value back to the TNULLn  keyword value (or not defined if the keyword is not
  present).
*/
{
    tcolumn *colptr;

    if (*status > 0)
        return(*status);

    if (fptr->datastart == DATA_UNDEFINED)
        if ( ffrdef(fptr, status) > 0)               /* rescan header */
            return(*status);

    if (fptr->hdutype != BINARY_TBL)
        return(*status = NOT_BTABLE);         /* not proper HDU type */

 
    colptr = fptr->tableptr;   /* set pointer to the first column */
    colptr += (colnum - 1);    /* increment to the correct column */

    colptr->tnull = nulvalue;

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffsnul(fitsfile *fptr,      /* I - FITS file pointer                  */
           int colnum,          /* I - column number to apply nulvalue to */
           char *nulstring,     /* I - null pixel value: value of TNULLn  */
           int *status)         /* IO - error status                      */
/*
  Define the string used to represent undefined pixels in the ASCII TABLE
  column. This routine overrides the null  value given by the TNULLn keyword
  if present.  Note that this routine does not write or modify the TNULLn
  keyword, but instead only modifies the value temporarily in the internal
  buffer. Thus, a subsequent call to the ffrdef routine will reset the null
  value back to the TNULLn keyword value (or not defined if the keyword is not
  present).
*/
{
    tcolumn *colptr;

    if (*status > 0)
        return(*status);

    if (fptr->datastart == DATA_UNDEFINED)
        if ( ffrdef(fptr, status) > 0)               /* rescan header */
            return(*status);

    if (fptr->hdutype != ASCII_TBL)
        return(*status = NOT_ATABLE);         /* not proper HDU type */

 
    colptr = fptr->tableptr;   /* set pointer to the first column */
    colptr += (colnum - 1);    /* increment to the correct column */

    colptr->strnull[0] = '\0';
    strncat(colptr->strnull, nulstring, 19);  /* limit string to 19 chars */

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffwend(fitsfile *fptr,       /* I - FITS file pointer */
            int *status)         /* IO - error status     */
/*
  write the END card and following fill (space chars) in the current header
*/
{
    int ii;
    long endpos, nspace;
    char blankkey[FLEN_CARD];

    if (*status > 0)
        return(*status);

    endpos = fptr->headend;

    /*  calc the data starting position if not currently defined */
    if (fptr->datastart == DATA_UNDEFINED)
        fptr->datastart = ( endpos / 2880 + 1 ) * 2880;

    /* calculate the number of blank keyword slots in the header */
    nspace = ( fptr->datastart - endpos ) / 80;

    ffmbyt(fptr, endpos, IGNORE_EOF, status); /* move to header end */

    /* construct a blank keyword (80 spaces )  */
    strcpy(blankkey, "                                        ");
    strcat(blankkey, "                                        ");
     
    for (ii=0; ii < nspace; ii++)
        ffpbyt(fptr, 80, blankkey, status);  /* write the blank keyword */

    /*
    The END keyword must either be placed immediately after the last
    keyword that was written (as indicated by the headend value), or
    must be in the first 80 bytes of the 2880-byte FITS record immediately 
    preceeding the data unit, whichever is further in the file. The
    latter will occur if space has been reserved for more header keywords
    which have not yet been written.
    */

    endpos=maxvalue( endpos, ( fptr->datastart - 2880 ) );
    ffmbyt(fptr, endpos, REPORT_EOF, status);  /* move to END position */

    strncpy(blankkey, "END", 3);        /*  construct END keyword */
    ffpbyt(fptr, 80, blankkey, status); /*  write the END keyword to header */

    if (*status > 0)
        ffpmsg("Error while writing END card (ffwend).");

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpdfl(fitsfile *fptr,      /* I - FITS file pointer */
           int *status)         /* IO - error status     */
/*
  Write the Data Unit Fill values if they are not already correct.
  The fill values are used to fill out the last 2880 byte block of the HDU.
  Fill the data unit with zeros or blanks depending on the type of HDU
  from the end of the data to the end of the current FITS 2880 byte block
*/
{
    char chfill, fill[2880];
    long fillstart;
    int nfill, tstatus, ii;

    if (*status > 0)
        return(*status);

    if (fptr->heapstart == 0)
        return(*status);      /* null data unit, so there is no fill */

    fillstart = fptr->datastart + fptr->heapstart + fptr->heapsize;

    nfill = (fillstart + 2879) / 2880 * 2880 - fillstart;

    if (!nfill)
        return(*status);  /* return if there are no fill bytes to write */

    if (fptr->hdutype == ASCII_TBL)
        chfill = 32;         /* ASCII tables are filled with spaces */
    else
        chfill = 0;          /* all other extensions are filled with zeros */

    tstatus = 0;
    ffmbyt(fptr, fillstart, REPORT_EOF, &tstatus); /* move to fill area */
    ffgbyt(fptr, nfill, fill, &tstatus);           /* get the fill bytes */

    if (tstatus == 0)
    {
        for (ii = 0; ii < nfill; ii++)
        {
            if (fill[ii] != chfill)
                break;
        }

        if (ii == nfill)
            return(*status);   /* all the fill values were correct */
    }

    /* fill values are incorrect or have not been written, so write them */

    for (ii = 0; ii < nfill; ii++)
        fill[ii] = chfill;  /* fill the buffer with the correct fill value */

    ffmbyt(fptr, fillstart, IGNORE_EOF, status); /* move to fill area */
    ffpbyt(fptr, nfill, fill, status); /* write the fill bytes */

    if (*status > 0)
        ffpmsg("Error writing Data Unit fill bytes (ffpdfl).");

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffcrhd(fitsfile *fptr,      /* I - FITS file pointer */
           int *status)         /* IO - error status     */
/*
  CReate Header Data unit:
  Create, initialize, and move the i/o pointer to a new extension
  after the last known existing extension of the FITS file.
*/
{
    long bytepos;

    if (*status > 0)
        return(*status);

    else if (fptr->maxhdu == MAXHDU)
        *status = BAD_HDU_NUM;       /* too many HDUs in file */

    else if (ffchdu(fptr, status) <= 0)  /* close the current HDU */
    {
      bytepos = fptr->headstart[ fptr->maxhdu + 1 ];  /* last known HDU */
      ffmbyt(fptr, bytepos, REPORT_EOF, status);  /* move file ptr to it */

      fptr->maxhdu++;       /* increment the known number of HDUs */
      fptr->curhdu = fptr->maxhdu;      /* set current HDU location */
      fptr->nextkey = bytepos;          /* next keyword = start of header */
      fptr->headend = bytepos;          /* end of header */
      fptr->datastart = DATA_UNDEFINED; /* start of data unit undefined */
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffcrim(fitsfile *fptr,      /* I - FITS file pointer           */
           int bitpix,          /* I - bits per pixel              */
           int naxis,           /* I - number of axes in the array */
           long *naxes,         /* I - size of each axis           */
           int *status)         /* IO - error status               */
/*
  create an IMAGE extension following the current HDU. If the
  current HDU is empty (contains no header keywords), then simply
  write the required image (or primary array) keywords to the current
  HDU. 
*/
{
    if (*status > 0)
        return(*status);

    /* create new extension if current header is not empty */
    if (fptr->headend != fptr->headstart[fptr->curhdu] )
        ffcrhd(fptr, status);

    /* write the required header keywords */
    ffphpr(fptr, TRUE, bitpix, naxis, naxes, 0, 1, TRUE, status);

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffcrtb(fitsfile *fptr,  /* I - FITS file pointer                        */
           int tbltype,     /* I - type of table to create                  */
           long naxis2,     /* I - number of rows in the table              */
           int tfields,     /* I - number of columns in the table           */
           char **ttype,    /* I - name of each column                      */
           char **tform,    /* I - value of TFORMn keyword for each column  */
           char **tunit,    /* I - value of TUNITn keyword for each column  */
           char *extname,   /* I - value of EXTNAME keyword, if any         */
           int *status)     /* IO - error status                            */
/*
  Create a table extension in a FITS file. 
*/
{
    long naxis1, ncols, *tbcol;

    if (*status > 0)
        return(*status);

    /* create new extension if current header is not empty */
    if (fptr->headend != fptr->headstart[fptr->curhdu] )
        ffcrhd(fptr, status);

    if (tbltype == BINARY_TBL)
    {
      /* write the required header keywords. This will write PCOUNT = 0 */
      /* so variable length array columns are not supported             */
      ffphbn(fptr, naxis2, tfields, ttype, tform, tunit, extname, 0, status);
    }
    else if (tbltype == ASCII_TBL)
    {
      /* allocate mem for tbcol; malloc can have problems allocating small */
      /* arrays, so allocate at least 20 bytes */

      ncols = maxvalue(5, tfields);
      tbcol = (long *) calloc(ncols, sizeof(long));

      if (tbcol)
      {
        /* calculate width of a row and starting position of each column. */
        /* Each column will be separated by 1 blank space */
        ffgabc(tfields, tform, 1, &naxis1, tbcol, status);

        /* write the required header keywords */
        ffphtb(fptr, naxis1, naxis2, tfields, ttype, tbcol, tform, tunit,
               extname, status);

        free(tbcol);
      }
    }
    else
      *status = NOT_TABLE;

    return(*status);
}    
/*--------------------------------------------------------------------------*/
int ffiimg(fitsfile *fptr,      /* I - FITS file pointer           */
           int bitpix,          /* I - bits per pixel              */
           int naxis,           /* I - number of axes in the array */
           long *naxes,         /* I - size of each axis           */
           int *status)         /* IO - error status               */
/*
  insert an IMAGE extension following the current HDU 
*/
{
    int bytlen, nexthdu, ii;
    long npixels, datasize, newstart, nblocks;

    if (*status > 0)
        return(*status);

    if (bitpix == 8)
        bytlen = 1;
    else if (bitpix == 16)
        bytlen = 2;
    else if (bitpix == 32 || bitpix == -32)
        bytlen = 4;
    else if (bitpix == -64)
        bytlen = 8;
    else
        return(*status = BAD_BITPIX);  /* illegal bitpix value */

    if (naxis < 0 || naxis > 999)
        return(*status = BAD_NAXIS);

    /* calculate number of pixels in the image */
    if (naxis == 0)
        npixels = 0;
    else if (naxes[0] > 0)
        npixels = naxes[0];
    else
        return(*status = BAD_NAXES);

    for (ii = 1; ii < naxis; ii++)
        npixels = npixels * naxes[ii];

    datasize = npixels * bytlen;          /* size of image in bytes */
    nblocks = ((datasize + 2879) / 2880) + 1;  /* +1 for the header */

    if (fptr->writemode == READWRITE) /* must have write access */
    {   /* close the CHDU */
        ffrdef(fptr, status);  /* scan header to redefine structure */
        ffpdfl(fptr, status);  /* insure correct data file values */
    }
    else
        return(*status = READONLY_FILE);

    nexthdu = (fptr->curhdu) + 1; /* number of the next (new) hdu */
    newstart = fptr->headstart[nexthdu]; /* save starting addr of HDU */

    fptr->hdutype = IMAGE_HDU;  /* so that correct fill value is used */
    if (ffiblk(fptr, nblocks, 1, status) > 0)  /* insert the blocks */
        return(*status);

    (fptr->maxhdu)++;      /* increment known number of HDUs in the file */
    for (ii = fptr->maxhdu; ii > fptr->curhdu; ii--)
        fptr->headstart[ii + 1] = fptr->headstart[ii];  /* incre start addr */

    fptr->headstart[nexthdu] = newstart; /* set starting addr of HDU */

    /* set default parameters for this new empty HDU */
    fptr->curhdu = nexthdu;   /* we are now located at the next HDU */
    fptr->nextkey = fptr->headstart[nexthdu];  
    fptr->headend = fptr->headstart[nexthdu];
    fptr->datastart = (fptr->headstart[nexthdu]) + 2880;

    /* write the required header keywords */
    ffphpr(fptr, TRUE, bitpix, naxis, naxes, 0, 1, TRUE, status);

    /* redefine internal structure for this HDU */
    ffrdef(fptr, status);

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffitab(fitsfile *fptr,  /* I - FITS file pointer                        */
           long naxis1,     /* I - width of row in the table                */
           long naxis2,     /* I - number of rows in the table              */
           int tfields,     /* I - number of columns in the table           */
           char **ttype,    /* I - name of each column                      */
           long *tbcol,     /* I - byte offset in row to each column        */
           char **tform,    /* I - value of TFORMn keyword for each column  */
           char **tunit,    /* I - value of TUNITn keyword for each column  */
           char *extname,   /* I - value of EXTNAME keyword, if any         */
           int *status)     /* IO - error status                            */
/*
  insert an ASCII table extension following the current HDU 
*/
{
    int nexthdu, ii, nunit, nhead;
    long datasize, newstart, nblocks;

    if (*status > 0)
        return(*status);

    if (naxis1 < 0)
        return(*status = NEG_WIDTH);
    else if (naxis2 < 0)
        return(*status = NEG_ROWS);
    else if (tfields < 0 || tfields > 999)
        return(*status = BAD_TFIELDS);

    /* count number of optional TUNIT keywords to be written */
    nunit = 0;
    for (ii = 0; ii < tfields; ii++)
    {
        if (*tunit[ii])
            nunit++;
    }

    if (*extname)
         nunit++;     /* add one for the EXTNAME keyword */

    nhead = (9 + (3 * tfields) + nunit + 35) / 36;  /* no. of header blocks */
    datasize = naxis1 * naxis2;          /* size of table in bytes */
    nblocks = ((datasize + 2879) / 2880) + nhead;  /* size of HDU */

    if (fptr->writemode == READWRITE) /* must have write access */
    {   /* close the CHDU */
        ffrdef(fptr, status);  /* scan header to redefine structure */
        ffpdfl(fptr, status);  /* insure correct data file values */
    }
    else
        return(*status = READONLY_FILE);

    nexthdu = (fptr->curhdu) + 1; /* number of the next (new) hdu */
    newstart = fptr->headstart[nexthdu]; /* save starting addr of HDU */

    fptr->hdutype = ASCII_TBL;  /* so that correct fill value is used */
    if (ffiblk(fptr, nblocks, 1, status) > 0)  /* insert the blocks */
    {
       return(*status);
    }

    (fptr->maxhdu)++;      /* increment known number of HDUs in the file */
    for (ii = fptr->maxhdu; ii > fptr->curhdu; ii--)
        fptr->headstart[ii + 1] = fptr->headstart[ii];  /* incre start addr */

    fptr->headstart[nexthdu] = newstart; /* set starting addr of HDU */

    /* set default parameters for this new empty HDU */
    fptr->curhdu = nexthdu;   /* we are now located at the next HDU */
    fptr->nextkey = fptr->headstart[nexthdu];  
    fptr->headend = fptr->headstart[nexthdu];
    fptr->datastart = (fptr->headstart[nexthdu]) + (nhead * 2880);

    /* write the required header keywords */
    ffphtb(fptr, naxis1, naxis2, tfields, ttype, tbcol, tform, tunit,
           extname, status);

    /* redefine internal structure for this HDU */
    ffrdef(fptr, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffibin(fitsfile *fptr,  /* I - FITS file pointer                        */
           long naxis2,     /* I - number of rows in the table              */
           int tfields,     /* I - number of columns in the table           */
           char **ttype,    /* I - name of each column                      */
           char **tform,    /* I - value of TFORMn keyword for each column  */
           char **tunit,    /* I - value of TUNITn keyword for each column  */
           char *extname,   /* I - value of EXTNAME keyword, if any         */
           long pcount,     /* I - size of special data area (heap)         */
           int *status)     /* IO - error status                            */
/*
  insert a Binary table extension following the current HDU 
*/
{
    int nexthdu, ii, nunit, nhead, datacode;
    long naxis1, datasize, newstart, nblocks, repeat, width;

    if (*status > 0)
        return(*status);

    if (naxis2 < 0)
        return(*status = NEG_ROWS);
    else if (tfields < 0 || tfields > 999)
        return(*status = BAD_TFIELDS);

    /* count number of optional TUNIT keywords to be written */
    nunit = 0;
    for (ii = 0; ii < tfields; ii++)
    {
        if (*tunit[ii])
            nunit++;
    }

    if (*extname)
         nunit++;     /* add one for the EXTNAME keyword */

    nhead = (9 + (2 * tfields) + nunit + 35) / 36;  /* no. of header blocks */

    /* calculate total width of the table */
    naxis1 = 0;
    for (ii = 0; ii < tfields; ii++)
    {
        ffbnfm(tform[ii], &datacode, &repeat, &width, status);

        if (datacode == TBIT)
            naxis1 = naxis1 + ((repeat + 7) / 8);
        else if (datacode == TSTRING)
            naxis1 += repeat;
        else
            naxis1 = naxis1 + (repeat * width);
    }

    datasize = (naxis1 * naxis2) + pcount;         /* size of table in bytes */
    nblocks = ((datasize + 2879) / 2880) + nhead;  /* size of HDU */

    if (fptr->writemode == READWRITE) /* must have write access */
    {   /* close the CHDU */
        ffrdef(fptr, status);  /* scan header to redefine structure */
        ffpdfl(fptr, status);  /* insure correct data file values */
    }
    else
        return(*status = READONLY_FILE);

    nexthdu = (fptr->curhdu) + 1; /* number of the next (new) hdu */
    newstart = fptr->headstart[nexthdu]; /* save starting addr of HDU */

    fptr->hdutype = BINARY_TBL;  /* so that correct fill value is used */
    if (ffiblk(fptr, nblocks, 1, status) > 0)  /* insert the blocks */
        return(*status);

    (fptr->maxhdu)++;      /* increment known number of HDUs in the file */
    for (ii = fptr->maxhdu; ii > fptr->curhdu; ii--)
        fptr->headstart[ii + 1] = fptr->headstart[ii];  /* incre start addr */

    fptr->headstart[nexthdu] = newstart; /* set starting addr of HDU */

    /* set default parameters for this new empty HDU */
    fptr->curhdu = nexthdu;   /* we are now located at the next HDU */
    fptr->nextkey = fptr->headstart[nexthdu];  
    fptr->headend = fptr->headstart[nexthdu];
    fptr->datastart = (fptr->headstart[nexthdu]) + (nhead * 2880);

    /* write the required header keywords. This will write PCOUNT = 0 */
    /* so that the variable length data will be written at the right place */
    ffphbn(fptr, naxis2, tfields, ttype, tform, tunit, extname, pcount,
           status);

    /* redefine internal structure for this HDU (with PCOUNT = 0) */
    ffrdef(fptr, status);

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffdhdu(fitsfile *fptr,      /* I - FITS file pointer                   */
           int *hdutype,        /* O - type of the new CHDU after deletion */
           int *status)         /* IO - error status                       */
/*
  Delete the CHDU (as long as it is not the primary array).  Return the
  type of the new CHDU after the old CHDU is deleted.
*/
{
    long nblocks, ii;

    if (*status > 0)
        return(*status);

    if (fptr->curhdu == 0)
        return(*status = BAD_HDU_NUM);  /* cannot delete the primary array */

    ffchdu(fptr, status);  /* close the CHDU to free memeory */

    /* calc number of blocks to delete */
    nblocks = ( fptr->headstart[fptr->curhdu + 1] - 
                fptr->headstart[fptr->curhdu] ) / 2880;

    if (ffdblk(fptr, nblocks, status) > 0) /* delete the HDU */
        return(*status);

    /* delete the CHDU from the list of HDUs */
    for (ii = fptr->curhdu + 1; ii <= fptr->maxhdu; ii++)
        fptr->headstart[ii] = fptr->headstart[ii + 1];

    fptr->headstart[fptr->maxhdu + 1] = 0;
    (fptr->maxhdu)--; /* decrement the known number of HDUs */

    if (ffrhdu(fptr, hdutype, status) > 0)  /* initialize next HDU as CHDU */
    {
        /* failed (end of file?), so move back one HDU */
        *status = 0;
        ffcmsg();       /* clear extraneous error messages */
        ffgext(fptr, (fptr->curhdu) - 1, hdutype, status);
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffdblk(fitsfile *fptr,      /* I - FITS file pointer                    */
           long nblocks,        /* I - number of 2880-byte blocks to delete */
           int *status)         /* IO - error status                        */
/*
  Delete the specified number of 2880-byte blocks from the end
  of the CHDU by shifting all following extensions up this this
  number of blocks.
*/
{
    char buffer[2880];
    int tstatus, ii;
    long readpos, writepos;

    if (*status > 0)
        return(*status);

    tstatus = 0;

    /* pointers to the read and write positions */
    readpos = fptr->headstart[(fptr->curhdu) + 1];
    writepos = readpos - (nblocks * 2880);

    while ( !ffmbyt(fptr, readpos, REPORT_EOF, &tstatus) &&
            !ffgbyt(fptr, 2880L, buffer, &tstatus) )
    {
        ffmbyt(fptr, writepos, REPORT_EOF, status);
        ffpbyt(fptr, 2880L, buffer, status);

        if (*status > 0)
        {
           ffpmsg("Error deleting FITS blocks (ffdblk)");
           return(*status);
        }
        readpos  += 2880;  /* increment to next block to transfer */
        writepos += 2880;
    }

    /* now fill the last nblock blocks with zeros */
    for (ii = 0; ii < 2880; ii++)
         buffer[ii] = 0;

    ffmbyt(fptr, writepos, REPORT_EOF, status);

    for (ii = 0; ii < nblocks; ii++)
        ffpbyt(fptr, 2880L, buffer, status);

    /* recalculate the starting location of all subsequent HDUs */
    for (ii = fptr->curhdu; ii <= fptr->maxhdu; ii++)
         fptr->headstart[ii + 1] -= (2880 * nblocks);

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffmahd(fitsfile *fptr,      /* I - FITS file pointer             */
           int hdunum,          /* I - number of the HDU to move to  */
           int *exttype,        /* O - type of extension, 0, 1, or 2 */
           int *status)         /* IO - error status                 */
/*
  Move to Absolute Header Data unit.  Move to the specified HDU
  and read the header to initialize the table structure.  Note that extnum 
  is one based, so the primary array is extnum = 1.
*/
{
    int moveto, tstatus;
    char message[FLEN_ERRMSG];

    if (*status > 0)
        return(*status);
    else if (hdunum < 1 || hdunum >= MAXHDU )
        return(*status = BAD_HDU_NUM);

    while( (fptr->curhdu) + 1 != hdunum) /* at the correct HDU? */
    {
        /* move directly to the extension if we know that it exists,
        otherwise move to the highest known extension.  */
        
        moveto = minvalue(hdunum - 1, (fptr->maxhdu) + 1);


        if (fptr->headstart[moveto] < fptr->filesize )        /* test if HDU exists */
        {
            if (ffchdu(fptr, status) <= 0)  /* close out the current HDU */
            {
                if (ffgext(fptr, moveto, exttype, status) > 0)
                {   /* failed to get the requested extension */
                    tstatus = 0;
                    ffrhdu(fptr, exttype, &tstatus); /* restore the CHDU */
                }
            }
        }
        else
            *status = END_OF_FILE;

        if (*status > 0)
        {
            sprintf(message,
            "Failed to move to HDU number %d (ffmahd).", hdunum);
            ffpmsg(message);
            return(*status);
        }
    }

    *exttype = fptr->hdutype; /* return the type of HDU */

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffmrhd(fitsfile *fptr,      /* I - FITS file pointer                    */
           int hdumov,          /* I - rel. no. of HDUs to move by (+ or -) */ 
           int *exttype,        /* O - type of extension, 0, 1, or 2        */
           int *status)         /* IO - error status                        */
/*
  Move a Relative number of Header Data units.  Offset to the specified
  extension and read the header to initialize the HDU structure. 
*/
{
    int extnum;

    if (*status > 0)
        return(*status);

    extnum = fptr->curhdu + 1 + hdumov;  /* the absolute HDU number */
    ffmahd(fptr, extnum, exttype, status);  /* move to the HDU */

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgext(fitsfile *fptr,      /* I - FITS file pointer                */
           int hdunum,          /* I - no. of HDU to move get (0 based) */ 
           int *exttype,        /* O - type of extension, 0, 1, or 2    */
           int *status)         /* IO - error status                    */
/*
  Get Extension.  Move to the specified extension and initialize the
  HDU structure.
*/
{
    int xcurhdu, xmaxhdu;
    long xheadend;

    if (*status > 0)
        return(*status);

    if (ffmbyt(fptr, fptr->headstart[hdunum], REPORT_EOF, status) <= 0)
    {
        /* temporarily save current values, in case of error */
        xcurhdu = fptr->curhdu;
        xmaxhdu = fptr->maxhdu;
        xheadend = fptr->headend;

        /* set new parameter values */
        fptr->curhdu = hdunum;
        fptr->maxhdu = maxvalue(fptr->maxhdu, hdunum);
        fptr->headend = 2000000000; /* set header end to huge value for now */

        if (ffrhdu(fptr, exttype, status) > 0)
        {   /* failed to get the new HDU, so restore previous values */
            fptr->curhdu = xcurhdu;
            fptr->maxhdu = xmaxhdu;
            fptr->headend = xheadend;
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffiblk(fitsfile *fptr,      /* I - FITS file pointer               */
           long nblock,         /* I - no. of blocks to insert         */ 
           int headdata,        /* O - insert where? 0=header, 1=data  */
           int *status)         /* IO - error status                   */
/*
   insert 2880-byte blocks at the end of the current header or data unit
*/
{
    int tstatus;
    long ii, jj, insertpt;
    char charfill;
    char buff1[2880], buff2[2880];
    char *inbuff, *outbuff, *tmpbuff;

    if (*status > 0)
        return(*status);

    tstatus = *status;

    if (headdata == 0 || fptr->hdutype == ASCII_TBL)
        charfill = 32;  /* headers and ASCII tables have space (32) fill */
    else
        charfill = 0;   /* images and binary tables have zero fill */

    for (jj = 0; jj < nblock; jj++)  /* insert one block at a time */
    {

      if (headdata == 0)  
        insertpt = fptr->datastart;  /* insert just before data, or */
      else
        insertpt = fptr->headstart[fptr->curhdu + 1]; /* before next HDU */


      inbuff = buff1;   /* set pointers to input and output buffers */
      outbuff = buff2;

      for (ii = 0; ii < 2880; ii++)
        outbuff[ii] = charfill;  /* initialize output buffer with fill */

      ffmbyt(fptr, insertpt, REPORT_EOF, status);  /* move to 1st point */
      ffgbyt(fptr, 2880, inbuff, status);  /* read first block of bytes */

      while (*status != READ_ERROR)
      {
        ffmbyt(fptr, insertpt, REPORT_EOF, status);  /* insert point */
        ffpbyt(fptr, 2880, outbuff, status);  /* write the output buffer */

        if (*status > 0)
            return(*status);

        tmpbuff = inbuff;   /* swap input and output pointers */
        inbuff = outbuff;
        outbuff = tmpbuff;
        insertpt += 2880;  /* increment insert point by 1 block */

        ffmbyt(fptr, insertpt, REPORT_EOF, status);  /* move to next block */
        ffgbyt(fptr, 2880, inbuff, status);  /* read block of bytes */
      }

      *status = tstatus;  /* reset status value */
      ffmbyt(fptr, insertpt, REPORT_EOF, status); /* move back to insert pt */
      ffpbyt(fptr, 2880, outbuff, status);  /* write the final block */

      if (headdata == 0)
        fptr->datastart += 2880;  /* update data start address */

      for (ii = fptr->curhdu; ii <= fptr->maxhdu; ii++)
         fptr->headstart[ii + 1] += 2880; /* update following HDU addresses */
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffirow(fitsfile *fptr,  /* I - FITS file pointer                        */
           long firstrow,   /* I - insert space AFTER this row              */
                            /*     0 = insert space at beginning of table   */
           long nrows,      /* I - number of rows to insert                 */
           int *status)     /* IO - error status                            */
/*
 insert NROWS blank rows immediated after row firstrow (1 = first row).
 Set firstrow = 0 to insert space at the beginning of the table.
*/
{
    int tstatus;
    long naxis1, naxis2;
    long datasize, freespace, firstbyte, nshift, nblock, nbytes;
    char comm[FLEN_COMMENT];

    if (*status > 0)
        return(*status);

    if (fptr->hdutype == IMAGE_HDU)
    {
        ffpmsg("Can only add rows to TABLE or BINTABLE extension (ffirow)");
        return(*status = NOT_TABLE);
    }

    if (nrows < 0 )
        return(*status = NEG_BYTES);
    else if (nrows == 0)
        return(*status);   /* no op, so just return */

    ffgkyj(fptr, "NAXIS1", &naxis1, comm, status); /* get the current   */
    ffgkyj(fptr, "NAXIS2", &naxis2, comm, status); /* size of the table */

    if (firstrow > naxis2)
    {
        ffpmsg(
   "Insert position greater than the number of rows in the table (ffirow)");
        return(*status = BAD_ROW_NUM);
    }
    else if (firstrow < 0)
    {
        ffpmsg("Insert position is less than 0 (ffirow)");
        return(*status = BAD_ROW_NUM);
    }

    datasize = fptr->heapstart + fptr->heapsize;    /* current size of data */
    freespace = ( ( (datasize + 2879) / 2880) * 2880) - datasize;
    nshift = naxis1 * nrows;              /* no. of bytes to add to table */

    if ( (freespace - nshift) < 0)   /* not enough existing space? */
    {
        nblock = (nshift - freespace + 2879) / 2880;   /* number of blocks */
        ffiblk(fptr, nblock, 1, status);               /* insert the blocks */
    }

    firstbyte = naxis1 * firstrow;    /* relative insert position */
    nbytes = datasize - firstbyte;    /* no. of bytes to shift down */
    firstbyte += (fptr->datastart);   /* absolute insert position */

    ffshft(fptr, firstbyte, nbytes, nshift, status); /* shift rows and heap */

    /* update the heap starting address */
    fptr->heapstart += nshift;

    /* update the THEAP keyword if it exists */
    tstatus = 0;
    ffmkyj(fptr, "THEAP", fptr->heapstart, "&", &tstatus);

    /* update the NAXIS2 keyword */
    ffmkyj(fptr, "NAXIS2", naxis2 + nrows, "&", status);

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffdrow(fitsfile *fptr,  /* I - FITS file pointer                        */
           long firstrow,   /* I - first row to delete (1 = first)          */
           long nrows,      /* I - number of rows to delete                 */
           int *status)     /* IO - error status                            */
/*
 delete NROWS rows from table starting with firstrow (1 = first row of table).
*/
{
    int tstatus;
    long naxis1, naxis2;
    long datasize, freespace, firstbyte, nshift, nblock, nbytes;
    char comm[FLEN_COMMENT];

    if (*status > 0)
        return(*status);

    if (fptr->hdutype == IMAGE_HDU)
    {
        ffpmsg("Can only add rows to TABLE or BINTABLE extension (ffirow)");
        return(*status = NOT_TABLE);
    }

    if (nrows < 0 )
        return(*status = NEG_BYTES);
    else if (nrows == 0)
        return(*status);   /* no op, so just return */

    ffgkyj(fptr, "NAXIS1", &naxis1, comm, status); /* get the current   */
    ffgkyj(fptr, "NAXIS2", &naxis2, comm, status); /* size of the table */

    if (firstrow > naxis2)
    {
        ffpmsg(
   "Delete position greater than the number of rows in the table (ffdrow)");
        return(*status = BAD_ROW_NUM);
    }
    else if (firstrow < 1)
    {
        ffpmsg("Delete position is less than 1 (ffdrow)");
        return(*status = BAD_ROW_NUM);
    }
    else if (firstrow + nrows - 1 > naxis2)
    {
        ffpmsg("No. of rows to delete exceeds size of table (ffdrow)");
        return(*status = BAD_ROW_NUM);
    }

    nshift = naxis1 * nrows;           /* no. of bytes to delete from table */
    datasize = fptr->heapstart + fptr->heapsize;    /* current size of data */
    firstbyte = naxis1 * (firstrow + nrows - 1); /* relative delete position */
    nbytes = datasize - firstbyte;    /* no. of bytes to shift up */
    firstbyte += (fptr->datastart);   /* absolute delete position */
    ffshft(fptr, firstbyte, nbytes, (-1) * nshift, status); /* shift data */

    freespace = ( ( (datasize + 2879) / 2880) * 2880) - datasize;
    nblock = (nshift + freespace) / 2880;   /* number of blocks */

    if (nblock > 0) 
        ffdblk(fptr, nblock, status);  /* delete integral number blocks */

    /* update the heap starting address */
    fptr->heapstart -= nshift;

    /* update the THEAP keyword if it exists */
    tstatus = 0;
    ffmkyj(fptr, "THEAP", fptr->heapstart, "&", &tstatus);

    /* update the NAXIS2 keyword */
    ffmkyj(fptr, "NAXIS2", naxis2 - nrows, "&", status);

    return(*status);
}
/*--------------------------------------------------------------------------*/
int fficol(fitsfile *fptr,  /* I - FITS file pointer                        */
           int numcol,      /* I - position for new col. (1 = 1st)          */
           char *ttype,     /* I - name of column (TTYPE keyword)           */
           char *tform,     /* I - format of column (TFORM keyword)         */
           int *status)     /* IO - error status                            */
/*
 Insert a new column into an existing table at position numcol.  If
 numcol is greater than the number of existing columns in the table
 then the new column will be appended as the last column in the table.
*/
{
    char *name, *format;

    name = ttype;
    format = tform;

    fficls(fptr, numcol, 1, &name, &format, status);
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fficls(fitsfile *fptr,  /* I - FITS file pointer                        */
           int fstcol,      /* I - position for first new col. (1 = 1st)    */
           int ncols,       /* I - number of columns to insert              */
           char **ttype,    /* I - array of column names(TTYPE keywords)    */
           char **tform,    /* I - array of formats of column (TFORM)       */
           int *status)     /* IO - error status                            */
/*
 Insert 1 or more new columns into an existing table at position numcol.  If
 fstcol is greater than the number of existing columns in the table
 then the new column will be appended as the last column in the table.
*/
{
    int colnum, datacode, decims, tfields, tstatus, ii;
    long width, delbyte, repeat, naxis1, naxis2, datasize, freespace, nadd;
    long nblock, firstbyte, nbytes, tbcol;
    char tfm[FLEN_VALUE], keyname[FLEN_KEYWORD], comm[FLEN_COMMENT];
    tcolumn *colptr;

    if (*status > 0)
        return(*status);

    if (fptr->hdutype == IMAGE_HDU)
    {
       ffpmsg("Can only add columns to TABLE or BINTABLE extension (fficol)");
       return(*status = NOT_TABLE);
    }

    /*  is the column number valid?  */
    tfields = fptr->tfield;
    if (fstcol < 1 )
        return(*status = BAD_COL_NUM);
    else if (fstcol > tfields)
        colnum = tfields + 1;   /* append as last column */
    else
        colnum = fstcol;

    /* parse the tform value and calc number of bytes to add to each row */
    delbyte = 0;
    for (ii = 0; ii < ncols; ii++)
    {
        strcpy(tfm, tform[ii]);
        ffupch(tfm);         /* make sure format is in upper case */

        if (fptr->hdutype == ASCII_TBL)
        {
            ffasfm(tfm, &datacode, &width, &decims, status);
            delbyte += width + 1;  /*  add one space between the columns */
        }
        else
        {
            ffbnfm(tfm, &datacode, &repeat, &width, status);

            if (datacode == 1) /* bit column; round up to multiple of 8 bits */
            delbyte += (repeat + 7) / 8;

            else if (datacode == 16)  /* ASCII string column */
                delbyte += repeat;
            else                      /* numerical data type */
                delbyte += (datacode / 10) * repeat;
        }
    }

    if (*status > 0 || delbyte <= 0) /* also aborts on variable-length cols */
        return(*status);

    naxis1 = fptr->rowlength;          /* current width of the table */
    ffgkyj(fptr, "NAXIS2", &naxis2, comm, status); /* number of rows */

    datasize = fptr->heapstart + fptr->heapsize;    /* current size of data */
    freespace = ( ( (datasize + 2879) / 2880) * 2880) - datasize;
    nadd = delbyte * naxis2;                /* no. of bytes to add to table */

    if ( (freespace - nadd) < 0)   /* not enough existing space? */
    {
        nblock = (nadd - freespace + 2879) / 2880;     /* number of blocks  */
        if (ffiblk(fptr, nblock, 1, status) > 0)       /* insert the blocks */
            return(*status);
    }

    /* shift heap down (if it exists) */
    if (fptr->heapsize > 0)
    {
        nbytes = fptr->heapsize;    /* no. of bytes to shift down */
        firstbyte = fptr->datastart + fptr->heapstart; /* absolute heap pos */

        if (ffshft(fptr, firstbyte, nbytes, nadd, status) > 0) /* move heap */
            return(*status);

        /* update the heap starting address */
        fptr->heapstart += nadd;

        /* update the THEAP keyword if it exists */
        tstatus = 0;
        ffmkyj(fptr, "THEAP", fptr->heapstart, "&", &tstatus);
    }

    /* calculate byte position in the row where to insert the new column */
    if (colnum > tfields)
        firstbyte = naxis1;
    else
    {
        colptr = fptr->tableptr;
        colptr += (colnum - 1);
        firstbyte = colptr->tbcol;
    }

    /* insert delbyte bytes in every row, at byte position firstbyte */
    ffcins(fptr, naxis1, naxis2, delbyte, firstbyte, status);

    if (fptr->hdutype == ASCII_TBL)
    {
        /* adjust the TBCOL values of the existing columns */
        for(ii = 0; ii < tfields; ii++)
        {
            ffkeyn("TBCOL", ii + 1, keyname, status);
            ffgkyj(fptr, keyname, &tbcol, comm, status);
            if (tbcol > firstbyte)
            {
                tbcol += delbyte;
                ffmkyj(fptr, keyname, tbcol, "&", status);
            }
        }
    }

    /* update the mandatory keywords */
    ffmkyj(fptr, "TFIELDS", tfields + ncols, "&", status);
    ffmkyj(fptr, "NAXIS1", naxis1 + delbyte, "&", status);

    /* increment the index value on any existing column keywords */
    ffkshf(fptr, colnum, tfields, ncols, status);

    /* add the required keywords for the new columns */
    for (ii = 0; ii < ncols; ii++, colnum++)
    {
        strcpy(comm, "label for field");
        ffkeyn("TTYPE", colnum, keyname, status);
        ffpkys(fptr, keyname, ttype[ii], comm, status);

        strcpy(comm, "format of field");
        strcpy(tfm, tform[ii]);
        ffupch(tfm);         /* make sure format is in upper case */
        ffkeyn("TFORM", colnum, keyname, status);
        ffpkys(fptr, keyname, tfm, comm, status);

        if (fptr->hdutype == ASCII_TBL)   /* write the TBCOL keyword */
        {
            if (colnum == tfields + 1)
                tbcol = firstbyte + 2;  /* allow space between preceding column */
            else
                tbcol = firstbyte + 1;

            strcpy(comm, "beginning column of field");
            ffkeyn("TBCOL", colnum, keyname, status);
            ffpkyj(fptr, keyname, tbcol, comm, status);

            /* increment the column starting position for the next column */
            ffasfm(tfm, &datacode, &width, &decims, status);
            firstbyte += width + 1;  /*  add one space between the columns */
        }
    }

    ffrdef(fptr, status); /* initialize the new table structure */
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffdcol(fitsfile *fptr,  /* I - FITS file pointer                        */
           int colnum,      /* I - column to delete (1 = 1st)               */
           int *status)     /* IO - error status                            */
/*
  Delete a column from a table.
*/
{
    int ii, tstatus;
    long firstbyte, delbyte, nspace, naxis1, naxis2;
    long size, freespace, nblock, ndelete, nbytes, tbcol;
    char keyname[FLEN_KEYWORD], comm[FLEN_COMMENT];
    tcolumn *colptr, *nextcol;

    if (*status > 0)
        return(*status);

    if (fptr->hdutype == IMAGE_HDU)
    {
       ffpmsg
       ("Can only delete column from TABLE or BINTABLE extension (ffdcol)");
       return(*status = NOT_TABLE);
    }

    if (colnum < 1 || colnum > fptr->tfield )
        return(*status = BAD_COL_NUM);

    colptr = fptr->tableptr;
    colptr += (colnum - 1);
    firstbyte = colptr->tbcol;  /* starting byte position of the column */

    /* use column width to determine how many bytes to delete in each row */
    if (fptr->hdutype == ASCII_TBL)
    {
      delbyte = colptr->twidth;  /* width of ASCII column */

      if (colnum < fptr->tfield) /* check for space between next column */
      {
        nextcol = colptr + 1;
        nspace = (nextcol->tbcol) - (colptr->tbcol) - delbyte;
        if (nspace > 0)
            delbyte++;
      }
      else if (colnum > 1)   /* check for space between last 2 columns */
      {
        nextcol = colptr - 1;
        nspace = (colptr->tbcol) - (nextcol->tbcol) - (nextcol->twidth);
        if (nspace > 0)
        {
           delbyte++;
           firstbyte--;  /* delete the leading space */
        }
      }
    }
    else   /* a binary table */
    {
      if (colnum < fptr->tfield)
      {
         nextcol = colptr + 1;
         delbyte = (nextcol->tbcol) - (colptr->tbcol);
      }
      else
      {
         delbyte = (fptr->rowlength) - (colptr->tbcol);
      }
    }

    naxis1 = fptr->rowlength;          /* current width of the table */
    ffgkyj(fptr, "NAXIS2", &naxis2, comm, status); /* number of rows */

    size = fptr->heapstart + fptr->heapsize;  /* current size of table */
    freespace = (delbyte * naxis2) + ((size + 2879) / 2880) * 2880 - size;
    nblock = freespace / 2880;   /* number of empty blocks to delete */

    ffcdel(fptr, naxis1, naxis2, delbyte, firstbyte, status); /* delete col */

    /* shift heap up (if it exists) */
    if (fptr->heapsize > 0)
    {
      nbytes = fptr->heapsize;    /* no. of bytes to shift up */
      firstbyte = fptr->datastart + fptr->heapstart; /* absolute heap pos */
      ndelete = delbyte * naxis2; /* size of shift */

      if (ffshft(fptr, firstbyte, nbytes, -ndelete, status) > 0) /* mv heap */
          return(*status);

      /* update the heap starting address */
      fptr->heapstart -= ndelete;

      /* update the THEAP keyword if it exists */
      tstatus = 0;
      ffmkyj(fptr, "THEAP", fptr->heapstart, "&", &tstatus);
    }

    /* delete the empty  blocks at the end of the HDU */
    if (nblock > 0)
        ffdblk(fptr, nblock, status);

    if (fptr->hdutype == ASCII_TBL)
    {
      /* adjust the TBCOL values of the remaining columns */
      for (ii = 1; ii <= fptr->tfield; ii++)
      {
        ffkeyn("TBCOL", ii, keyname, status);
        ffgkyj(fptr, keyname, &tbcol, comm, status);
        if (tbcol > firstbyte)
        {
          tbcol = tbcol - delbyte;
          ffmkyj(fptr, keyname, tbcol, "&", status);
        }
      }
    }

    /* update the mandatory keywords */
    ffmkyj(fptr, "TFIELDS", (fptr->tfield) - 1, "&", status);        
    ffmkyj(fptr,  "NAXIS1",   naxis1 - delbyte, "&", status);

    /*
      delete the index keywords starting with 'T' associated with the 
      deleted column and subtract 1 from index of all higher keywords
    */
    ffkshf(fptr, colnum, fptr->tfield, -1, status);

    ffrdef(fptr, status);  /* initialize the new table structure */
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffcins(fitsfile *fptr,  /* I - FITS file pointer                        */
           long naxis1,     /* I - width of the table, in bytes             */
           long naxis2,     /* I - number of rows in the table              */
           long ninsert,    /* I - number of bytes to insert in each row    */
           long bytepos,    /* I - rel. position in row to insert bytes     */
           int *status)     /* IO - error status                            */
/*
 Insert 'ninsert' bytes into each row of the table at position 'bytepos'.
*/
{
    unsigned char buffer[10000], cfill;
    long newlen, fbyte, nbytes, ii, irow, nseg;

    if (*status > 0)
        return(*status);

    /* select appropriate fill value */
    if (fptr->hdutype == ASCII_TBL)
        cfill = 32;                     /* ASCII tables use blank fill */
    else
        cfill = 0;    /* primary array and binary tables use zero fill */

    newlen = naxis1 + ninsert;

    if (newlen <= 10000)
    {
       /*******************************************************************
       CASE #1: optimal case where whole new row fits in the work buffer
       *******************************************************************/

        for (ii = 0; ii < ninsert; ii++)
            buffer[ii] = cfill;      /* initialize buffer with fill value */

        /* first move the trailing bytes (if any) in the last row */
        fbyte = bytepos + 1;
        nbytes = naxis1 - bytepos;
        ffgtbb(fptr, naxis2, fbyte, nbytes, &buffer[ninsert], status);
        fptr->rowlength = newlen;  /* set row length to its new value */

        /* write the row (with leading fill bytes) in the new place */
        nbytes += ninsert;
        ffptbb(fptr, naxis2, fbyte, nbytes, buffer, status);
        fptr->rowlength = naxis1;  /* reset row length to original value */

        /*  now move the rest of the rows */
        for (irow = naxis2 - 1; irow > 0; irow--)
        {
            /* read the row to be shifted (work backwards thru the table) */
            ffgtbb(fptr, irow, fbyte, naxis1, &buffer[ninsert], status);
            fptr->rowlength = newlen;  /* set row length to its new value */

            /* write the row (with the leading fill bytes) in the new place */
            ffptbb(fptr, irow, fbyte, newlen, buffer, status);
            fptr->rowlength = naxis1; /* reset row length to original value */
        }
    }
    else
    {
        /*****************************************************************
        CASE #2:  whole row doesn't fit in work buffer; move row in pieces
        ******************************************************************
        first copy the data, then go back and write fill into the new column
        start by copying the trailing bytes (if any) in the last row.     */

        nbytes = naxis1 - bytepos;
        nseg = (nbytes + 9999) / 10000;
        fbyte = (nseg - 1) * 10000 + bytepos + 1;
        nbytes = naxis1 - fbyte + 1;

        for (ii = 0; ii < nseg; ii++)
        {
            ffgtbb(fptr, naxis2, fbyte, nbytes, buffer, status);
            fptr->rowlength = newlen;  /* set row length to its new value */

            ffptbb(fptr, naxis2, fbyte + ninsert, nbytes, buffer, status);
            fptr->rowlength = naxis1; /* reset row length to original value */

            fbyte -= 10000;
            nbytes = 10000;
        }

        /* now move the rest of the rows */
        nseg = (naxis1 + 9999) / 10000;
        for (irow = naxis2 - 1; irow > 0; irow--)
        {
          fbyte = (nseg - 1) * 10000 + bytepos + 1;
          nbytes = naxis1 - (nseg - 1) * 10000;
          for (ii = 0; ii < nseg; ii++)
          { 
            /* read the row to be shifted (work backwards thru the table) */
            ffgtbb(fptr, irow, fbyte, nbytes, buffer, status);
            fptr->rowlength = newlen;  /* set row length to its new value */

            /* write the row in the new place */
            ffptbb(fptr, irow, fbyte + ninsert, nbytes, buffer, status);
            fptr->rowlength = naxis1; /* reset row length to original value */

            fbyte -= 10000;
            nbytes = 10000;
          }
        }

        /* now write the fill values into the new column */
        nbytes = minvalue(ninsert, 10000);
        for (ii = 0; ii < nbytes; ii++)
            buffer[ii] = cfill;          /* initialize with fill value */

        nseg = (ninsert + 9999) / 10000;
        fptr->rowlength = newlen;  /* set row length to its new value */

        for (irow = 1; irow <= naxis2; irow++)
        {
          fbyte = bytepos + 1;
          nbytes = ninsert - ((nseg - 1) * 10000);
          for (ii = 0; ii < nseg; ii++)
          {
            ffptbb(fptr, irow, fbyte, nbytes, buffer, status);
            fbyte += nbytes;
            nbytes = 10000;
          }
        }
        fptr->rowlength = naxis1;  /* reset row length to original value */
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffcdel(fitsfile *fptr,  /* I - FITS file pointer                        */
           long naxis1,     /* I - width of the table, in bytes             */
           long naxis2,     /* I - number of rows in the table              */
           long ndelete,    /* I - number of bytes to delete in each row    */
           long bytepos,    /* I - rel. position in row to delete bytes     */
           int *status)     /* IO - error status                            */
/*
 delete 'ndelete' bytes from each row of the table at position 'bytepos'.
*/
{
    unsigned char buffer[10000];
    long newlen, i1, i2, ii, remain, nbytes, irow, nseg;

    if (*status > 0)
        return(*status);

    newlen = naxis1 - ndelete;

    if (newlen <= 10000)
    {
      /*******************************************************************
      CASE #1: optimal case where whole new row fits in the work buffer
      *******************************************************************/
      i1 = bytepos + 1;
      i2 = i1 + ndelete;
      for (irow = 1; irow < naxis2; irow++)
      {
        ffgtbb(fptr, irow, i2, newlen, buffer, status); /* read row */
        fptr->rowlength = newlen;  /* set row length to its new value */

        ffptbb(fptr, irow, i1, newlen, buffer, status); /* write row */
        fptr->rowlength = naxis1;  /* reset row length to original value */
      }

      /* now do the last row */
      remain = naxis1 - (bytepos + ndelete);

      if (remain > 0)
      {
        ffgtbb(fptr, naxis2, i2, remain, buffer, status); /* read row */
        fptr->rowlength = newlen;  /* set row length to its new value */

        ffptbb(fptr, naxis2, i1, remain, buffer, status); /* write row */
        fptr->rowlength = naxis1;  /* reset row length to original value */
      }
    }
    else
    {
        /*****************************************************************
        CASE #2:  whole row doesn't fit in work buffer; move row in pieces
        ******************************************************************/

        nseg = (newlen + 9999) / 10000;
        for (irow = 1; irow < naxis2; irow++)
        {
          i1 = bytepos + 1;
          i2 = i1 + ndelete;

          nbytes = newlen - (nseg - 1) * 10000;
          for (ii = 0; ii < nseg; ii++)
          { 
            ffgtbb(fptr, irow, i2, nbytes, buffer, status); /* read bytes */
            fptr->rowlength = newlen;  /* set row length to its new value */

            ffptbb(fptr, irow, i1, nbytes, buffer, status); /* rewrite bytes */
            fptr->rowlength = naxis1; /* reset row length to original value */

            i1 += nbytes;
            i2 += nbytes;
            nbytes = 10000;
          }
        }

        /* now do the last row */
        remain = naxis1 - (bytepos + ndelete);

        if (remain > 0)
        {
          nseg = (remain + 9999) / 10000;
          i1 = bytepos + 1;
          i2 = i1 + ndelete;
          nbytes = remain - (nseg - 1) * 10000;
          for (ii = 0; ii < nseg; ii++)
          { 
            ffgtbb(fptr, naxis2, i2, nbytes, buffer, status);
            fptr->rowlength = newlen;  /* set row length to its new value */

            ffptbb(fptr, naxis2, i1, nbytes, buffer, status); /* write row */
            fptr->rowlength = naxis1;  /* reset row length to original value */

            i1 += nbytes;
            i2 += nbytes;
            nbytes = 10000;
          }
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffkshf(fitsfile *fptr,  /* I - FITS file pointer                        */
           int colmin,      /* I - starting col. to be incremented; 1 = 1st */
           int colmax,      /* I - last column to be incremented            */
           int incre,       /* I - shift index number by this amount        */
           int *status)     /* IO - error status                            */
/*
  shift the index value on any existing column keywords
  This routine will modify the name of any keyword that begins with 'T'
  and has an index number in the range COLMIN - COLMAX, inclusive.

  if incre is positive, then the index values will be incremented.
  if incre is negative, then the kewords with index = COLMIN
  will be deleted and the index of higher numbered keywords will
  be decremented.
*/
{
    int nkeys, nmore, nrec, tstatus, i1;
    long ivalue;
    char rec[FLEN_CARD], q[FLEN_KEYWORD], newkey[FLEN_KEYWORD];

    ffghsp(fptr, &nkeys, &nmore, status);  /* get number of keywords */

    /* go thru header starting with the 9th keyword looking for 'TxxxxNNN' */

    for (nrec = 9; nrec <= nkeys; nrec++)
    {     
        ffgrec(fptr, nrec, rec, status);

        if (rec[0] == 'T')
        {
            i1 = 0;
            strncpy(q, &rec[1], 4);
            if (!strncmp(q, "BCOL", 4) || !strncmp(q, "FORM", 4) ||
                !strncmp(q, "TYPE", 4) || !strncmp(q, "SCAL", 4) ||
                !strncmp(q, "UNIT", 4) || !strncmp(q, "NULL", 4) ||
                !strncmp(q, "ZERO", 4) || !strncmp(q, "DISP", 4) ||
                !strncmp(q, "LMIN", 4) || !strncmp(q, "LMAX", 4) ||
                !strncmp(q, "DMIN", 4) || !strncmp(q, "DMAX", 4) ||
                !strncmp(q, "CTYP", 4) || !strncmp(q, "CRPX", 4) ||
                !strncmp(q, "CRVL", 4) || !strncmp(q, "CDLT", 4) ||
                !strncmp(q, "CROT", 4) || !strncmp(q, "CUNI", 4) )
              i1 = 5;
            else if (!strncmp(rec, "TDIM", 4) )
              i1 = 4;

            if (i1)
            {
              /* try reading the index number suffix */
              q[0] = '\0';
              strncat(q, &rec[i1], 8 - i1);

              tstatus = 0;
              ffc2ii(q, &ivalue, &tstatus);

              if (tstatus == 0 && ivalue >= colmin && ivalue <= colmax)
              {
                if (incre <= 0 && ivalue == colmin)       
                {
                  ffdrec(fptr, nrec, status); /* delete keyword */
                  nkeys = nkeys - 1;
                  nrec = nrec - 1;
                }
                else
                {
                  ivalue = ivalue + incre;
                  q[0] = '\0';
                  strncat(q, rec, i1);
     
                  ffkeyn(q, ivalue, newkey, status);
                  strncpy(rec, "        ", 8);    /* erase old keyword name */
                  i1 = strlen(newkey);
                  strncpy(rec, newkey, i1);   /* overwrite new keyword name */
                  ffmrec(fptr, nrec, rec, status);  /* modify the record */
                }
              }
            }
        }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffshft(fitsfile *fptr,  /* I - FITS file pointer                        */
           long firstbyte,  /* I - position of first byte in block to shift */
           long nbytes,     /* I - size of block of bytes to shift          */
           long nshift,     /* I - size of shift in bytes (+ or -)          */
           int *status)     /* IO - error status                            */
/*
    Shift block of bytes by nshift bytes (positive or negative).
    A positive nshift value moves the block down further in the file, while a
    negative value shifts the block towards the beginning of the file.
*/
{
    int ntodo, ntomov, ii;
    long ptr;
    char buffer[10000];

    if (*status > 0)
        return(*status);

    ntodo = nbytes;   /* total number of bytes to shift */

    if (nshift > 0)
            /* start at the end of the block and work backwards */
            ptr = firstbyte + nbytes;
    else
            /* start at the beginning of the block working forwards */
            ptr = firstbyte;

    while (ntodo)
    {
        /* number of bytes to move at one time */
        ntomov = minvalue(ntodo, 10000);

        if (nshift > 0)     /* if moving block down ... */
            ptr -= ntomov;

        /* move to position and read the bytes to be moved */
        ffmbyt(fptr, ptr, REPORT_EOF, status);
        ffgbyt(fptr, ntomov, buffer, status);

        /* move by shift amount and write the bytes */
        ffmbyt(fptr, ptr + nshift, IGNORE_EOF, status);
        if (ffpbyt(fptr, ntomov, buffer, status) > 0)
        {
           ffpmsg("Error while shifting block (ffshft)");
           return(*status);
        }

        ntodo -= ntomov;
        if (nshift < 0)     /* if moving block up ... */
            ptr += ntomov;
    }

    /* now overwrite the old data with fill */
    if (fptr->hdutype == ASCII_TBL)
    {
        for (ii = 0; ii < 10000; ii++)
            buffer[ii] = ' ';    /* fill ASCII tables with spaces */
    }
    else
    {
        for (ii = 0; ii < 10000; ii++)
            buffer[ii] = 0;      /* fill other HDUs with zeros */
    }

    if (nshift < 0)
    {
        ntodo = -nshift;
        /* point to the end of the shifted block */
        ptr = firstbyte + nbytes + nshift;
    }
    else
    {
        ntodo = nshift;
        /* point to original beginning of the block */
        ptr = firstbyte;
    }

    ffmbyt(fptr, ptr, REPORT_EOF, status);

    while (ntodo)
    {
        ntomov = minvalue(ntodo, 10000);
        ffpbyt(fptr, ntomov, buffer, status);
        ntodo -= ntomov;
    }
    return(*status);
}
/*------------------------------------------------------------------------*/
int ffcsum(fitsfile *fptr,      /* I - FITS file pointer                  */
           long nrec,           /* I - number of 2880-byte blocks to sum  */
           unsigned long *sum,  /* IO - accumulated checksum              */
           int *status)         /* IO - error status                      */
/*
    Calculate a 32-bit 1's complement checksum of the FITS 2880-byte blocks.
    This routine is based on the C algorithm developed by Rob
    Seaman at NOAO that was presented at the 1994 ADASS conference,  
    published in the Astronomical Society of the Pacific Conference Series.
    This uses a 32-bit 1's complement checksum in which the overflow bits
    are permuted back into the sum and therefore all bit positions are
    sampled evenly. 
*/
{
    long ii, jj;
    unsigned short sbuf[1440];
    unsigned long hi, lo, hicarry, locarry;

    if (*status > 0)
        return(*status);
  /*
    Sum the specified number of FITS 2880-byte records.  This assumes that
    the FITSIO file pointer points to the start of the records to be summed.
    Read each FITS block as 1440 short values (do byte swapping if needed).
  */
    for (jj = 0; jj < nrec; jj++)
    {

      ffgi2b(fptr, 1440, 2, (short *) sbuf, status);

      hi = (*sum >> 16);
      lo = *sum & 0xFFFF;

      for (ii = 0; ii < 1440; ii += 2)
      {
        hi += sbuf[ii];
        lo += sbuf[ii+1];
      }

      hicarry = hi >> 16;    /* fold carry bits in */
      locarry = lo >> 16;

      while (hicarry | locarry)
      {
        hi = (hi & 0xFFFF) + locarry;
        lo = (lo & 0xFFFF) + hicarry;
        hicarry = hi >> 16;
        locarry = lo >> 16;
      }

      *sum = (hi << 16) + lo;
    }
    return(*status);
}
/*-------------------------------------------------------------------------*/
void ffesum(unsigned long sum,  /* I - accumulated checksum                */
           int complm,          /* I - = 1 to encode complement of the sum */
           char *ascii)         /* O - 16-char ASCII encoded checksum      */
/*
    encode the 32 bit checksum by converting every 
    2 bits of each byte into an ASCII character (32 bit word encoded 
    as 16 character string).   Only ASCII letters and digits are used
    to encode the values (no ASCII punctuation characters).

    If complm=TRUE, then the complement of the sum will be encoded.

    This routine is based on the C algorithm developed by Rob
    Seaman at NOAO that was presented at the 1994 ADASS conference,
    published in the Astronomical Society of the Pacific Conference Series.
*/
{
    unsigned int exclude[13] = { 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40,
                                       0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60 };
    unsigned long mask[4] = { 0xff000000, 0xff0000, 0xff00, 0xff  };

    int offset = 0x30;     /* ASCII 0 (zero) */

    unsigned long value;
    int byte, quotient, remainder, ch[4], check, ii, jj, kk;
    char asc[32];

    if (complm)
        value = 0xFFFFFFFF - sum;   /* complement each bit of the value */
    else
        value = sum;

    for (ii = 0; ii < 4; ii++)
    {
        byte = (value & mask[ii]) >> (24 - (8 * ii));
        quotient = byte / 4 + offset;
        remainder = byte % 4;
        for (jj = 0; jj < 4; jj++)
            ch[jj] = quotient;

        ch[0] += remainder;

        for (check = 1; check;)   /* avoid ASCII  punctuation */
            for (check = 0, kk = 0; kk < 13; kk++)
                for (jj = 0; jj < 4; jj += 2)
                    if (ch[jj] == exclude[kk] || ch[jj+1] == exclude[kk])
                    {
                        ch[jj]++;
                        ch[jj+1]--;
                        check++;
                    }

        for (jj = 0; jj < 4; jj++)        /* assign the bytes */
            asc[4*jj+ii] = ch[jj];
    }

    for (ii = 0; ii < 16; ii++)       /* shift the bytes 1 to the right */
        ascii[ii] = asc[(ii+15)%16];

    ascii[16] = '\0';
}
/*-------------------------------------------------------------------------*/
unsigned long ffdsum(char *ascii,  /* I - 16-char ASCII encoded checksum   */
                     int complm,   /* I - =1 to decode complement of the   */
                     unsigned long *sum)  /* O - 32-bit checksum           */
/*
    decode the 16-char ASCII encoded checksum into an unsigned 32-bit long.
    If complm=TRUE, then the complement of the sum will be decoded.

    This routine is based on the C algorithm developed by Rob
    Seaman at NOAO that was presented at the 1994 ADASS conference,
    published in the Astronomical Society of the Pacific Conference Series.
*/
{
    char cbuf[16];
    unsigned long hi = 0, lo = 0, hicarry, locarry;
    int ii;

    /* remove the permuted FITS byte alignment and the ASCII 0 offset */
    for (ii = 0; ii < 16; ii++)
    {
        cbuf[ii] = ascii[(ii+1)%16];
        cbuf[ii] -= 0x30;
    }

    for (ii = 0; ii < 16; ii += 4)
    {
        hi += (cbuf[ii]   << 8) + cbuf[ii+1];
        lo += (cbuf[ii+2] << 8) + cbuf[ii+3];
    }

    hicarry = hi >> 16;
    locarry = lo >> 16;
    while (hicarry || locarry)
    {
        hi = (hi & 0xFFFF) + locarry;
        lo = (lo & 0xFFFF) + hicarry;
        hicarry = hi >> 16;
        locarry = lo >> 16;
    }

    *sum = (hi << 16) + lo;
    if (complm)
        *sum = 0xFFFFFFFF - *sum;   /* complement each bit of the value */

    return(*sum);
}
/*------------------------------------------------------------------------*/
int ffpcks(fitsfile *fptr,      /* I - FITS file pointer                  */
           int *status)         /* IO - error status                      */
/*
   Create or update the checksum keywords in the CHDU.  These keywords
   provide a checksum verification of the FITS HDU based on the ASCII
   coded 1's complement checksum algorithm developed by Rob Seaman at NOAO.
*/
{
    char datestr[9], checksum[FLEN_VALUE], datasum[FLEN_VALUE];
    char  comm[FLEN_COMMENT], chkcomm[FLEN_COMMENT], datacomm[FLEN_COMMENT];
    time_t tp;
    struct tm *ptr;
    int tstatus, chutype;
    long nrec;
    unsigned long dsum, olddsum, sum;
    double tdouble;

    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    /* generate current date string and construct the keyword comments */
    time(&tp);
    ptr = localtime(&tp);
    strftime(datestr, 9, "%d/%m/%y", ptr);
    strcpy(chkcomm, "encoded HDU checksum updated on ");
    strcat(chkcomm, datestr);
    strcpy(datacomm, "data unit checksum updated on ");
    strcat(datacomm, datestr);

    /* write the CHECKSUM keyword if it does not exist */
    tstatus = *status;
    if (ffgkys(fptr, "CHECKSUM", checksum, comm, status) == KEY_NO_EXIST)
    {
        *status = tstatus;
        strcpy(checksum, "0000000000000000");
        ffpkys(fptr, "CHECKSUM", checksum, chkcomm, status);
    }

    /* write the DATASUM keyword if it does not exist */
    tstatus = *status;
    if (ffgkys(fptr, "DATASUM", datasum, comm, status) == KEY_NO_EXIST)
    {
        *status = tstatus;
        olddsum = 0;
        ffpkys(fptr, "DATASUM", "         0", datacomm, status);

        /* set the CHECKSUM keyword as undefined, if it isn't already */
        if (strcmp(checksum, "0000000000000000") )
        {
            strcpy(checksum, "0000000000000000");
            ffmkys(fptr, "CHECKSUM", checksum, chkcomm, status);
        }
    }
    else
    {
        /* decode the datasum into an unsigned long variable */

        /* olddsum = strtoul(datasum, 0, 10); doesn't work on SUN OS */

        tdouble = atof(datasum);
        olddsum = tdouble;
    }

    /* close header: rewrite END keyword and following blank fill */
    if (ffwend(fptr, status) > 0)
        return(*status);

    /* now re-read the required keywords to determine the structure */
    if (ffrhdu(fptr, &chutype, status) > 0)
        return(*status);

    /* write the correct data fill values, if they are not already correct */
    if (ffpdfl(fptr, status) > 0)
        return(*status);

    /* calc size of data unit, in FITS 2880-byte blocks */
    nrec = (fptr->headstart[fptr->curhdu + 1] - fptr->datastart) / 2880;
    dsum = 0;

    if (nrec > 0)
    {
        /* accumulate the 32-bit 1's complement checksum */
        ffmbyt(fptr, fptr->datastart, REPORT_EOF, status);
        if (ffcsum(fptr, nrec, &dsum, status) > 0)
            return(*status);
    }

    if (dsum != olddsum)
    {
        /* update the DATASUM keyword with the correct value */ 
        sprintf(datasum, "%u", dsum);
        ffmkys(fptr, "DATASUM", datasum, datacomm, status);

        /* set the CHECKSUM keyword as undefined, if it isn't already */
        if (strcmp(checksum, "0000000000000000") )
        {
            strcpy(checksum, "0000000000000000");
            ffmkys(fptr, "CHECKSUM", checksum, chkcomm, status);
        }
    }        

    if (strcmp(checksum, "0000000000000000") )
    {
        /* check if CHECKSUM is still OK; move to the start of the header */
        ffmbyt(fptr, fptr->headstart[fptr->curhdu], REPORT_EOF, status);

        /* accumulate the header checksum into the previous data checksum */
        nrec = (fptr->datastart - fptr->headstart[fptr->curhdu]) / 2880;
        sum = dsum;
        if (ffcsum(fptr, nrec, &sum, status) > 0)
            return(*status);

        if (sum == 0 || sum == 0xFFFFFFFF)
           return(*status);            /* CHECKSUM is correct */

        /* Zero the CHECKSUM and recompute the new value */
        ffmkys(fptr, "CHECKSUM", "0000000000000000", chkcomm, status);
    }

    /* move to the start of the header */
    ffmbyt(fptr, fptr->headstart[fptr->curhdu], REPORT_EOF, status);

    /* accumulate the header checksum into the previous data checksum */
    nrec = (fptr->datastart - fptr->headstart[fptr->curhdu]) / 2880;
    sum = dsum;
    if (ffcsum(fptr, nrec, &sum, status) > 0)
           return(*status);

    /* encode the COMPLEMENT of the checksum into a 16-character string */
    ffesum(sum, TRUE, checksum);

    /* update the CHECKSUM keyword value with the new string */
    ffmkys(fptr, "CHECKSUM", checksum, "&", status);

    return(*status);
}
/*------------------------------------------------------------------------*/
int ffupck(fitsfile *fptr,      /* I - FITS file pointer                  */
           int *status)         /* IO - error status                      */
/*
   Update the CHECKSUM keyword value.  This assumes that the DATASUM
   keyword exists and has the correct value.
*/
{
    char datestr[9], chkcomm[FLEN_COMMENT], comm[FLEN_COMMENT];
    char checksum[FLEN_VALUE], datasum[FLEN_VALUE];
    time_t tp;
    struct tm *ptr;
    int tstatus;
    long nrec;
    unsigned long sum, dsum;
    double tdouble;

    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    /* generate current date string and construct the keyword comments */
    time(&tp);
    ptr = localtime(&tp);
    strftime(datestr, 9, "%d/%m/%y", ptr);
    strcpy(chkcomm, "encoded HDU checksum updated on ");
    strcat(chkcomm, datestr);

    /* get the DATASUM keyword and convert it to a unsigned long */
    if (ffgkys(fptr, "DATASUM", datasum, comm, status) == KEY_NO_EXIST)
    {
        ffpmsg("DATASUM keyword not found (ffupck");
        return(*status);
    }

    tdouble = atof(datasum); /* read as a double as a workaround */
    dsum = tdouble;

    /* get the checksum keyword, if it exists */
    tstatus = *status;
    if (ffgkys(fptr, "CHECKSUM", checksum, comm, status) == KEY_NO_EXIST)
    {
        *status = tstatus;
        strcpy(checksum, "0000000000000000");
        ffpkys(fptr, "CHECKSUM", checksum, chkcomm, status);
    }
    else
    {
        /* check if CHECKSUM is still OK */
        /* rewrite END keyword and following blank fill */
        if (ffwend(fptr, status) > 0)
            return(*status);

        /* move to the start of the header */
        ffmbyt(fptr, fptr->headstart[fptr->curhdu], REPORT_EOF, status);

        /* accumulate the header checksum into the previous data checksum */
        nrec = (fptr->datastart - fptr->headstart[fptr->curhdu]) / 2880;
        sum = dsum;
        if (ffcsum(fptr, nrec, &sum, status) > 0)
           return(*status);

        if (sum == 0 || sum == 0xFFFFFFFF)
           return(*status);    /* CHECKSUM is already correct */

        /* Zero the CHECKSUM and recompute the new value */
        ffmkys(fptr, "CHECKSUM", "0000000000000000", chkcomm, status);
    }

    /* move to the start of the header */
    ffmbyt(fptr, fptr->headstart[fptr->curhdu], REPORT_EOF, status);

    /* accumulate the header checksum into the previous data checksum */
    nrec = (fptr->datastart - fptr->headstart[fptr->curhdu]) / 2880;
    sum = dsum;
    if (ffcsum(fptr, nrec, &sum, status) > 0)
           return(*status);

    /* encode the COMPLEMENT of the checksum into a 16-character string */
    ffesum(sum, TRUE, checksum);

    /* update the CHECKSUM keyword value with the new string */
    ffmkys(fptr, "CHECKSUM", checksum, "&", status);

    return(*status);
}
/*------------------------------------------------------------------------*/
int ffvcks(fitsfile *fptr,      /* I - FITS file pointer                  */
           int *datastatus,     /* O - data checksum status               */
           int *hdustatus,      /* O - hdu checksum status                */
                                /*     1  verification is correct         */
                                /*     0  checksum keyword is not present */
                                /*    -1 verification not correct         */
           int *status)         /* IO - error status                      */
/*
    Verify the HDU by comparing the value of the computed checksums against
    the values of the DATASUM and CHECKSUM keywords if they are present.
*/
{
    int tstatus;
    double tdouble;
    unsigned long datasum, hdusum, olddatasum;
    char chksum[FLEN_VALUE], comm[FLEN_COMMENT];

    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    *datastatus = -1;
    *hdustatus  = -1;

    tstatus = *status;
    if (ffgkys(fptr, "CHECKSUM", chksum, comm, status) == KEY_NO_EXIST)
    {
        *hdustatus = 0;             /* CHECKSUM keyword does not exist */
        *status = tstatus;
    }

    if (ffgkys(fptr, "DATASUM", chksum, comm, status) == KEY_NO_EXIST)
    {
        *datastatus = 0;            /* DATASUM keyword does not exist */
        *status = tstatus;
    }

    if ( *status > 0 || (!(*hdustatus) && !(*datastatus)) )
        return(*status);            /* return if neither keywords exist */

    /* convert string to unsigned long */

    /* olddatasum = strtoul(chksum, 0, 10);  doesn't work w/ gcc on SUN OS */
    /* sscanf(chksum, "%u", &olddatasum);   doesn't work w/ cc on VAX/VMS */

    tdouble = atof(chksum); /* read as a double as a workaround */
    olddatasum = tdouble;

    /*  calculate the data checksum and the HDU checksum */
    if (ffgcks(fptr, &datasum, &hdusum, status) > 0)
        return(*status);

    if (*datastatus)
        if (datasum == olddatasum)
            *datastatus = 1;

    if (*hdustatus)
        if (hdusum == 0 || hdusum == 0xFFFFFFFF)
            *hdustatus = 1;

    return(*status);
}
/*------------------------------------------------------------------------*/
int ffgcks(fitsfile *fptr,           /* I - FITS file pointer             */
           unsigned long *datasum,   /* O - data checksum                 */
           unsigned long *hdusum,    /* O - hdu checksum                  */
           int *status)              /* IO - error status                 */

    /* calculate the checksums of the data unit and the total HDU */
{
    long nrec;

    if (*status > 0)           /* inherit input status value if > 0 */
        return(*status);

    nrec = (fptr->headstart[fptr->curhdu + 1] - fptr->datastart) / 2880;
    *datasum = 0;

    if (nrec > 0)
    {
        /* accumulate the 32-bit 1's complement checksum */
        ffmbyt(fptr, fptr->datastart, REPORT_EOF, status);
        if (ffcsum(fptr, nrec, datasum, status) > 0)
            return(*status);
    }

    /* move to the start of the header and calc. size of header */
    ffmbyt(fptr, fptr->headstart[fptr->curhdu], REPORT_EOF, status);
    nrec = (fptr->datastart - fptr->headstart[fptr->curhdu]) / 2880;

    /* accumulate the header checksum into the previous data checksum */
    *hdusum = *datasum;
    ffcsum(fptr, nrec, hdusum, status);

    return(*status);
}

