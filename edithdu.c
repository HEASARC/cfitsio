/*  This file, edithdu.c, contains the FITSIO routines related to       */
/*  copying, inserting, or deleting HDUs in a FITS file                 */

/*  The FITSIO software was written by William Pence at the High Energy    */
/*  Astrophysic Science Archive Research Center (HEASARC) at the NASA      */
/*  Goddard Space Flight Center.  Users shall not, without prior written   */
/*  permission of the U.S. Government,  establish a claim to statutory     */
/*  copyright.  The Government and others acting on its behalf, shall have */
/*  a royalty-free, non-exclusive, irrevocable,  worldwide license for     */
/*  Government purposes to publish, distribute, translate, copy, exhibit,  */
/*  and perform such material.                                             */

#include <string.h>
#include "fitsio2.h"
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

    /* check that the output header is empty */
    ffghsp(outfptr, &nkeys, &nadd, status); /* get no. of keywords in header */
    if (nkeys != 0)
    {
        ffpmsg("Cannot copy HDU to a non-empty HDU (ffcopy)");
        return(*status = HEADER_NOT_EMPTY);
    }

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
    /* ffiblk also increments headstart for all following HDUs */
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
           char *extnm,   /* I - value of EXTNAME keyword, if any         */
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

    if (*extnm)
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
    /* ffiblk also increments headstart for all following HDUs */
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

    /* write the required header keywords */

    ffphtb(fptr, naxis1, naxis2, tfields, ttype, tbcol, tform, tunit,
           extnm, status);

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
           char *extnm,   /* I - value of EXTNAME keyword, if any         */
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

    if (*extnm)
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
    /* ffiblk also increments headstart for all following HDUs */
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
    ffphbn(fptr, naxis2, tfields, ttype, tform, tunit, extnm, pcount,
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

    /* ffdblk also updates the starting address of all following HDUs */
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

