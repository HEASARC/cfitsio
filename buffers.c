/*  This file, buffers.c, contains the core set of FITSIO routines         */
/*  that use or manage the internal set of IO buffers.                     */

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

char iobuffer[NIOBUF][IOBUFLEN];      /* initialize to zero by default  */
fitsfile *bufptr[NIOBUF];             /* initialize to zero by default  */
long bufrecnum[NIOBUF];               /* initialize to zero by default  */
int dirty[NIOBUF], ageindex[NIOBUF];  /* ages get initialized in ffwhbf */

#if MACHINE == ALPHAVMS
  static float testfloat = TESTFLOAT;  /* use to test floating pt format */
#endif

/*--------------------------------------------------------------------------*/
int ffopen(fitsfile **fptr,      /* O - FITS file pointer                   */ 
           const char *filename, /* I - name of file to open                */
           int readwrite,        /* I - 0 = open readonly; 1 = read/write   */
           int *status)          /* IO - error status                       */
/*
  Open an existing FITS file with either readonly or read/write access.
*/
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

          (*fptr)->filesize = filesize;          /* physical file size */
          (*fptr)->logfilesize = filesize;       /* logical file size */
          (*fptr)->writemode = readwrite;        /* read-write mode    */
          (*fptr)->datastart = DATA_UNDEFINED;   /* unknown start of data */
          (*fptr)->curbuf = -1;   /* undefined current IO buffer */

          ffldrc(*fptr, 0, REPORT_EOF, status);     /* load first record */
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
/*
  Create and initialize a new FITS file.
*/
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
          (*fptr)->curbuf = -1;   /* undefined current IO buffer */
          ffldrc(*fptr, 0, IGNORE_EOF, status);  /* initialize first record */
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

    /* if null pointer, then file is not opened, so just return */
    if (!fptr)
        return(*status);

    ffchdu(fptr, status);           /* close and flush the current HDU */
    ffflsh(fptr, TRUE, status);     /* flush and disassociate IO buffers */

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

    /* if null pointer, then file is not opened, so just return */
    if (!fptr)
        return(*status);

    ffchdu(fptr, &tstatus);    /* close the current HDU, ignore any errors */
    ffflsh(fptr, TRUE, &tstatus);     /* flush and disassociate IO buffers */

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

    ffflsh(fptr, FALSE, status);  /* flush any modified IO buffers to disk */

    if (ffgext(fptr, hdunum - 1, &hdutype, status) > 0) /* reopen HDU */
        ffpmsg("ffflus could not reopen the current HDU.");

    return(*status);
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
    long record;
    extern long bufrecnum[NIOBUF];

    if (*status > 0)
       return(*status);

    if (bytepos < 0)
        return(*status = NEG_FILE_POS);

    record = bytepos / IOBUFLEN;  /* zero-indexed record number */

    /* if this is not the current record, then load it */
    if (record != bufrecnum[fptr->curbuf]) 
        ffldrc(fptr, record, err_mode, status);
    
    if (*status <= 0)
        fptr->bytepos = bytepos;  /* save new file position */

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpbyt(fitsfile *fptr,   /* I - FITS file pointer                    */
           int nbytes,       /* I - number of bytes to write             */
           void *buffer,     /* I - buffer containing the bytes to write */
           int *status)      /* IO - error status                        */
/*
  put (write) the buffer of bytes to the output FITS file, starting at
  the current file position.  Write large blocks of data directly to disk;
  write smaller segments to intermediate IO buffers to improve efficiency.
*/
{
    int ii, nbuff;
    long filepos, recstart, recend;
    long ntodo, bufpos, nspace, nwrite;
    char *cptr;

    extern fitsfile *bufptr[NIOBUF];
    extern char iobuffer[NIOBUF][IOBUFLEN];
    extern long bufrecnum[NIOBUF]; 
    extern int dirty[NIOBUF];

    if (*status > 0)
       return(*status);

    cptr = buffer;
    ntodo =  nbytes;

    if (nbytes >= MINDIRECT)
    {
      /* write large blocks of data directly to disk instead of via buffers */
      /* first, fill up the current IO buffer before flushing it to disk */

      nbuff = fptr->curbuf;          /* current IO buffer number */
      filepos = fptr->bytepos;       /* save the write starting position */
      recstart = bufrecnum[nbuff];                 /* starting record */
      recend = (filepos + nbytes - 1) / IOBUFLEN;  /* ending record   */

      /* bufpos is the starting position within the IO buffer */
      bufpos = filepos - (recstart * IOBUFLEN);
      nspace = IOBUFLEN - bufpos;   /* amount of space left in the buffer */

      if (nspace)
      { /* fill up the IO buffer */
        memcpy(iobuffer[nbuff] + bufpos, cptr, nspace);
        ntodo -= nspace;           /* decrement remaining number of bytes */
        cptr += nspace;            /* increment user buffer pointer */
        filepos += nspace;         /* increment file position pointer */
        dirty[nbuff] = TRUE;       /* mark record as having been modified */
      }

      for (ii = 0; ii < NIOBUF; ii++) /* flush any affected buffers to disk */
      {
        if (bufptr[ii] == fptr && bufrecnum[ii] >= recstart
            && bufrecnum[ii] <= recend )
        {
          if (dirty[ii])        /* flush modified buffer to disk */
             ffbfwt(ii, status);

          bufptr[ii] = NULL;  /* disassociate buffer from the file */
        }
      }

      /* move to the correct write position; must seek if last op was a read */
      if (fptr->io_pos != filepos || fptr->last_io_op == IO_READ)
         ffseek(fptr->fileptr, filepos);

      nwrite = ((ntodo - 1) / IOBUFLEN) * IOBUFLEN; /* don't write last buff */

      ffwrite(fptr->fileptr, nwrite, cptr, status); /* write the data */
      ntodo -= nwrite;                /* decrement remaining number of bytes */
      cptr += nwrite;                  /* increment user buffer pointer */
      fptr->io_pos = filepos + nwrite; /* update the file position */
      fptr->last_io_op = IO_WRITE;     /* save type of last operation */

      if (fptr->io_pos >= fptr->filesize) /* at the EOF? */
      {
        fptr->filesize = fptr->io_pos;   /* increment the file size */

        /* initialize the current buffer with the correct fill value */
        if (fptr->hdutype == ASCII_TBL)
          memset(iobuffer[nbuff], 32, IOBUFLEN);  /* blank fill */
        else
          memset(iobuffer[nbuff],  0, IOBUFLEN);  /* zero fill */
      }
      else
      {
        /* read next record; must seek because last op was a write */
        ffseek(fptr->fileptr, fptr->io_pos);

        ffread(fptr->fileptr, IOBUFLEN, iobuffer[nbuff], status);
        fptr->io_pos += IOBUFLEN; 
        fptr->last_io_op = IO_READ;    /* save type of last operation */
      }

      /* copy remaining bytes from user buffer into current IO buffer */
      memcpy(iobuffer[nbuff], cptr, ntodo);
      dirty[nbuff] = TRUE;       /* mark record as having been modified */
      bufrecnum[nbuff] = recend; /* record number */
      bufptr[nbuff] = fptr;      /* file pointer associated with IO buffer */

      fptr->logfilesize = maxvalue(fptr->logfilesize, (recend + 1) * IOBUFLEN);
      fptr->bytepos = fptr->io_pos + ntodo;
    }
    else
    {
      /* bufpos is the starting position in IO buffer */
      bufpos = fptr->bytepos - (bufrecnum[fptr->curbuf] * IOBUFLEN);
      nspace = IOBUFLEN - bufpos;   /* amount of space left in the buffer */

      while (ntodo)
      {
        nwrite = minvalue(ntodo, nspace);

        /* copy bytes from user's buffer to the IO buffer */
        memcpy(iobuffer[fptr->curbuf] + bufpos, cptr, nwrite);
        ntodo -= nwrite;            /* decrement remaining number of bytes */
        cptr += nwrite;
        fptr->bytepos += nwrite;    /* increment file position pointer */
        dirty[fptr->curbuf] = TRUE; /* mark record as having been modified */

        if (ntodo)                  /* load next record into a buffer */
        {
          ffldrc(fptr, fptr->bytepos / IOBUFLEN, IGNORE_EOF, status);
          bufpos = 0;
          nspace = IOBUFLEN;
        }
      }
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpbytoff(fitsfile *fptr, /* I - FITS file pointer                   */
           long gsize,        /* I - size of each group of bytes         */
           long ngroups,      /* I - number of groups to write           */
           long offset,       /* I - size of gap between groups          */
           void *buffer,      /* I - buffer to be written                */
           int *status)       /* IO - error status                       */
/*
  put (write) the buffer of bytes to the output FITS file, with an offset
  between each group of bytes.  This function combines ffmbyt and ffpbyt
  for increased efficiency.
*/
{
    int bcurrent;
    long ii, ntodo, bufpos, nspace, nwrite, record;
    char *cptr, *ioptr;
    extern char iobuffer[NIOBUF][IOBUFLEN];
    extern long bufrecnum[NIOBUF]; 
    extern int dirty[NIOBUF];

    if (*status > 0)
       return(*status);

    cptr = buffer;
    bcurrent = fptr->curbuf;     /* number of the current IO buffer */
    record = bufrecnum[bcurrent];  /* zero-indexed record number */
    bufpos = fptr->bytepos - (record * IOBUFLEN); /* starting buffer pos. */
    nspace = IOBUFLEN - bufpos;  /* amount of space left in buffer */
    ioptr = iobuffer[bcurrent] + bufpos;  

    for (ii = 1; ii < ngroups; ii++)  /* write all but the last group */
    {
      /* copy bytes from user's buffer to the IO buffer */
      nwrite = minvalue(gsize, nspace);
      memcpy(ioptr, cptr, nwrite);
      cptr += nwrite;          /* increment buffer pointer */

      if (nwrite < gsize)        /* entire group did not fit */
      {
        dirty[bcurrent] = TRUE;  /* mark record as having been modified */
        record++;
        ffldrc(fptr, record, IGNORE_EOF, status);  /* load next record */
        bcurrent = fptr->curbuf;
        ioptr   = iobuffer[bcurrent];

        nwrite  = gsize - nwrite;
        memcpy(ioptr, cptr, nwrite);
        cptr   += nwrite;            /* increment buffer pointer */
        ioptr  += (offset + nwrite); /* increment IO buffer pointer */
        nspace = IOBUFLEN - offset - nwrite;  /* amount of space left */
      }
      else
      {
        ioptr  += (offset + nwrite);  /* increment IO bufer pointer */
        nspace -= (offset + nwrite);
      }

      if (nspace <= 0) /* beyond current record? */
      {
        dirty[bcurrent] = TRUE;
        record += ((IOBUFLEN - nspace) / IOBUFLEN); /* new record number */
        ffldrc(fptr, record, IGNORE_EOF, status);
        bcurrent = fptr->curbuf;

        bufpos = (-nspace) % IOBUFLEN; /* starting buffer pos */
        nspace = IOBUFLEN - bufpos;
        ioptr = iobuffer[bcurrent] + bufpos;  
      }
    }
      
    /* now write the last group */
    nwrite = minvalue(gsize, nspace);
    memcpy(ioptr, cptr, nwrite);
    cptr += nwrite;          /* increment buffer pointer */

    if (nwrite < gsize)        /* entire group did not fit */
    {
      dirty[bcurrent] = TRUE;  /* mark record as having been modified */
      record++;
      ffldrc(fptr, record, IGNORE_EOF, status);  /* load next record */
      bcurrent = fptr->curbuf;
      ioptr   = iobuffer[bcurrent];

      nwrite  = gsize - nwrite;
      memcpy(ioptr, cptr, nwrite);
    }

    dirty[bcurrent] = TRUE;    /* mark record as having been modified */
    fptr->bytepos = fptr->bytepos + (ngroups * gsize)
                                  + (ngroups - 1) * offset;
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgbyt(fitsfile *fptr,    /* I - FITS file pointer             */
           long nbytes,       /* I - number of bytes to read       */
           void *buffer,      /* O - buffer to read into           */
           int *status)       /* IO - error status                 */
/*
  get (read) the requested number of bytes from the file, starting at
  the current file position.  Read large blocks of data directly from disk;
  read smaller segments via intermediate IO buffers to improve efficiency.
*/
{
    int ii;
    long filepos, recstart, recend, ntodo, bufpos, nspace, nread;
    char *cptr;

    extern fitsfile *bufptr[NIOBUF];
    extern char iobuffer[NIOBUF][IOBUFLEN];
    extern long bufrecnum[NIOBUF]; 
    extern int dirty[NIOBUF];

    if (*status > 0)
       return(*status);

    cptr = buffer;

    if (nbytes >= MINDIRECT)
    {
      /* read large blocks of data directly from disk instead of via buffers */
      filepos = fptr->bytepos;       /* save the read starting position */

      recstart = bufrecnum[fptr->curbuf];          /* starting record */
      recend = (filepos + nbytes - 1) / IOBUFLEN;  /* ending record   */

      for (ii = 0; ii < NIOBUF; ii++) /* flush any affected buffers to disk */
      {
        if (dirty[ii] && bufptr[ii] == fptr && 
            bufrecnum[ii] >= recstart && bufrecnum[ii] <= recend)
            {
              ffbfwt(ii, status);    /* flush modified buffer to disk */
            }
      }

       /* move to the correct read position; must seek if last op was write */
      if (fptr->io_pos != filepos || fptr->last_io_op == IO_WRITE)
         ffseek(fptr->fileptr, filepos);

      ffread(fptr->fileptr, nbytes, cptr, status); /* read the data */
      fptr->io_pos = filepos + nbytes; /* update the file position */
      fptr->last_io_op = IO_READ;      /* save type of last operation */
    }
    else
    {
      /* read small chucks of data using the IO buffers for efficiency */

      /* bufpos is the starting position in IO buffer */
      bufpos = fptr->bytepos - (bufrecnum[fptr->curbuf] * IOBUFLEN);
      nspace = IOBUFLEN - bufpos;   /* amount of space left in the buffer */

      ntodo =  nbytes;
      while (ntodo)
      {
        nread  = minvalue(ntodo, nspace);

        /* copy bytes from IO buffer to user's buffer */
        memcpy(cptr, iobuffer[fptr->curbuf] + bufpos, nread);
        ntodo -= nread;            /* decrement remaining number of bytes */
        cptr  += nread;
        fptr->bytepos += nread;    /* increment file position pointer */

        if (ntodo)                  /* load next record into a buffer */
        {
          ffldrc(fptr, fptr->bytepos / IOBUFLEN, REPORT_EOF, status);
          bufpos = 0;
          nspace = IOBUFLEN;
        }
      }
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgbytoff(fitsfile *fptr, /* I - FITS file pointer                   */
           long gsize,        /* I - size of each group of bytes         */
           long ngroups,      /* I - number of groups to write           */
           long offset,       /* I - size of gap between groups          */
           void *buffer,      /* I - buffer to be written                */
           int *status)       /* IO - error status                       */
/*
  get (read) the requested number of bytes from the file, starting at
  the current file position.  This function combines ffmbyt and ffgbyt
  for increased efficiency.
*/
{
    int bcurrent;
    long ii, ntodo, bufpos, nspace, nread, record;
    char *cptr, *ioptr;
    extern char iobuffer[NIOBUF][IOBUFLEN];
    extern long bufrecnum[NIOBUF]; 

    if (*status > 0)
       return(*status);

    cptr = buffer;
    bcurrent = fptr->curbuf;     /* number of the current IO buffer */
    record = bufrecnum[bcurrent];  /* zero-indexed record number */
    bufpos = fptr->bytepos - (record * IOBUFLEN); /* starting buffer pos. */
    nspace = IOBUFLEN - bufpos;  /* amount of space left in buffer */
    ioptr = iobuffer[bcurrent] + bufpos;  

    for (ii = 1; ii < ngroups; ii++)  /* read all but the last group */
    {
      /* copy bytes from IO buffer to the user's buffer */
      nread = minvalue(gsize, nspace);
      memcpy(cptr, ioptr, nread);
      cptr += nread;          /* increment buffer pointer */

      if (nread < gsize)        /* entire group did not fit */
      {
        record++;
        ffldrc(fptr, record, REPORT_EOF, status);  /* load next record */
        bcurrent = fptr->curbuf;
        ioptr   = iobuffer[bcurrent];

        nread  = gsize - nread;
        memcpy(cptr, ioptr, nread);
        cptr   += nread;            /* increment buffer pointer */
        ioptr  += (offset + nread); /* increment IO buffer pointer */
        nspace = IOBUFLEN - offset - nread;  /* amount of space left */
      }
      else
      {
        ioptr  += (offset + nread);  /* increment IO bufer pointer */
        nspace -= (offset + nread);
      }

      if (nspace <= 0) /* beyond current record? */
      {
        record += ((IOBUFLEN - nspace) / IOBUFLEN); /* new record number */
        ffldrc(fptr, record, REPORT_EOF, status);
        bcurrent = fptr->curbuf;

        bufpos = (-nspace) % IOBUFLEN; /* starting buffer pos */
        nspace = IOBUFLEN - bufpos;
        ioptr = iobuffer[bcurrent] + bufpos;  
      }
    }

    /* now read the last group */
    nread = minvalue(gsize, nspace);
    memcpy(cptr, ioptr, nread);
    cptr += nread;          /* increment buffer pointer */

    if (nread < gsize)        /* entire group did not fit */
    {
      record++;
      ffldrc(fptr, record, REPORT_EOF, status);  /* load next record */
      bcurrent = fptr->curbuf;
      ioptr   = iobuffer[bcurrent];

      nread  = gsize - nread;
      memcpy(cptr, ioptr, nread);
    }

    fptr->bytepos = fptr->bytepos + (ngroups * gsize)
                                  + (ngroups - 1) * offset;
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffldrc(fitsfile *fptr,        /* I - FITS file pointer             */
           long record,           /* I - record number to be loaded    */
           int err_mode,          /* I - 1=ignore EOF, 0 = return EOF error */
           int *status)           /* IO - error status                 */
{
/*
  low-level routine to load a specified record from a file into
  a physical buffer, if it is not already loaded.  Reset all
  pointers to make this the new current record for that file.
  Update ages of all the physical buffers.
*/
    int ibuff, nbuff, ii;
    long rstart;

    extern char iobuffer[NIOBUF][IOBUFLEN];
    extern fitsfile *bufptr[NIOBUF];
    extern long bufrecnum[NIOBUF]; 
    extern int dirty[NIOBUF], ageindex[NIOBUF];

    /* check if record is already loaded in one of the buffers */
    /* search from youngest to oldest buffer for efficiency */

    for (ibuff = NIOBUF - 1; ibuff >= 0; ibuff--)
    {
      nbuff = ageindex[ibuff];
      if (bufptr[nbuff] == fptr && record == bufrecnum[nbuff])
         goto updatebuf;  /* use 'goto' for efficiency */
    }

    /* record is not already loaded */

    rstart = record * IOBUFLEN;

    if ( !err_mode && (rstart >= fptr->logfilesize) )  /* EOF? */
         return(*status = END_OF_FILE);

    if (ffwhbf(fptr, &nbuff) < 0)  /* which buffer should we reuse? */
       return(*status = TOO_MANY_FILES); 

    if (dirty[nbuff])
       ffbfwt(nbuff, status); /* write dirty buffer to disk */

    if (rstart >= fptr->filesize)  /* EOF? */
    {
      /* initialize an empty buffer with the correct fill value */
      if (fptr->hdutype == ASCII_TBL)
         memset(iobuffer[nbuff], 32, IOBUFLEN); /* blank fill */
      else
         memset(iobuffer[nbuff],  0, IOBUFLEN);  /* zero fill */

      fptr->logfilesize = maxvalue(fptr->logfilesize, rstart + IOBUFLEN);
      dirty[nbuff] = TRUE;  /* mark record as having been modified */
    }
    else  /* not EOF, so read record from disk */
    {
      if (fptr->io_pos != rstart || fptr->last_io_op == IO_WRITE)
           ffseek(fptr->fileptr, rstart);

      ffread(fptr->fileptr, IOBUFLEN, iobuffer[nbuff], status);
      fptr->io_pos = rstart + IOBUFLEN;  /* set new IO position */
      fptr->last_io_op = IO_READ;        /* save type of last operation */
    }

    bufptr[nbuff] = fptr;   /* file pointer for this buffer */
    bufrecnum[nbuff] = record;   /* record number contained in buffer */

updatebuf:

    fptr->curbuf = nbuff; /* this is the current buffer for this file */

    if (ibuff < 0)
    { 
      /* find the current position of the buffer in the age index */
      for (ibuff = 0; ibuff < NIOBUF; ibuff++)
         if (ageindex[ibuff] == nbuff)
            break;  
    }

    /* increment the age of all the buffers that were younger than it */
    for (ibuff++; ibuff < NIOBUF; ibuff++)
      ageindex[ibuff - 1] = ageindex[ibuff];

    ageindex[NIOBUF - 1] = nbuff; /* this is now the youngest buffer */

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffwhbf(fitsfile *fptr,        /* I - FITS file pointer             */
           int *nbuff)            /* O - which buffer to use           */
{
/*
  decide which buffer to (re)use to hold a new file record
*/
    int ii, ibuff;
    static int ageinit = 0;

    extern fitsfile *bufptr[NIOBUF];
    extern int ageindex[NIOBUF];

    if (!ageinit)  /* first time thru, initialize default age of buffers */
    {
       for (ii = 0; ii < NIOBUF; ii++)
           ageindex[ii] = ii;
       ageinit = 1;
    }

    for (ii = 0; ii < NIOBUF; ii++)
    {
      ibuff = ageindex[ii];  /* search from the oldest to youngest buffer */

      if (bufptr[ibuff] == NULL ||         /* if buffer is empty, or    */
          bufptr[ibuff]->curbuf != ibuff)  /* is not the current buffer */
         return(*nbuff = ibuff);           /* then choose this buffer   */
    }

    /* all the buffers are locked, so we have to reuse the current one */
    /* Returns -1 if there is no current buffer (i.e. too many open files) */
    return(*nbuff = fptr->curbuf);
}
/*--------------------------------------------------------------------------*/
int ffflsh(fitsfile *fptr,        /* I - FITS file pointer           */
           int clearbuf,          /* I - also clear buffer contents? */
           int *status)           /* IO - error status               */
{
/*
  flush all dirty IO buffers associated with the file to disk
*/
    int ii;

    extern fitsfile *bufptr[NIOBUF];
    extern long bufrecnum[NIOBUF]; 
    extern int dirty[NIOBUF];

    for (ii = 0; ii < NIOBUF; ii++)
    {
      if (bufptr[ii] == fptr)
      {
        if (dirty[ii])        /* flush modified buffer to disk */
           ffbfwt(ii, status);

        if (clearbuf)
          bufptr[ii] = NULL;  /* set contents of buffer as undefined */
      }
    }

    ffflushx(fptr->fileptr);  /* flush system buffers to disk */
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffbfwt(int nbuff,             /* I - which buffer to write          */
           int *status)           /* IO - error status                  */
{
/*
  write contents of buffer to disk;  If the position of the buffer
  is beyond the current EOF, then the file may need to be extended
  with fill values, and/or with the contents of some of the other
  i/o buffers.
*/
    fitsfile *fptr;
    int  ii,ibuff;
    long jj, irec, minrec, nloop, filepos;

    static char zeros[IOBUFLEN];  /*  initialized to zero by default */

    extern char iobuffer[NIOBUF][IOBUFLEN];
    extern fitsfile *bufptr[NIOBUF];
    extern long bufrecnum[NIOBUF]; 
    extern int dirty[NIOBUF];

    fptr = bufptr[nbuff];
    filepos = bufrecnum[nbuff] * IOBUFLEN;

    if (filepos <= fptr->filesize)
    {
      /* record is located within current file, so just write it */

      /* move to the correct write position; must seek if last op was a read */
      if (fptr->io_pos != filepos || fptr->last_io_op == IO_READ)
         ffseek(fptr->fileptr, filepos);

      ffwrite(fptr->fileptr, IOBUFLEN, iobuffer[nbuff], status);
      fptr->io_pos = filepos + IOBUFLEN;
      fptr->last_io_op = IO_WRITE;     /* save type of last operation */

      if (filepos == fptr->filesize)   /* appended new record? */
         fptr->filesize += IOBUFLEN;   /* increment the file size */

      dirty[nbuff] = FALSE;
    }

    else  /* if record is beyond the EOF, append any other records */ 
          /* and/or insert fill values if necessary */
    {
      /* move to EOF; must seek if last op was a read */
      if (fptr->io_pos != fptr->filesize || fptr->last_io_op == IO_READ)
         ffseek(fptr->fileptr, fptr->filesize);

      ibuff = NIOBUF;  /* initialize to impossible value */
      while(ibuff != nbuff) /* repeat until requested buffer is written */
      {
        minrec = fptr->filesize / IOBUFLEN;

        /* write lowest record beyond the EOF first */

        irec = bufrecnum[nbuff]; /* initially point to the requested buffer */
        ibuff = nbuff;

        for (ii = 0; ii < NIOBUF; ii++)
        {
          if (bufptr[ii] == fptr && bufrecnum[ii] >= minrec &&
            bufrecnum[ii] < irec)
          {
            irec = bufrecnum[ii];  /* found a lower record */
            ibuff = ii;
          }
        }

        filepos = irec * IOBUFLEN;    /* byte offset of record in file */

        /* append 1 or more fill records if necessary */
        if (filepos > fptr->filesize)
        {                    
          nloop = (filepos - (fptr->filesize)) / IOBUFLEN; 

          for (jj = 0; jj < nloop; jj++)
            ffwrite(fptr->fileptr, IOBUFLEN, zeros, status);

          fptr->filesize = filepos;   /* increment the file size */
        } 

        /* write the buffer itself */
        ffwrite(fptr->fileptr, IOBUFLEN, iobuffer[ibuff], status);
        dirty[ibuff] = FALSE;

        fptr->filesize += IOBUFLEN;     /* increment the file size */
      } /* loop back if more buffers need to be written */

      fptr->io_pos = fptr->filesize;  /* currently positioned at EOF */
      fptr->last_io_op = IO_WRITE;    /* save type of last operation */
    }
    return(*status);       
}
/*--------------------------------------------------------------------------*/
int ffgrsz( fitsfile *fptr, /* I - FITS file pionter                        */
            long *nrows,    /* O - optimal number of rows to access         */
            int  *status)   /* IO - error status                            */
/*
  Returns an optimal value for the number of rows that should be 
  read or written at one time in a binary table for maximum efficiency.
  Accessing more rows than this may cause excessive flushing and 
  rereading of buffers to/from disk.
*/
{
    int ii, jj, unique, nfiles;

    /* There are NIOBUF internal buffers available each IOBUFLEN bytes long. */

    if (fptr->datastart == DATA_UNDEFINED)
      if ( ffrdef(fptr, status) > 0)   /* rescan header to get col struct */
           return(*status);

    /* determine how many different FITS files are currently open */
    nfiles = 0;
    for (ii = 0; ii < NIOBUF; ii++)
    {
      unique = TRUE;
      for (jj = 0; jj < ii; jj++)
      {
        if (!bufptr[ii] || bufptr[ii] == bufptr[jj])
        {
          unique = FALSE;
          break;
        }
      }

      if (unique)
        nfiles++;
    }

    /* one buffer (at least) is always allocated to each open file */
    *nrows = ((NIOBUF - nfiles) * IOBUFLEN) / maxvalue(1, fptr->rowlength);
    *nrows = maxvalue(1, *nrows); 
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
           long byteloc,   /* I - position within file to start reading     */
           long nvals,     /* I - number of pixels to read                  */
           long incre,     /* I - byte increment between pixels             */
           unsigned char *values, /* O - returned array of values           */
           int *status)    /* IO - error status                             */
/*
  get (read) the array of values from the FITS file, doing machine dependent
  format conversion (e.g. byte-swapping) if necessary.
*/
{
    long postemp;

    if (incre == 1)      /* read all the values at once (contiguous bytes) */
    {
        if (nvals < MINDIRECT)  /* read normally via IO buffers */
        {
           ffmbyt(fptr, byteloc, REPORT_EOF, status);
           ffgbyt(fptr, nvals, values, status);
        }
        else            /* read directly from disk, bypassing IO buffers */
        {
           postemp = fptr->bytepos;       /* store current file position */
           fptr->bytepos = byteloc;       /* set to the desired position */
           ffgbyt(fptr, nvals, values, status);
           fptr->bytepos = postemp;       /* reset to original position */
        }
    }
    else         /* have to read each value individually (not contiguous ) */
    {
        ffmbyt(fptr, byteloc, REPORT_EOF, status);
        ffgbytoff(fptr, 1, nvals, incre - 1, values, status);
    }
    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgi2b(fitsfile *fptr,  /* I - FITS file pointer                        */
           long byteloc,    /* I - position within file to start reading    */
           long nvals,      /* I - number of pixels to read                 */
           long incre,      /* I - byte increment between pixels            */
           short *values,   /* O - returned array of values                 */
           int *status)     /* IO - error status                            */
/*
  get (read) the array of values from the FITS file, doing machine dependent
  format conversion (e.g. byte-swapping) if necessary.
*/
{
    long postemp;

    if (incre == 2)      /* read all the values at once (contiguous bytes) */
    {
        if (nvals * 2 < MINDIRECT)  /* read normally via IO buffers */
        {
           ffmbyt(fptr, byteloc, REPORT_EOF, status);
           ffgbyt(fptr, nvals * 2, values, status);
        }
        else            /* read directly from disk, bypassing IO buffers */
        {
           postemp = fptr->bytepos;       /* store current file position */
           fptr->bytepos = byteloc;       /* set to the desired position */
           ffgbyt(fptr, nvals * 2, values, status);
           fptr->bytepos = postemp;       /* reset to original position */
        }
    }
    else         /* have to read each value individually (not contiguous ) */
    {
        ffmbyt(fptr, byteloc, REPORT_EOF, status);
        ffgbytoff(fptr, 2, nvals, incre - 2, values, status);
    }

#if BYTESWAPPED == TRUE

    ffswap2(values, nvals);    /* reverse order of bytes in each value */

#endif

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgi4b(fitsfile *fptr,  /* I - FITS file pointer                        */
           long byteloc,    /* I - position within file to start reading    */
           long nvals,      /* I - number of pixels to read                 */
           long incre,      /* I - byte increment between pixels            */
           long *values,    /* O - returned array of values                 */
           int *status)     /* IO - error status                            */
/*
  get (read) the array of values from the FITS file, doing machine dependent
  format conversion (e.g. byte-swapping) if necessary.
*/
{
    long postemp;

    if (incre == 4)      /* read all the values at once (contiguous bytes) */
    {
        if (nvals * 4 < MINDIRECT)  /* read normally via IO buffers */
        {
           ffmbyt(fptr, byteloc, REPORT_EOF, status);
           ffgbyt(fptr, nvals * 4, values, status);
        }
        else            /* read directly from disk, bypassing IO buffers */
        {
           postemp = fptr->bytepos;       /* store current file position */
           fptr->bytepos = byteloc;       /* set to the desired position */
           ffgbyt(fptr, nvals * 4, values, status);
           fptr->bytepos = postemp;       /* reset to original position */
        }
    }
    else         /* have to read each value individually (not contiguous ) */
    {
        ffmbyt(fptr, byteloc, REPORT_EOF, status);
        ffgbytoff(fptr, 4, nvals, incre - 4, values, status);
    }

#if BYTESWAPPED == TRUE

    ffunswaplong(values, nvals);    /* reverse order of bytes in each value */

#endif

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffgr4b(fitsfile *fptr,  /* I - FITS file pointer                        */
           long byteloc,    /* I - position within file to start reading    */
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
    long ii, postemp;

    if (incre == 4)      /* read all the values at once (contiguous bytes) */
    {
        if (nvals * 4 < MINDIRECT)  /* read normally via IO buffers */
        {
           ffmbyt(fptr, byteloc, REPORT_EOF, status);
           ffgbyt(fptr, nvals * 4, values, status);
        }
        else            /* read directly from disk, bypassing IO buffers */
        {
           postemp = fptr->bytepos;       /* store current file position */
           fptr->bytepos = byteloc;       /* set to the desired position */
           ffgbyt(fptr, nvals * 4, values, status);
           fptr->bytepos = postemp;       /* reset to original position */
        }
    }
    else         /* have to read each value individually (not contiguous ) */
    {
        ffmbyt(fptr, byteloc, REPORT_EOF, status);
        ffgbytoff(fptr, 4, nvals, incre - 4, values, status);
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
           long byteloc,    /* I - position within file to start reading    */
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
    long ii, postemp;

    if (incre == 8)      /* read all the values at once (contiguous bytes) */
    {
        if (nvals * 8 < MINDIRECT)  /* read normally via IO buffers */
        {
           ffmbyt(fptr, byteloc, REPORT_EOF, status);
           ffgbyt(fptr, nvals * 8, values, status);
        }
        else            /* read directly from disk, bypassing IO buffers */
        {
           postemp = fptr->bytepos;       /* store current file position */
           fptr->bytepos = byteloc;       /* set to the desired position */
           ffgbyt(fptr, nvals * 8, values, status);
           fptr->bytepos = postemp;       /* reset to original position */
        }
    }
    else         /* have to read each value individually (not contiguous ) */
    {
        ffmbyt(fptr, byteloc, REPORT_EOF, status);
        ffgbytoff(fptr, 8, nvals, incre - 8, values, status);
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
    if (incre == 1)      /* write all the values at once (contiguous bytes) */

        ffpbyt(fptr, nvals, values, status);

    else         /* have to write each value individually (not contiguous ) */

        ffpbytoff(fptr, 1, nvals, incre - 1, values, status);

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
#if BYTESWAPPED == TRUE

    ffswap2(values, nvals);  /* reverse order of bytes in each value */

#endif

    if (incre == 2)      /* write all the values at once (contiguous bytes) */

        ffpbyt(fptr, nvals * 2, values, status);

    else         /* have to write each value individually (not contiguous ) */

        ffpbytoff(fptr, 2, nvals, incre - 2, values, status);

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
#if BYTESWAPPED == TRUE

    ffswaplong(values, nvals); /* reverse order of bytes in each value */

#endif

    if (incre == 4)      /* write all the values at once (contiguous bytes) */

        ffpbyt(fptr, nvals * 4, values, status);

    else         /* have to write each value individually (not contiguous ) */

        ffpbytoff(fptr, 4, nvals, incre - 4, values, status);

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
    long ii;

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
        ffswapfloat(values, nvals); /* swap bytes in each IEEE value */

#elif BYTESWAPPED == TRUE

    ffswapfloat(values, nvals); /* reverse order of bytes in each value */

#endif

    if (incre == 4)      /* write all the values at once (contiguous bytes) */

        ffpbyt(fptr, nvals * 4, values, status);

    else         /* have to write each value individually (not contiguous ) */

        ffpbytoff(fptr, 4, nvals, incre - 4, values, status);

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
    long ii;

#if MACHINE == VAXVMS

    ii = nvals;                      /* call VAX macro routine to convert */
    ieevpd(values, values, &ii);     /* from D float -> IEEE float        */

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

        ffpbytoff(fptr, 8, nvals, incre - 8, values, status);

    return(*status);
}

